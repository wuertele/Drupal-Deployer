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
function _services_admin_service_list() {
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
  $result = _services_service_call($service, $args);
  drupal_json(array('status' => TRUE, 'data' => print_r($result, TRUE)));
  exit();
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
