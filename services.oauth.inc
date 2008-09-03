<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  OAuth token integration.
 */

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
  $server = _oauth_init_server();

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
    print($e->getMessage() . "\n<hr />\n");
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
      return drupal_get_form('oauth_grant_access');
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
 * Generate a request token from the request
 */
function _services_token_request() {
  if (variable_get('oauth_use_coolauth', 0)) {
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
      $server = _oauth_init_server();
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
    $server = _oauth_init_server();
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
