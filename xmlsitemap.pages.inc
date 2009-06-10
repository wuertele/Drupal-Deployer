<?php
// $Id$

/**
 * @file
 * Page callbacks for the xmlsitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * Output a sitemap page.
 *
 * @see xmlsitemap_file_transfer()
 */
function xmlsitemap_output_chunk($chunk = 0) {
  global $language;

  $file = xmlsitemap_get_chunk_file($chunk, $language);

  if (!file_exists($file) || (isset($_GET['refresh']) && user_access('administer xmlsitemap'))) {
    module_load_include('inc', 'xmlsitemap');
    if (!xmlsitemap_generate($chunk, $language)) {
      return drupal_not_found();
    }
  }

  $last_modified = gmdate('D, d M Y H:i:s', filemtime($file)) . ' GMT';
  $etag = '"' . md5($last_modified) . '"';

  // See if the client has provided the required HTTP headers.
  $if_modified_since = isset($_SERVER['HTTP_IF_MODIFIED_SINCE']) ? stripslashes($_SERVER['HTTP_IF_MODIFIED_SINCE']) : FALSE;
  $if_none_match = isset($_SERVER['HTTP_IF_NONE_MATCH']) ? stripslashes($_SERVER['HTTP_IF_NONE_MATCH']) : FALSE;
  if ($if_modified_since && $if_none_match && $if_none_match == $etag && $if_modified_since == $last_modified) {
    header('HTTP/1.1 304 Not Modified');
    // All 304 responses must send an etag if the 200 response for the same object contained an etag
    header('Etag: ' . $etag);
    exit;
  }

  $headers = array(
    'Content-type: text/xml; charset=utf-8',
    //'Content-length: ' . filesize($file),
    'Last-modified: ' . $last_modified,
    'Etag: ' . $etag,
    'Expires: Sun, 20 May 1984 00:00:00 GMT',
    'Cache-Control: must-revalidate',
  );

  // Transfer the file as output.
  xmlsitemap_file_transfer($file, $headers);
}

/**
 * Modified version of file_transfer() that invokes hook_exit()s afterwards.
 *
 * @see file_transfer()
 */
function xmlsitemap_file_transfer($source, $headers) {
  if (ob_get_level()) {
    ob_end_clean();
  }

  foreach ($headers as $header) {
    // To prevent HTTP header injection, we delete new lines that are
    // not followed by a space or a tab.
    // See http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.2
    //$header = preg_replace('/\r?\n(?!\t| )/', '', $header);
    drupal_set_header($header);
  }

  //$source = file_create_path($source);

  // Transfer file in 1024 byte chunks to save memory usage.
  if ($handle = fopen($source, 'rb')) {
    while (!feof($handle)) {
      print fread($handle, 1024);
    }
    fclose($handle);
  }
  else {
    drupal_not_found();
  }

  module_invoke_all('exit');
  exit();
}

/**
 * Output an XML transformation file for the sitemap XML.
 */
function xmlsitemap_output_xsl() {
  module_load_include('inc', 'xmlsitemap');
  drupal_set_header('Content-type: application/xml; charset=utf-8');
  $module_path = drupal_get_path('module', 'xmlsitemap');
  $xsl_css = base_path() . $module_path . '/xmlsitemap-xml.css';
  $xsl_js = base_path() . $module_path . '/xmlsitemap-xml.js';

  $contents = file_get_contents($module_path . '/xmlsitemap.xsl');
  $contents = str_replace(array('[xsl-css]', '[xsl-js]', '[version]'), array($xsl_css, $xsl_js, _xmlsitemap_get_version()), $contents);
  echo $contents;
}
