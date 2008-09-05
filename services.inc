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
  $output = '<?xml version="1.0"?>'."\n".
    '<!DOCTYPE cross-domain-policy SYSTEM "http://www.macromedia.com/xml/dtds/cross-domain-policy.dtd">'."\n".
    '<cross-domain-policy>'."\n".
    '  <allow-access-from domain="'. check_plain($_SERVER['HTTP_HOST']) .'" />'."\n".
    '  <allow-access-from domain="*.'. check_plain($_SERVER['HTTP_HOST']) .'" />'."\n".
    '</cross-domain-policy>';

  // Include the XML header before the content
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
  $server_info = _services_server_get();

  // Look for custom error handling function.
  // Should be defined in each server module.
  if ($server_info && module_hook($server_info->module, 'server_error')) {
    return module_invoke($server_info->module, 'server_error', $message);
  }

  // No custom error handling function found.
  return $message;
}

/**
 * Callback for server endpoint
 */
function _services_server($server_path = NULL) {
  // Find which module the server is part of
  foreach (module_implements('server_info') as $module) {
    $info = module_invoke($module, 'server_info');
    if ($info['#path'] == $server_path) {

      // Call the server
      $server_info = new stdClass();
      $server_info->module = $module;
      $server_info->drupal_path = getcwd();
      _services_server_get($server_info);
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
 * Get the into from the current server. If none was yet set,
 * then set the server given as current.
 *
 * @param $server_info
 *   Object (optional). The current server info.
 * @return
 *   Object. The current server info. Or FALSE if none was set
 *   and none was given.
 */
function _services_server_get($server_info = NULL) {
  static $info;
  if (empty($info)) {
    if (!empty($server_info)) {
      $info = $server_info;
    }
    else {
      return FALSE;
    }
  }
  return $info;
}

/**
 * Execute a given service.
 */
function _services_service_call($method_name, $args = array()) {
  // Check that method exists.
  if (!$method = _services_service_get($method_name)) {
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
  }

  // Check the OAuth authentication
  $session_backup = NULL;
  if (empty($method['#no_auth']) and variable_get('services_use_oauth', TRUE)) {
    $consumer_key = array_shift($args);
    $token_access = array_shift($args);
    if (empty($consumer_key)) {
      return services_error(t('Invalid consumer key.'));
    }
    elseif (empty($token_access)) {
      return services_error(t('Invalid access token.'));
    }
    $session_backup = _services_session_load($sessid);
  }

  // Load the file where the service callback is
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
  $server_info = _services_server_get();
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
 * Get the into from a service.
 *
 * @param $method_name
 *   String. The service name
 * @return
 *   Object. The service info.
 */
function _services_service_get($method_name) {
  static $method_cache;
  if (!isset($method_cache[$method_name])) {
    foreach (services_service_get_all() as $method) {
      if ($method_name == $method['#method']) {
        $method_cache[$method_name] = $method;
        break;
      }
    }
  }
  return $method_cache[$method_name];
}

/**
 * Build a complete list of all services available,
 *
 * @note This should probably be cached in drupal cache.
 */
function services_service_get_all() {
  static $methods_cache;
  if (!isset($methods_cache)) {
    $methods = module_invoke_all('service');
    foreach ($methods as $key => $method) {
      // Add the access functions of each service
      if (!isset($methods[$key]['#access callback'])) {
        $methods[$key]['#access callback'] = 'user_access';
        if (!isset($methods[$key]['#access arguments'])) {
          $methods[$key]['#access arguments'] = array('access services');
        }
      }

      if (!isset($methods[$key]['#args'])) {
        $methods[$key]['#args'] = array();
      }

      // Add OAuth fields in case the OAuth keys are required
      if (empty($methods[$key]['#no_auth']) and variable_get('services_use_oauth', TRUE)) {
        array_unshift($methods[$key]['#args'],
          array(
            '#name'         => 'oauth_consumer_key',
            '#type'         => 'string',
            '#description'  => t('OAuth consumer key.'),
          ),
          array(
            '#name'         => 'oauth_token_access',
            '#type'         => 'string',
            '#description'  => t('OAuth access token.'),
          )
        );
      }

      // Set defaults for args
      foreach ($methods[$key]['#args'] as $arg_key => $arg) {
        if (is_array($arg)) {
          if (!isset($arg['#optional'])) {
            $methods[$key]['#args'][$arg_key]['#optional'] = FALSE;
          }
        }
        else {
          $arr_arg = array();
          $arr_arg['#name'] = t('unnamed');
          $arr_arg['#type'] = $arg;
          $arr_arg['#description'] = t('No description given.');
          $arr_arg['#optional'] = FALSE;
          $methods[$key]['#args'][$arg_key] = $arr_arg;
        }
      }
      reset($methods[$key]['#args']);
    }
    $methods_cache = $methods;
  }
  return $methods_cache;
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
