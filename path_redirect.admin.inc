<?php
//$Id$

/**
 * Menu callback for edit pages
 */
function path_redirect_admin_edit($rid = 0) {
  if ($rid) {
    $redirect = path_redirect_load($pid);
    drupal_set_title(check_plain($redirect['path']));
    $output = drupal_get_form('path_redirect_admin_form', $alias);
  }
  else {
    $output = drupal_get_form('path_redirect_admin_form');
  }

  return $output;
}

/**
 * Callback for administration pages
 *
 * @param $rid
 *  redirect id
 * @param $op
 *  operation: edit, delete
 * @return
 *  themed output for page
 */
function path_redirect_admin($rid = FALSE, $op = FALSE) {
  if ($rid) {
    $breadcrumbs = array(l(t('Home'), '/'), l(t('Administer'), 'admin'), l(t('URL redirects'), 'admin/build/path_redirect'));
    $rid = db_escape_string($rid);
    $redirect = path_redirect_load($rid);
    if ($redirect && $op != 'delete') {
      drupal_set_breadcrumb($breadcrumbs);
      return drupal_get_form('path_redirect_admin_form', $redirect);
    }
    elseif ($redirect && $op == 'delete') {
      return drupal_get_form('path_redirect_delete_confirm', $redirect);
    }
    else {
      drupal_not_found();
    }
  }

  $header = array(
    array('data' => t('From'), 'field' => 'path', 'sort' => 'asc'),
    array('data' => t('To'), 'field' => 'redirect'),
    array('data' => t('Type'), 'field' => 'type'),
    array('data' => t('Operations'), 'colspan' => '3')
  );

  $result = pager_query('SELECT rid, path, redirect, query, fragment, type FROM {path_redirect}'. tablesort_sql($header), 50);
  $count = 0;
  $types = path_redirect_error_list();
  $destination = drupal_get_destination();
  while ($r = db_fetch_object($result)) {
    $path = $r->path;
    $redirect = $r->redirect;
    $query = $r->query ? "?$r->query" : '';
    $fragment = $r->fragment ? "#$r->fragment" : '';
    $rows[] = array(
      $path,
      check_url($redirect . $query . $fragment),
      $types[$r->type]['title'],
      array('data' => l(t('test'), $r->path, array())),
      array('data' => l(t('edit'), 'admin/build/path_redirect/edit/'. $r->rid, array('query' => $destination))),
      array('data' => l(t('delete'), 'admin/build/path_redirect/delete/'. $r->rid, array('query' => $destination))),
    );
    $count++;
  }

  if (!$count) {
    $rows[] = array(array('data' => t('No redirects have been defined.'), 'colspan' => '6'));
  }

  $output = theme('table', $header, $rows);
  $output .= '<p>'. l(t('Add new redirect'), 'admin/build/path_redirect/add') .'</p>';
  $output .= theme('pager');

  return $output;
}

function path_redirect_admin_form(&$form_state, $edit = array('path' => '', 'redirect' => '', 'query' => '', 'fragment' => '', 'type' => '301', 'rid' => NULL)) {
  $default_type = 301;

  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('From'),
    '#description' => t('Enter a Drupal path or path alias to redirect. Query strings <em>?foo=bar</em> and fragment anchors <em>#baz</em> are <strong>not</strong> allowed.'),
    '#maxlength' => 255,
    '#default_value' => $edit['path'],
  );

  $form['redirect'] = array(
    '#type' => 'item',
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
    '#title' => t('To'),
    '#description' => '<div style="display:block">'. t('Enter a Drupal path, path alias, or external URL to redirect to. Use %front to redirect to the front page.  Enter (optional) queries after "?" and (optional) anchor after "#". Most redirects will not contain queries or fragment anchors.', array('%front' => '<front>')) .'</div>',
  );

  $form['redirect']['redirect'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 255,
    '#default_value' => $edit['redirect'],
  );

  $form['redirect'][] = array(
    '#value' => '?',
  );

  $form['redirect']['query'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#maxlength' => 50,
    '#default_value' => $edit['query'],
  );

  $form['redirect'][] = array(
    '#value' => '#',
  );

  $form['redirect']['fragment'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#maxlength' => 50,
    '#default_value' => $edit['fragment'],
  );

  $form[] = array(
    '#value' => "<p> </p>", // little bit of extra space
  );

  $form['type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Redirect Type'),
    '#collapsible' => true,
    '#collapsed' => ($edit['type'] == $default_type),
  );

  foreach (path_redirect_error_list() as $key => $info) {
    $form['type'][]['type'] = array(
      '#type' => 'radio',
      '#title' => $info['title'],
      '#description' => $info['description'],
      '#return_value' => $key,
      '#default_value' => $edit['type'],
    );
  }
  $form['type']['link'] = array(
    '#type' => 'markup',
    '#value' => t('<p>Find more information about http redirect codes <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3">here</a>.</p>'),
  );

  $form['rid'] = array(
    '#type' => 'hidden',
    '#value' => $edit['rid'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $edit['rid'] ? t('Update redirect') : t('Create new redirect'),
  );

  return $form;
}

function path_redirect_admin_form_validate($form, &$form_state) {
  if (trim($form_state['values']['path']) == '') {
    form_set_error('path', t('You must enter a <strong>from</strong> path.'));
  }
  else {
    $path_error = '';

    // The "from" path should not conflict with another redirect
    $result = path_redirect_load(NULL, preg_replace('/[?#].*/', '', $form_state['values']['path']));
    if ($result && (!$form_state['values']['rid'] || ($form_state['values']['rid'] !== $result['rid']))) {
      $path_error .= ' '. t('The <strong>from</strong> path you entered is already redirected. You can <a href="@edit-page">edit this redirect</a> instead.', array('@edit-page' => url('admin/build/path_redirect/edit/'. $result['rid'])));
    }
    // Check that the "from" path is valid and contains no # or ?
    if (strstr($form_state['values']['path'], '#')) {
      $path_error .= ' '. t('You cannot redirect from a fragment anchor.');
    }
    // No query string allowed in "from" (but see #174961)
    if (strstr($form_state['values']['path'], '?')) {
      $path_error .= ' '. t('You cannot currently include a query in your redirect <strong>from</strong> path.');
    }
    // Make sure "from" has the form of a local Drupal path
    if (!valid_url($form_state['values']['path'])) {
      $path_error .= ' '. t('The redirect <strong>from</strong> path does not appear valid. This must be a local Drupal path.');
    }

    if (!empty($path_error)) {
      form_set_error('path', $path_error);
    }
  }

  if (!valid_url($form_state['values']['redirect']) && !valid_url($form_state['values']['redirect'], TRUE) && $form_state['values']['redirect'] != '<front>') {
    form_set_error('redirect', t('The redirect <strong>to</strong> path does not appear valid.'));
  }

  if ($form_state['values']['redirect'] == '<front>') {
    $form_state['values']['redirect'] = variable_get('site_frontpage', 'node');
  }

  // check that there there are no redirect loops
  if ($form_state['values']['path'] === $form_state['values']['redirect']) {
    form_set_error('redirect', t('You are attempting to redirect the page to itself. This will result in an infinite loop.'));
  }

  // Allow spaces in "from" path
  $form_state['values']['path'] = str_replace("+", " ", $form_state['values']['path']);
}

function path_redirect_admin_form_submit($form, &$form_state) {
  path_redirect_save($form_state['values']);
  drupal_set_message(t('Redirect has been saved.'));
  drupal_goto('admin/build/path_redirect');
}

function path_redirect_admin_delete_confirm($form_state, $rid) {
  if (user_access('administer redirects')) {
    $form['rid'] = array('#type' => 'value', '#value' => $rid);
    $redirect = path_redirect_load($rid);
    $output = confirm_form($form,
      t('Are you sure you want to delete the redirect from %path to %redirect?', array('%path' => $redirect['path'], '%redirect' => $redirect['redirect'])),
      isset($_GET['destination']) ? $_GET['destination'] : 'admin/build/path_redirect');
  }
  return $output;
}

function path_redirect_admin_delete_confirm_submit($form, $form_state) {
  if ($form_state['values']['confirm']) {
    path_redirect_delete(NULL, NULL, $form_state['values']['rid']);
    drupal_set_message(t('The redirect has been deleted.'));
  }
}

/**
 * Return an array of 300-range error codes
 * placed here for clarity
 */
function path_redirect_error_list() {
  $errors = array(
    300 => array('title' => t('300 Multiple Choices'), 'description' => t('The request is ambiguous and needs clarification as to which resource was requested.')),
    301 => array('title' => t('301 Moved Permanently'), 'description' => t('Moved Permanently. The resource has permanently moved elsewhere, the response indicates where it has gone to. <strong>Recommended.</strong>')),
    302 => array('title' => t('302 Found'), 'description' => t('The resource has temporarily moved elsewhere, the response indicates where it is at present. <strong>This is Drupal\'s default redirect type.</strong>')),
    303 => array('title' => t('303 See Other'), 'description' => t('See Other/Redirect. A preferred alternative source should be used at present.')),
    304 => array('title' => t('304 Not Modified'), 'description' => t('The server has identified from the request information that the client\'s copy of the information is up-to-date and the requested information does not need to be sent again.')),
    305 => array('title' => t('305 Use Proxy'), 'description' => t('The request must be sent through the indicated proxy server.')),
    307 => array('title' => t('307 Temporary Redirect'), 'description' => t('The resource has temporarily moved elsewhere, the response indicates where it is at present. Client should still use this URL.')),
  );

  return $errors;
}
