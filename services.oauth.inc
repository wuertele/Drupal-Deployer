<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  OAuth token integration.
 */

/**
 * Generate a request token from the request
 *
 * @param $application_key
 *    String. Key of registered cool-auth application this could be
 *    found on /user/%user/coolauth page OR will be supplied by
 *    application server
 * @param $application_secret
 *    String. Unique per application_key and is obtained from oauth_cool_auth
 *    table which stores information for different registered cool auth
 *    applications
 * @param $required_hash
 *    String. this is a way to identify a specific server which is making
 *    OAuth calls to our server - md5($application_key.$oauth_nonce) is
 *    used as application_sig to identify aplication
 * @param $token
 *    String. It is in form oauth_token=token&oauth_token_secret=secret
 *    is passed over server - this is what you are looking for in all
 *    this call :)
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
