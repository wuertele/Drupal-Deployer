<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 * All functions that are not hooks or constantly accessed.
 */

/**
 * Callback for crossdomain.xml
 */
function _services_crossdomain_xml() {
  global $base_url;
  $output = '<!DOCTYPE cross-domain-policy SYSTEM "http://www.macromedia.com/xml/dtds/cross-domain-policy.dtd">'."\n";
  $output .= '<cross-domain-policy>'."\n";
  $output .= '  <allow-access-from domain="'. check_plain($_SERVER['HTTP_HOST']) .'" />'."\n";
  $output .= '  <allow-access-from domain="*.'. check_plain($_SERVER['HTTP_HOST']) .'" />'."\n";
  $keys = services_get_keys();

  foreach ($keys as $key) {
    if (!empty($key->domain)) {
      $output .= '  <allow-access-from domain="'. check_plain($key->domain) .'" />'."\n";
      $output .= '  <allow-access-from domain="*.'. check_plain($key->domain) .'" />'."\n";
    }
  }

  $output .= '</cross-domain-policy>';

  services_xml_output($output);
}

/**
 * Prepare an error message for returning to the XMLRPC caller.
 *
 * @param $message
 *   String. The error message.
 * @return
 *   String. Depends on the services server.
 */
function services_error($message) {
  $server_info = services_get_server_info();

  // Look for custom error handling function.
  // Should be defined in each server module.
  if ($server_info && module_hook($server_info->module, 'server_error')) {
    return module_invoke($server_info->module, 'server_error', $message);
  }

  // No custom error handling function found.
  return $message;
}

/**
 * Callback for server endpoint
 */
function _services_server($server_path = NULL) {
  // Find which module the server is part of
  foreach (module_implements('server_info') as $module) {
    $info = module_invoke($module, 'server_info');
    if ($info['#path'] == $server_path) {

      // call the server
      services_set_server_info($module);
      print module_invoke($module, 'server');

      // Do not let this output
      module_invoke_all('exit');
      exit;
    }
  }
  // return 404 if the service doesn't exist
  drupal_not_found();
}
