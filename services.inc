<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 * All functions that are not hooks or constantly accessed.
 */

/**
 * Callback for crossdomain.xml
 */
function _services_crossdomain_xml() {
  global $base_url;
  $output = '<!DOCTYPE cross-domain-policy SYSTEM "http://www.macromedia.com/xml/dtds/cross-domain-policy.dtd">'."\n";
  $output .= '<cross-domain-policy>'."\n";
  $output .= '  <allow-access-from domain="'. check_plain($_SERVER['HTTP_HOST']) .'" />'."\n";
  $output .= '  <allow-access-from domain="*.'. check_plain($_SERVER['HTTP_HOST']) .'" />'."\n";
  $keys = services_get_keys();

  foreach ($keys as $key) {
    if (!empty($key->domain)) {
      $output .= '  <allow-access-from domain="'. check_plain($key->domain) .'" />'."\n";
      $output .= '  <allow-access-from domain="*.'. check_plain($key->domain) .'" />'."\n";
    }
  }

  $output .= '</cross-domain-policy>';

  // Include the XML header before the content
  $output = '<?xml version="1.0"?>'."\n". $output;
  header('Connection: close');
  header('Content-Length: '. drupal_strlen($output));
  header('Content-Type: text/xml');
  header('Date: '. date('r'));

  // Print the XML
  echo $output;
  exit;
}

/**
 * Prepare an error message for returning to the XMLRPC caller.
 *
 * @param $message
 *   String. The error message.
 * @return
 *   String. Depends on the services server.
 */
function services_error($message) {
  $server_info = services_get_server_info();

  // Look for custom error handling function.
  // Should be defined in each server module.
  if ($server_info && module_hook($server_info->module, 'server_error')) {
    return module_invoke($server_info->module, 'server_error', $message);
  }

  // No custom error handling function found.
  return $message;
}

/**
 * Check it the given key is valud.
 *
 * @param $kid
 *   Number. Key ID.
 * @return
 *   Boolean. TRUE if the key is valid.
 */
function _services_key_validate($kid, $timestamp, $domain, $nonce, $method_name, $hash_parameters, $hash) {
  $hash_parameters = array_merge(array($timestamp, $domain, $nonce, $method_name), $hash_parameters);
  $rehash = hash_hmac("sha256", implode(';', $hash_parameters), $kid);
  return $rehash == $hash;
}

/**
 * This is the magic function through which all remote method calls must pass.
 */
function services_method_call($method_name, $args = array(), $ignore_hash = FALSE) {
  $method = services_method_get($method_name);

  // Check that method exists.
  if (empty($method)) {
    return services_error(t('Method %name does not exist.', array('%name' => $method_name)));
  }

  // Check for missing args and identify if arg is required in the hash.
  $hash_parameters = array();
  foreach ($method['#args'] as $key => $arg) {
    if (!$arg['#optional']) {
      if (!is_numeric($args[$key]) and empty($args[$key])) {
        return services_error(t('Missing required arguments.'));
      }
    }

    // Key is part of the hash
  if ($arg['#signed'] == TRUE and variable_get('services_use_key', TRUE)) {
      if (is_numeric($args[$key]) or !empty($args[$key])) {
        if (is_array($args[$key]) || is_object($args[$key])){
          $hash_parameters[] = serialize($args[$key]);
        }
        else{
          $hash_parameters[] = $args[$key];
        }
      }
      else{
        $hash_parameters[] = '';
      }
    }
  }

  if ($method['#key'] and variable_get('services_use_key', TRUE)) {
    $hash = array_shift($args);
    $domain = array_shift($args);
    $timestamp = array_shift($args);
    $nonce = array_shift($args);

    $expiry_time = $timestamp + variable_get('services_key_expiry', 30);

    if ($expiry_time < time()) {
      return services_error(t('Token has expired.'));
    }

    // Still in time but has it been used before
    if (db_result(db_query("SELECT count(*) FROM {services_timestamp_nonce}
        WHERE domain = '%s' AND timestamp = %d AND nonce = '%s'",
        $domain, $timestamp, $nonce))) {
      return services_error(t('Token has been used previously for a request.'));
    }
    else{
      db_query("INSERT INTO {services_timestamp_nonce} (domain, timestamp, nonce)
        VALUES ('%s', %d, '%s')", $domain, $timestamp, $nonce);
    }

    $api_key = db_result(db_query("SELECT kid FROM {services_keys} WHERE domain = '%s'", $domain));

    if (!_services_key_validate($api_key, $timestamp, $domain, $nonce, $method_name, $hash_parameters, $hash)) {
      return services_error(t('Invalid API key.'));
    }
  }

  // Add additonal processing for methods requiring authentication
  $session_backup = NULL;
  if ($method['#auth'] and variable_get('services_use_sessid', TRUE)) {
    $sessid = array_shift($args);
    if (empty($sessid)) {
      return services_error(t('Invalid sessid.'));
    }
    $session_backup = _services_session_load($sessid);
  }

  // Load the proper file
  if ($file = $method['#file']) {
    module_load_include($file['file'], $file['module']);
  }

  // Check access
  $access_arguments = isset($method['#access arguments']) ? $method['#access arguments'] : $args;
  // Call default or custom access callback
  if (call_user_func_array($method['#access callback'], $access_arguments) != TRUE) {
    return services_error(t('Access denied.'));
  }

  // Change working directory to drupal root to call drupal function,
  // then change it back to server module root to handle return.
  $server_root = getcwd();
  $server_info = services_get_server_info();
  if ($server_info) {
    chdir($server_info->drupal_path);
  }
  $result = call_user_func_array($method['#callback'], $args);
  if ($server_info) {
    chdir($server_root);
  }

  // Add additonal processing for methods requiring authentication.
  if ($session_backup !== NULL) {
    _services_session_unload($session_backup);
  }

  return $result;
}

/**
 * Callback for server endpoint
 */
function _services_server($server_path = NULL) {
  // Find which module the server is part of
  foreach (module_implements('server_info') as $module) {
    $info = module_invoke($module, 'server_info');
    if ($info['#path'] == $server_path) {

      // call the server
      services_set_server_info($module);
      print module_invoke($module, 'server');

      // Do not let this output
      module_invoke_all('exit');
      exit;
    }
  }
  // return 404 if the service doesn't exist
  drupal_not_found();
}

/**
 * Backup current session data and import user session.
 */
function _services_session_load($sessid) {
  global $user;

  // If user's session is already loaded, just return current user's data
  if ($user->sid == $sessid) {
    return $user;
  }

  // Make backup of current user and session data
  $backup = $user;
  $backup->session = session_encode();

  // Empty current session data
  $_SESSION = array();

  // Some client/servers, like XMLRPC, do not handle cookies, so imitate it to make sess_read() function try to look for user,
  // instead of just loading anonymous user :).
  if (!isset($_COOKIE[session_name()])) $_COOKIE[session_name()] = $sessid;

  // Load session data
  sess_read($sessid);

  // Check if it really loaded user and, for additional security, if user was logged from the same IP. If not, then revert automatically.
  if ($user->sid != $sessid or $user->hostname != $backup->hostname) {
    _services_session_unload($backup);
    return NULL;
  }

  // Prevent saving of this impersonation in case of unexpected failure.
  session_save_session(FALSE);

  return $backup;
}

/**
 * Revert to previously backuped session.
 */
function _services_session_unload($backup) {
  global $user;

  // No point in reverting if it's the same user's data
  if ($user->sid == $backup->sid) {
    return;
  }

  // Some client/servers, like XMLRPC, do not handle cookies, so imitate it to make sess_read() function try to look for user,
  // instead of just loading anonymous user :).
  if (!isset($_COOKIE[session_name()])) $_COOKIE[session_name()] = $sessid;

  // Save current session data
  sess_write($user->sid, session_encode());

  // Empty current session data
  $_SESSION = array();

  // Revert to previous user and session data
  $user = $backup;
  session_decode($user->session);

  session_save_session(TRUE);
}
