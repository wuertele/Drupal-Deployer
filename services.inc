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
function services_crossdomain_xml() {
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
