<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  Browser thru all services and servers.
 */

/**
 * List all services available.
 */
function services_admin_browse_index() {
  module_load_include('inc', 'services');
  $methods = services_service_get_all();

  if (empty($methods)) {
    return t('No services have been enabled.');
  }
  else {
    // Group namespaces
    $services = array();
    foreach ($methods as $method) {
      $namespace = drupal_substr($method['#method'], 0, strrpos($method['#method'], '.'));
      $services[$namespace][$method['#method']] = $method;
    }
    ksort($services);

    foreach ($services as $namespace => $methods) {
      ksort($methods);
      $output .= '<h2>'. $namespace .'</h2>';
      $output .= '<ul>';
      foreach ($methods as $method) {
        $output .= '<li class="leaf">'. l($method['#method'], 'admin/build/services/services/'. $method['#method']) .'</li>';
      }
      $output .= '</ul>';
    }

    return $output;
  }
}

/**
 * Allow users to test a given service.
 *
 * @param $method
 *   Object. The service info.
 */
function services_admin_browse_method($method) {
  global $_services_admin_browse_test_submit_result;

  $output = '<h3>'. $method['#method'] .'</h3>'.
    '<p>'. $method['#description'] .'</p>';

  // List arguments.
  $output .= '<h3>'. t('Arguments') .' ('. count($method['#args']) .')</h3>';
  $output .= '<dl id="service-browser-arguments">';
  $count = 0;
  foreach ($method['#args'] as $arg) {
    $count++;
    $output .= '<dt><em class="type">'. $arg['#type'] .'</em><strong class="name">'.
      $arg['#name'] .'</strong> ('. (($arg['#optional']) ? t('optional') : t('required')) .')</dt>';
    $output .= '<dd>'. $arg['#description'] .'</dd>';
  }

  $output .= '</dl>';

  // Allow testing of methods
  $output .= '<h3>'. t('Call method') .'</h3>'.
    drupal_get_form('services_admin_browse_test');

  // Display results
  if ($_services_admin_browse_test_submit_result) {
    $output .= '<div id="output">';
    $output .= '<h3>'. t('Result') .'</h3>';
    $output .= '<code>'. $_services_admin_browse_test_submit_result .'</code>';
    $output .= '</div>';
  }

  return $output;
}

/**
 * Allow users to test a given service.
 *
 * @param $method
 *   Object. The service info.
 *
 * @ingroup form
 */
function _services_admin_service_test($form_state, $service) {
  // Testing fields
  $form['arg'] = array(
    '#tree'   => TRUE,
  );
  foreach ($service['#args'] as $key => $arg) {
    $form['arg'][$key] = array(
      '#description'  => $arg['#description'],
      '#required'     => !$arg['#optional'],
      '#title'        => $arg['#name'],
      '#type'         => 'textfield',
    );
    if ($arg['#size'] == 'big') {
      $form['arg'][$key]['#type'] = 'textarea';
    }
  }

  $form['service'] = array(
    '#value'          => $service,
    '#type'           => 'value',
  );
  $form['service_name'] = array(
    '#value'          => $service['#method'],
    '#type'           => 'hidden',
  );

  $form['submit'] = array(
    '#ahah' => array(
      'path'      => 'admin/build/services/js',
      'wrapper'   => 'services-test-result',
      'progress'  => array('type' => 'bar', 'message' => t('Please wait...')),
    ),
    '#suffix'         => '<div id="services-test-result"></div>',
    '#type'           => 'submit',
    '#value'          => t('Call method')
  );
  return $form;
}

/**
 * AJAX callback to return a service test.
 *
 * @return
 *   Prints the services result in JSON format.
 */
function _services_admin_service_test_js() {
  $service  = $_POST['service_name'];
  $args     = $_POST['arg'];
  module_load_include('inc', 'services');
  $result = services_method_call($service, $args);
  drupal_json(array('status' => TRUE, 'data' => print_r($result, TRUE)));
  exit();
}

/**
 * Theme service testing form
 *
 * @param $form
 *   Array. The service testing form.
 *
 * @ingroup themable
 */
function theme__services_admin_service_test($form) {
  global $_services_admin_browse_test_submit_result;
  $service = $form['service']['#value'];

  // Testing fields
  $output .= drupal_render($form);

  // Testing results
  if ($_services_admin_browse_test_submit_result) {
    $output .= '<div id="output">';
    $output .= '<h3>'. t('Result') .'</h3>';
    $output .= '<code>'. $_services_admin_browse_test_submit_result .'</code>';
    $output .= '</div>';
  }

  return $output;
}

/**
 * Configure the Services module
 */
function services_admin_settings() {
  $form['security'] = array(
    '#title'        => t('Security'),
    '#type'         => 'fieldset',
    '#description'  => t('Changing security settings will require you to adjust all method calls. This will affect all applications using site services.'),
  );
  $form['security']['services_use_oauth'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Use OAuth'),
    '#default_value'  => variable_get('services_use_oauth', TRUE),
    '#description'    => t('When enabled, all method calls must include a valid OAuth access token and consumer token.')
  );
  return system_settings_form($form);
}
