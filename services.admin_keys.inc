<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  Manage security keys.
 */

/**
 * List all registered application keys.
 */
function services_admin_keys_list() {
  $header = array(
    array('data' => t('User'),    'field' => 'uid', 'sort' => 'asc'),
    array('data' => t('Title'),   'field' => 'application_title'),
    array('data' => t('Domain'),  'field' => 'application_url'),
    array('data' => t('Type'),    'field' => 'application_type'),
    array('data' => t('Operations'), 'colspan' => '2')
  );
  $result = pager_query('SELECT * FROM {oauth_cool_auth}'. tablesort_sql($header), 10);
  while ($key = db_fetch_array($result)) {
    $keys[$key['uid']] = $key;
  }
  $rows = array();

  foreach ($keys as $kid => $key) {
    if ($key['application_type'] == 0) {
      $option = t('Web');
    }
    elseif ($key['application_type'] == 1) {
      $option = t('Desktop');
    }
    else {
      $option = t('Mobile');
    }

    $row = array();
    $row[] = $kid;
    $row[] = $key['application_title'];
    $row[] = $key['application_url'];
    $row[] = $option;
    $row[] = array('data' => l(t('edit'), 'admin/build/services/keys/edit'. $key->application_key));
    $row[] = array('data' => l(t('delete'), 'admin/build/services/registered/delete/'. $key['application_key']));
    $rows[] = $row;
  }

  if (empty($rows)) {
    return t('No website registered to use cool auth on server.');
  }

  return theme('table', $header, $rows);
}

/**
 * Create/edit a application keys.
 *
 * @ingroup form
 */
function _services_admin_keys_form($form_state, $user) {
  $default = db_fetch_array(db_query('SELECT * from {oauth_cool_auth}
    WHERE uid = %d', $form['uid']['#value']));
  $form['new'] = array(
    '#value'          => !(bool)$default,
    '#type'           => 'value',
  );

  if (user_access('create cool auth for other users')) {
    $form['uid'] = array(
      '#default_value'  => (empty($default['uid']) ? $user->uid : $default['uid']),
      '#description'    => t('User ID corresponding to whome you want to register an cool auth API key'),
      '#required'       => TRUE,
      '#title'          => t('User'),
      '#type'           => 'textfield',
    );
  }
  else {
    $form['uid'] = array(
      '#value'          => $user->uid,
      '#type'           => 'value',
    );
  }
  $form['application_title'] = array(
    '#default_value'  => $default['application_title'],
    '#description'    => t('The name of the application.'),
    '#required'       => TRUE,
    '#title'          => t('Application'),
    '#type'           => 'textfield',
  );
  $form['application_url'] = array(
    '#default_value'  => $default['application_url'],
    '#description'    => t('URL of your application which is making access to server.'),
    '#required'       => TRUE,
    '#title'          => t('Application URL'),
    '#type'           => 'textfield',
  );
  $form['application_type'] = array(
    '#default_value'  => (empty($default) ? 0 : $default['application_type']),
    '#options'        => array(
      t('Web Application'),
      t('Desktop Application'),
      t('Mobile Application')
    ),
    '#required'       => TRUE,
    '#title'          => t('Type of Application'),
    '#type'           => 'radios',
  );
  $form['application_description'] = array(
    '#default_value'  => $default['application_description'],
    '#title'          => t('Description'),
    '#type'           => 'textarea',
  );
  $form['application_callback_url'] = array(
    '#default_value'  => $default['application_callback_url'],
    '#description'    => t('Address of callback URL of your service'),
    '#required'       => TRUE,
    '#title'          => t('Callback URL'),
    '#type'           => 'textfield',
  );
  $form['application_notes'] = array(
    '#default_value'  => $default['application_notes'],
    '#description'    => t('Any extra note or comment about this application.'),
    '#title'          => t('Notes'),
    '#type'           => 'textarea',
  );
  $form['submit'] = array(
    '#value'          => t('Save application'),
    '#type'           => 'submit',
  );
  return $form;
}

/**
 * Create/edit a application keys.
 *
 * @ingroup form
 */
function _services_admin_keys_form_validate(&$form, &$form_state) {
  if (!is_numeric($form_state['values']['uid'])) {
    form_set_error('form', t('Please fill a valid user ID'));
  }
  elseif (!user_load($form_state['values']['uid'])) {
    form_set_error('uid', t('User does not exist'));
  }

  // Check the consumer URLs
  if (!valid_url($form_state['values']['application_url'], TRUE)) {
    form_set_error('application_url', t('Please fill a valid url for application'));
  }
  if (!valid_url($form_state['values']['application_callback_url'], TRUE)) {
    form_set_error('application_callback_url', t('Please fill a valid url for application'));
  }
}

/**
 * Create/edit a application keys.
 *
 * @ingroup form
 */
function _services_admin_keys_form_submit(&$form, &$form_state) {
  if (empty($form_state['values']['new'])) {
    drupal_write_record('oauth_cool_auth', $form_state['values'], 'uid');
  }
  else {
    $form_state['values']['application_key']    = md5(uniqid(mt_rand(), TRUE));
    $form_state['values']['application_secret'] = md5(uniqid(mt_rand(), TRUE));
    drupal_write_record('oauth_cool_auth', $form_state['values']);
  }

  return 'admin/build/server/applications';
}
