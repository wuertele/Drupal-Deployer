<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  OAuth token integration.
 */

/**
 * Initialize and store an OAuthServer object.
 *
 * Here we initialize all signature objects - yAy this module supports all 3 supported signature methods
 */
function _services_oauth_server_init() {
  static $server;
  if (empty($server)) {
    module_load_include('oauth_lib.php', 'services');
    /**
    * Database abstraction class
    */
    class DrupalOAuthDataStore extends OAuthDataStore {
      function lookup_consumer($consumer_key) {
      $result = db_query("SELECT * FROM {oauth_consumer} WHERE consumer_key='%s'", $consumer_key);
        if ($object = db_fetch_object($result)) {
          return new OAuthConsumer($object->consumer_key, $object->consumer_secret);
        }
        return NULL;
      }

      function lookup_token($consumer, $token_type, $token) {
        $result = db_query("SELECT * FROM {oauth_token}
          WHERE type='%s' AND consumer_key='%s' AND token_key = '%s'",
          $token_type, $consumer->key, $token);
        if ($object = db_fetch_object($result)) {
          return new OAuthToken($object->token_key, $object->token_secret);
        }
        return NULL;
      }

      function lookup_nonce($consumer, $token, $nonce, $timestamp) {
        $nonce_1 = db_result(db_query("SELECT nonce FROM {oauth_nonce}
          WHERE nonce_timestamp='%s'", $timestamp));
        if (!$nonce_1) {
          db_query("INSERT INTO {oauth_nonce} (nonce, nonce_timestamp)
            VALUES ('%s', %d)", $nonce, $timestamp);
          return NULL;
        }
        return $nonce_1;
      }

      function new_request_token($consumer, $application_key = NULL) {
        $user_id = db_result(db_query("SELECT uid FROM {oauth_consumer}
          WHERE consumer_key='%s'", $consumer->key));
        $token = new OAuthToken(user_password(32), user_password(32));
        db_query("INSERT INTO {oauth_token}
          (application_key, consumer_key, type, token_key, token_secret, uid)
          VALUES ('%s', '%s', '%s', '%s', '%s', %d)",
          $application_key, $consumer->key, 'request', $token->key, $token->secret, $user_id);
        return $token;
      }

      /**
      *
      * @param session_id
      *    is session id corresponding to logged in user
      *    we are storing session id in oauth_services table to keep track of users and issued access tokens
      *
      * @param user_id
      *    user_id is actually $user->id but we are fetching it from oauth_consumer table and using it to get session_id from sessions
      *    table and also storing its value to oauth_token table (corresponding to access token here)
      *
      */
      function new_access_token($request_token, $consumer, $user = NULL) {
        if ($object = db_fetch_object(db_query("SELECT * FROM {oauth_token}
          WHERE type='request' and token_key = '%s'", $request_token->key))) {
          if ($object->authorized) {
            $token = new OAuthToken(user_password(32), user_password(32));
            $user_id = db_result(db_query("SELECT uid FROM {oauth_consumer} WHERE consumer_key = '%s'", $consumer->key));
            $session_id = db_result(db_query("SELECT sid FROM {sessions} WHERE uid=%d", $user_id));
            if (variable_get('oauth_use_coolauth', 0)) {
              db_query("INSERT INTO {oauth_token}
                (application_key, consumer_key, type, token_key, token_secret, uid)
                VALUES ('%s', '%s', '%s', '%s', '%s', %d)",
                $object->application_key, $consumer->key, 'access', $token->key, $token->secret, $user_id);
            }
            else {
              db_query("INSERT INTO {oauth_token}
                (application_key, consumer_key, type, token_key, token_secret, uid)
                VALUES ('%s', '%s', '%s', '%s', '%s', %d)", 'NULL',
                $consumer->key, 'access', $token->key, $token->secret, $user_id);
            }
            db_query("DELETE FROM {oauth_token} WHERE type='request' AND token_key='%s'", $request_token->key);
            if (variable_get('oauth_use_with_services', 0)) {
              db_query("UPDATE {oauth_services} SET token_key = '%s' WHERE session_id= '%s'", $token->key, $session_id);
            }
            return $token;
          }
        }
        return NULL;
      }
    }

    // Create the instance of Server
    $server = new OAuthServer(new DrupalOAuthDataStore());
    $server->add_signature_method(new OAuthSignatureMethod_HMAC_SHA1());
    $server->add_signature_method(new OAuthSignatureMethod_PLAINTEXT());
    $server->add_signature_method(new OAuthSignatureMethod_RSA_SHA1());
  }
  return $server;
}

/**
 * Access the OAuth services
 */
function _services_token_access() {
  if (empty($_GET['oauth_token'])) {
    drupal_set_message('OAuth token is not included in request.', 'error');
    drupal_access_denied();
  }
  if (empty($_GET['oauth_consumer_key'])) {
    drupal_set_message('Please include a valid consumer key in your request.', 'error');
    drupal_access_denied();
  }
  $server = _services_oauth_server_init();

  // Remove the q variable from the query string, as it will break
  // the signature validation.
  $q = $_GET['q'];
  unset($_GET['q']);

  try {
    global $user;
    $req = OAuthRequest::from_request();
    $token = $server->fetch_access_token($req);
    parse_str($token);
    print $token;
  }
  catch (OAuthException $e) {
    print($e->getMessage() ."\n<hr />\n");
    print_r($req);
    die();
  }

  // Set the $_GET['q'] back to it's original value
  $_GET['q'] = $q;
}

/**
 * Authorize a request token.
 *
 * Redirects to login form if not logged in, and displays the grant access form once logged in.
 */
function _services_token_auth() {
  // Redirect to the right form, or present an error.
  global $user;
  if ($user->uid != 0) {
    if (user_access('authorize external services')) {
      return drupal_get_form('_services_token_auth_form');
    }
    else {
      drupal_set_message('error', t('You are not authorized to allow external services access to this system.'));
      drupal_access_denied();
    }
  }
  else {
    return drupal_get_form('user_login');
  }
}

/**
 * Form for granting access to the consumer
 *
 * Here user is asked to issue access/deny permission to
 * specific services as demanded by calling server
 *
 * @ingroup $form
 */
function _services_token_auth_form() {
  if($_GET['oauth_token'] == ''){
    drupal_set_message("error, OAuth token is not included in request. Whole API documentation for OAuth and cool auth is at http://drupal.org/node.");
    drupal_access_denied();
  }
  if($_GET['oauth_callback'] == ''){
    drupal_set_message("error, Please include a valid callback url in your request OR read API documentation over http://drupal.org");
    drupal_access_denied();
  }
  module_invoke('services');
  $services = services_get_all();

  #todo tripple check image reference.

  $form['oauth_callback'] = array(
    '#type' => 'hidden',
    '#value' => $_GET['oauth_callback']
  );
  $form['oauth_token'] = array(
    '#type' => 'hidden',
    '#value' => $_GET['oauth_token']
  );
  $form['oauth_consumer_key'] = array(
    '#type' => 'hidden',
    '#value' => $_GET['oauth_consumer_key'],
  );
  $form['oauth_nonce'] = array(
    '#type' => 'hidden',
    '#value' => $_GET['oauth_nonce'],
  );
  $form['oauth_nonce_timestamp'] = array(
    '#type' => 'hidden',
    '#value' => $_GET['oauth_timestamp'],
  );
  if(variable_get('oauth_use_with_services', 0)){
    $form['services'] = array(
    '#title' => t('Select services'),
    '#type' => 'fieldset',
  );

  foreach ( $services as $service ){
    $method_name = $service['#method'];

    $form['services'][$method_name] = array(
      '#title' => $service['#method'],
      '#type' => 'radios',
      '#options' => array( 0 => t('access'), 1 => t('block')),
      '#default_value' => 1,
    );
    }
  }
  $form['confirm'] = array(
    '#type' => 'submit',
    '#value' => t('Grant access'),
    '#weight' =>  10,
  );
  $form['#tree'] = TRUE;
  return $form;
}

/**
 * Generate a request token from the request
 */
function _services_token_request() {
  if (variable_get('services_use_coolauth', FALSE)) {
    if (empty($_GET['application_key'])) {
      drupal_set_message("error", t('Please use a valid application key in your request OR read whole API documentation carefully', TRUE));
      drupal_access_denied();
    }
    $application_key = $_GET['application_key'];
    $application_secret = db_result(db_query("SELECT application_secret FROM {oauth_cool_auth} WHERE application_key=%d", $application_key));
    if (!$application_secret) {
      drupal_set_message('error', t("Your Application key is not valid", TRUE));
      drupal_access_denied();
    }
    $required_hash = md5($application_secret . $_GET['oauth_nonce']);

    if ($_GET['application_sig'] == $required_hash) {
      $server = _services_oauth_server_init();
      // Remove the q variable from the query string, as it will break
      // the signature validation.
      $q = $_GET['q'];
      unset($_GET['q']);

      try {
        $req = OAuthRequest::from_request();
        $token = $server->fetch_request_token($req, $application_key);
        print $token;
        die();
      }
      catch (OAuthException $e) {
        print($e->getMessage() ."\n<hr />\n");

        print_r($req);
        die();
      }
      // Set the $_GET['q'] back to it's original value
      $_GET['q'] = $q;
    }
  }
  else {
    $server = _services_oauth_server_init();
    // Remove the q variable from the query string, as it will break
    // the signature validation.
    $q = $_GET['q'];
    unset($_GET['q']);

    try {
      $req = OAuthRequest::from_request();
      $token = $server->fetch_request_token($req, $application_key);
      print $token;
      die();
    }
    catch (OAuthException $e) {
      print($e->getMessage() ."\n<hr />\n");

      print_r($req);
      die();
    }
    // Set the $_GET['q'] back to it's original value
    $_GET['q'] = $q;
  }
}
