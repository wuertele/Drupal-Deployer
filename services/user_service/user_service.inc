<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  Link general user functionalities to services module.
 */

/**
 * Delete an user.
 *
 * @param $uid
 *   Number. The user ID.
 */
function user_service_delete($uid) {
  // Everything went right.
  return TRUE;
}

/**
 * Get user details.
 *
 * @param $uid
 *   Number. The user ID.
 */
function user_service_get($uid) {
  // Everything went right.
  return TRUE;
}

/**
 * Login a user
 *
 * @param $username
 *   String. The username.
 * @param $password
 *   String. The user password.
 */
function user_service_login($username, $password) {
  global $user;

  if ($user->uid) {
    // user is already logged in
    return services_error(t('Already logged in as !user.', array('!user' => $user->name)));
  }

  $user = user_authenticate(array('name' => $username, 'pass' => $password));

  if ($user->uid) {
    session_start();
    $return = new stdClass();
    $return->sessid = session_id();
    $return->user = $user;
    return $return;
  }
  session_destroy();
  return services_error(t('Wrong username or password.'));
}

/**
 * Logout user
 */
function user_service_logout() {
  global $user;

  if (!$user->uid) {
    // User is not logged in
    return services_error(t('User is not logged in.'));
  }

  watchdog('user', 'Session closed for %name.', array('%name' => theme('placeholder', $user->name)));

  // Destroy the current session:
  session_destroy();
  module_invoke_all('user', 'logout', NULL, $user);

  // Load the anonymous user
  $user = drupal_anonymous_user();

  return TRUE;
}

/**
 * Save user details.
 *
 * @param $user_data
 *   Object. The user object with all user data.
 */
function user_service_save($user_data) {
  // Everything went right.
  return TRUE;
}
