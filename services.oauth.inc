<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  OAuth token integration.
 */

/**
 * Return consumer object related to a user. If the consumer
 * does not exist, it will be created.
 *
 * @param $uid
 *   Number. User ID to retrieve consumer object for.
 * @return
 *   Object. OAuth service consumer.
 */
function _services_consumer_get($uid) {
  module_load_include('oauth_lib.php', 'services');

  $result = db_query('SELECT * FROM {services_consumer} WHERE uid = %d', $uid);
  if ($object = db_fetch_array($result)) {
    return new OAuthConsumer($object['consumer_key'], $object['consumer_secret']);
  }
  else {
    $sql = array(
      'uid'             => $uid,
      'consumer_key'    => user_password(32),
      'consumer_secret' => user_password(32),
    );
    drupal_write_record('services_consumer', $sql);
    return new OAuthConsumer($sql['consumer_key'], $sql['consumer_secret']);
  }
}

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
        $result = db_query("SELECT * FROM {services_consumer}
          WHERE consumer_key = '%s'", $consumer_key);
        if ($object = db_fetch_object($result)) {
          return new OAuthConsumer($object->consumer_key, $object->consumer_secret);
        }
        return NULL;
      }

      /**
       * Check if the token exists.
       *
       * @param $consumer
       *   Object. The service consumer information.
       * @param $token_type
       *   Strint. The type of the token: 'request' or 'access'.
       * @param $token
       *   Strint. The token value.
       * @return
       *   String or NULL. The existing token or NULL in
       *   case it doesnt exist.
       */
      function lookup_token($consumer, $token_type, $token) {
        $result = db_query("SELECT * FROM {services_token}
          WHERE type = '%s' AND consumer_key = '%s' AND token_key = '%s'",
          $token_type, $consumer->key, $token);
        if ($object = db_fetch_object($result)) {
          return new OAuthToken($object->token_key, $object->token_secret);
        }
        return NULL;
      }

      /**
       * Check if the nonce value exists. If not, generate one.
       *
       * @param $consumer
       *   Object. The service consumer information.
       * @param $token
       *   Strint. The current token.
       * @param $nonce
       *   Strint. A new nonce value, in case a one doesnt current exit.
       * @param $timestamp
       *   Number. The current time.
       * @return
       *   String or NULL. The existing nonce value or NULL in
       *   case it doesnt exist.
       */
      function lookup_nonce($consumer, $token, $nonce, $timestamp) {
        if (!$nonce_1 = db_result(db_query("SELECT nonce FROM {services_timestamp_nonce}
          WHERE timestamp = '%s'", $timestamp))) {
          $sql = array(
            'nonce'     => $nonce,
            'timestamp' => $timestamp
          );
          drupal_write_record('services_timestamp_nonce', $sql);
          return NULL;
        }
        return $nonce_1;
      }

      /**
       * Generate a new request token.
       *
       * @param $consumer
       *   Object. The service consumer information.
       * @param $application_key
       *   Strint. This application key.
       */
      function new_request_token($consumer, $application_key = NULL) {
        $user_id = db_result(db_query("SELECT uid FROM {services_consumer}
          WHERE consumer_key = '%s'", $consumer->key));
        $token = new OAuthToken(user_password(32), user_password(32));
        $sql = array(
          'application_key' => $application_key,
          'consumer_key'    => $consumer->key,
          'type'            => 'request',
          'token_key'       => $token->key,
          'token_secret'    => $token->secret,
          'uid'             => $user_id
        );
        drupal_write_record('services_token', $sql);
        return $token;
      }

      /**
       * Generate a new access token and delete the old request token.
       *
       * @param $request_token
       *   Strint. The old request token.
       * @param $consumer
       *   Object. The service consumer information.
       * @param $user
       *   Object (optional). The user information.
       */
      function new_access_token($request_token, $consumer, $user = NULL) {
        if ($object = db_fetch_object(db_query("SELECT * FROM {services_token}
          WHERE type = 'request' AND token_key = '%s'", $request_token->key))) {
          if ($object->authorized) {
            $token = new OAuthToken(user_password(32), user_password(32));
            $user_id = db_result(db_query("SELECT uid FROM {services_consumer}
              WHERE consumer_key = '%s'", $consumer->key));
            $sql = array(
              'application_key' => (variable_get('services_use_coolauth', 0) ? $object->application_key : 'NULL'),
              'consumer_key'    => $consumer->key,
              'type'            => 'access',
              'token_key'       => $token->key,
              'token_secret'    => $token->secret,
              'uid'             => $user_id
            );
            drupal_write_record('services_token', $sql);

            db_query("DELETE FROM {services_token} WHERE
              type = 'request' AND token_key = '%s'", $request_token->key);
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
 * Test the OAuth implementation.
 *
 * @ingroup form
 */
function _services_oauth_test() {
  global $user;
  $consumer = _services_consumer_get($user->uid);
  $form['oauth_consumer_key'] = array(
    '#default_value'  => $consumer->key,
    '#title'          => t('Consumer Key'),
    '#type'           => 'textfield',
  );
  $form['oauth_consumer_secret'] = array(
    '#default_value'  => $consumer->secret,
    '#title'          => t('Consumer Secret'),
    '#type'           => 'textfield',
  );
  $form['stage'] = array(
    '#default_value'  => 1,
    '#options'        => array(
      t('Resquest token'),
      t('Authentication token'),
      t('Access token'),
      t('Example service'),
    ),
    '#title'          => t('Stage'),
    '#type'           => 'radios',
  );
  $form['submit'] = array(
    '#type'           => 'submit',
    '#value'          => t('Test OAuth'),
  );
  return $form;
}

/**
 * Test the OAuth implementation. Test request token, auth token,
 * access token and a simple service test.
 *
 * @ingroup form
 */
function _services_oauth_test_submit(&$form, &$form_state) {
  module_load_include('oauth_lib.php', 'services');

  // Resquet token
  $url_options = array(
    'query'     => array(
      'oauth_consumer_key'    => $form_state['values']['oauth_consumer_key'],
      'oauth_consumer_secret' => $form_state['values']['oauth_consumer_secret'],
      'oauth_timestamp'       => time(),
    ),
    'absolute'  => TRUE,
  );
  $url  = url('services/request', $url_options);
  $test = drupal_http_request($url, array(), 'GET');
  if ($test->code == 200) {
    drupal_set_message(t('Resquest token test succeeded!'), 'ok');
    if ($form_state['values']['stage'] <= 0) {
      return;
    }
  }
  else {
    drupal_set_message(t('Resquest token test failed: %message', array('%message' => $test->data)), 'error');
    return;
  }

  // Authentication token
//   parse_str($test->data);
//   $url_options = array(
//     'query'     => array(
//       'oauth_token'         => $oauth_token,
//       'oauth_token_secret'  => $oauth_token_secret,
//       'oauth_callback'      => url('admin/services', array('absolute' => TRUE)),
//     ),
//     'absolute'  => TRUE,
//   );
//   $url  = url('services/auth', $url_options);
//   $test = drupal_http_request($url, array(), 'GET');
//   if ($test->code == 200) {
//     drupal_set_message(t('Authentication token test succeeded!'), 'ok');
//     if ($form_state['values']['stage'] <= 1) {
//       return;
//     }
//   }
//   else {
//     drupal_set_message(t('Authentication token test failed: %message', array('%message' => $test->data)), 'error');
//     return;
//   }

  // Access token
  parse_str($test->data);
  $url_options = array(
    'query'     => array(
      'oauth_token'         => $oauth_token,
      'oauth_consumer_key'  => $form_state['values']['oauth_consumer_key'],
      'oauth_timestamp'     => time(),
    ),
    'absolute'  => TRUE,
  );
  $url  = url('services/access', $url_options);
  $test = drupal_http_request($url, array(), 'GET');
  dpm($test);
  if ($test->code == 200) {
    drupal_set_message(t('Access token test succeeded!'), 'ok');
    if ($form_state['values']['stage'] <= 2) {
      return;
    }
  }
  else {
    drupal_set_message(t('Access token test failed: %message', array('%message' => $test->data)), 'error');
    return;
  }
}

/**
 * Access the OAuth services
 */
function _services_token_access() {
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
    if (empty($_GET['oauth_token'])) {
      drupal_set_message('OAuth token is not included in request.', 'error');
      return drupal_access_denied();
    }
    elseif (empty($_GET['oauth_callback'])) {
      drupal_set_message('Please include a valid callback url in your request.', 'error');
      return drupal_access_denied();
    }
    elseif (!user_access('authorize external services')) {
      drupal_set_message('error', t('You are not authorized to allow external services access to this system.'));
      return drupal_access_denied();
    }
    return drupal_get_form('_services_token_auth_form');
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
  $form['oauth_callback'] = array(
    '#type'   => 'hidden',
    '#value'  => $_GET['oauth_callback']
  );
  $form['oauth_token'] = array(
    '#type'   => 'hidden',
    '#value'  => $_GET['oauth_token']
  );
  $form['oauth_consumer_key'] = array(
    '#type'   => 'hidden',
    '#value'  => $_GET['oauth_consumer_key'],
  );
  $form['oauth_nonce'] = array(
    '#type'   => 'hidden',
    '#value'  => $_GET['oauth_nonce'],
  );
  $form['oauth_nonce_timestamp'] = array(
    '#type'   => 'hidden',
    '#value'  => $_GET['oauth_timestamp'],
  );
  if (variable_get('oauth_use_with_services', 0)) {
    $form['services'] = array(
      '#description'  => t('Select which services you will allow %consumer use in your behalf',
        array('%consumer' => $_GET['oauth_callback'])
      ),
      '#title'        => t('Services'),
      '#tree'         => TRUE,
      '#type'         => 'fieldset',
    );

  // Display all services avaialbe
  module_load_include('inc', 'services');
  foreach (services_service_get_all() as $service) {
    $form['services'][$service['#method']] = array(
      '#title'          => $service['#method'],
      '#type'           => 'checkbox',
      '#default_value'  => 0,
    );
    }
  }
  $form['confirm'] = array(
    '#type'   => 'submit',
    '#value'  => t('Grant access'),
  );
  return $form;
}

/**
 * Asks users for granting proper access/deny permissions for different services
 * Authorizes an existing oauth request token and redirects to sender.
 *
 * @ingroup form
 */
function _services_token_auth_form_submit(&$form, &$form_state) {
  global $user;

  module_load_include('inc', 'services');
  foreach (services_service_get_all() as $service) {
    $method_name = $service['#method'];
    if (!empty($form_state['values']['services'][$method_name])) {
      $services[$method_name] = TRUE;
    }
  }

  // Save into the DB
  $sql = array(
    'consumer_key'  => $form_state['values']['oauth_consumer_key'],
    'services'      => $services,
    'timestamp'     => $form_state['values']['oauth_nonce_timestamp'],
    'session_id'    => $user->sid,
  );
  drupal_write_record('services_services', $sql);

  // Start the OAuth server
  $server = _services_oauth_server_init();

  $q = $_GET['q'];
  unset($_GET['q']);
  try {
    $req = OAuthRequest::from_request();
    oauth_authorize_request_token($form_state['values']['oauth_token']);
    drupal_goto($req->get_parameter('oauth_callback'));
  }
  catch (OAuthException $e) {
    print($e->getMessage() . "\n<hr />\n");
    print_r($req);
    die();
  }

  // Set the $_GET['q'] back to it's original value
  $_GET['q'] = $q;

  $form_state['redirect'] = $form_state['values']['oauth_callback'];
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
