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
  $service = $_POST['service_name'];
  $arg = $_POST['args'];
  module_load_include('inc', 'services');
  $result = services_method_call($service, $arg);
  drupal_json(array('status' => TRUE, 'data' => $result));
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

/*
 * Callback for admin page
 */
function services_admin_settings() {
  $node_types = node_get_types('names');
  $defaults = isset($node_types['blog']) ? array('blog' => 1) : array();
  $form['security'] = array(
    '#title'        => t('Security'),
    '#type'         => 'fieldset',
    '#description'  => t('Changing security settings will require you to adjust all method calls. This will affect all applications using site services.'),
  );
  $form['security']['services_use_key'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Use keys'),
    '#default_value'  => variable_get('services_use_key', TRUE),
    '#description'    => t('When enabled all method calls need to provide a validation token to autheciate themselves with the server.'),
  );
  $form['security']['services_key_expiry'] = array(
    '#type'           => 'textfield',
    '#prefix'         => "<div id='services-key-expiry'>",
    '#suffix'         => "</div>",
    '#title'          => t('Token expiry time'),
    '#default_value'  => variable_get('services_key_expiry', 30),
    '#description'    => t('The time frame for which the token will be valid. Default is 30 secs'),
  );
  $form['security']['services_use_sessid'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Use sessid'),
    '#default_value'  => variable_get('services_use_sessid', TRUE),
    '#description'    => t('When enabled, all method calls must include a valid sessid. Only disable this setting if the application will user browser-based cookies.')
  );

  services_admin_js($form);

  return system_settings_form($form);
}

/**
 * UI enhancement for services page
 */
function services_admin_js($form) {
  $out = <<<EOJS
  $(document).ready(function() {
    $("#services-key-expiry")[$("#edit-services-use-key").attr('checked') ? 'show' : 'hide']();
    $("#edit-services-use-key").click(function() {
      $("#services-key-expiry")[$(this).attr('checked') ? 'show' : 'hide']();
   });
  });
EOJS;
  drupal_add_js($out, 'inline', 'footer');
}
