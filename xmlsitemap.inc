<?php
// $Id$

/**
 * @file
 * Miscellaneous functions for the xmlsitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * Delete and regenerate the sitemap files.
 */
function xmlsitemap_regenerate() {
  _xmlsitemap_regenerate_before();

  // Generate the sitemap pages.
  $languages = language_list();
  $chunk_count = xmlsitemap_get_chunk_count(TRUE);
  if ($chunk_count > 1) {
    // If we have more than one chunk, we need to increment this value by one
    // since the index page (chunk 0) will also need to be generated.
    $chunk_count++;
  }
  foreach (xmlsitemap_var('languages') as $language) {
    for ($i = 0; $i < $chunk_count; $i++) {
      xmlsitemap_generate($i, $languages[$language]);
    }
  }

  _xmlsitemap_regenerate_after();
}

/**
 * Perform operations before rebuilding the sitemap.
 */
function _xmlsitemap_regenerate_before() {
  // Increase the processing time since this might take a little bit.
  @set_time_limit(240);

  // Clear all cached sitemap files.
  xmlsitemap_clear_cache();

  // Save custom data into a temporary table so it can be re-loaded later.
  //db_query_temporary("SELECT type, id, priority, changefreq, changecount FROM {xmlsitemap}", 'xmlsitemap_temp');
}

/**
 * Perform operations after rebuilding the sitemap.
 */
function _xmlsitemap_regenerate_after() {
  // Show a watchdog message that the sitemap was regenerated.
  watchdog('xmlsitemap', 'Sitemap regenerated.', array(), WATCHDOG_NOTICE, l(t('View'), 'sitemap.xml'));

  // Unset the regenerate flag.
  variable_set('xmlsitemap_regenerate_needed', FALSE);

  // If the chunk count has changed, we will need to rebuild the menu.
  variable_set('menu_rebuild_needed', TRUE);

  variable_set('xmlsitemap_generated_last', REQUEST_TIME);
}

/**
 * Fetch the data from {xmlsitemap}, generates the sitemap, then caches it.
 *
 * @param $chunk
 *   An integer representing the integer of the sitemap page chunk.
 * @param $language
 *   A language object, defaults to the default language.
 * @return
 *   TRUE on success; otherwise FALSE
 *
 * @todo Revise/simplify or remove the function.
 */
function xmlsitemap_generate($chunk = 0, $language = NULL) {
  if (!is_numeric($chunk) || $chunk > xmlsitemap_get_chunk_count()) {
    trigger_error("Inproper conditiion hit in xmlsitemap_generate(). Chunk: $chunk, Chunk Count: " . xmlsitemap_get_chunk_count());
    return FALSE;
  }

  if (!isset($language)) {
    $language = language_default();
  }

  $file = xmlsitemap_get_chunk_file($chunk, $language);

  if (!$handle = fopen($file, 'wb')) {
    watchdog('xmlsitemap', 'Could not open file @file for writing.', array('@file' => $file), WATCHDOG_ERROR);
    return FALSE;
  }

  $status = TRUE;
  if (xmlsitemap_get_chunk_count() > 1 && !$chunk) {
    xmlsitemap_generate_index($handle, $status, $language);
  }
  else {
    xmlsitemap_generate_chunk($handle, $status, $chunk, $language);
  }
  fclose($handle);

  if (!$status) {
    watchdog('xmlsitemap', 'Unknown error occurred while writing to file @file.', array('@file' => $file), WATCHDOG_ERROR);
  }
  elseif (xmlsitemap_var('gz')) {
    $file_gz = xmlsitemap_get_chunk_file($chunk, $language, TRUE);
    file_put_contents($file_gz, gzencode(file_get_contents($file), 9));
  }

  return $status;
}

//function xmlsitemap_fwrite($handle, &$status, $string) {
//  $status &= (bool) fwrite($handle, $string);
//}

/**
 * Write the proper XML sitemap header.
 *
 * @param $handle
 *   A file system pointer resource that is typically created using fopen().
 * @param $status
 * @param $index
 */
function xmlsitemap_generate_chunk_header($handle, &$status, $index = FALSE) {
  $status &= (bool) fwrite($handle, '<?xml version="1.0" encoding="UTF-8"?>' . "\n");
  if (xmlsitemap_var('xsl')) {
    $status &= (bool) fwrite($handle, '<?xml-stylesheet type="text/xsl" href="'. url('sitemap.xsl') .'"?>' . "\n");
  }
  $status &= (bool) fwrite($handle, '<' . ($index ? 'sitemapindex' : 'urlset') . ' xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' . "\n");
  //$status &= (bool) fwrite($handle, '<' . ($index ? 'sitemapindex' : 'urlset') . ' xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"' . "\n");
  //$status &= (bool) fwrite($handle, '  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"' . "\n");
  //$status &= (bool) fwrite($handle, '  xsi:schemaLocation="http://www.sitemaps.org/schemas/sitemap/0.9' . "\n");
  //$status &= (bool) fwrite($handle, '  http://www.sitemaps.org/schemas/sitemap/0.9/' . ($index ? 'siteindex.xsd' : 'sitemap.xsd') . '">' . "\n");
  return $status;
}

/**
 * Generate one page (chunk) of the sitemap.
 *
 * @param $handle
 *   A file system pointer resource that is typically created using fopen().
 * @param $status
 * @param $chunk
 *   An integer representing the integer of the sitemap page chunk.
 * @param $language
 *   A language object, defaults to the default language.
 */
function xmlsitemap_generate_chunk($handle, &$status, $chunk, $language = NULL) {
  $url_options = xmlsitemap_get_url_options(array('language' => $language));

  // @todo Optimize the query.
  // @todo When should "AND x.language IN ('%s', '')" be used? Drupal still lists English content when viewing site in French.
  //$sql = "SELECT x.loc, COALESCE(ua.dst, x.loc) AS alias, x.lastmod, x.changefreq, x.changecount, x.priority, ua.language, ua.pid FROM {xmlsitemap} x
  //        LEFT JOIN {url_alias} ua ON x.loc = ua.src AND ua.language IN ('%s', '')
  //        WHERE x.status = 1
  //        ORDER BY x.loc, ua.language DESC, ua.pid DESC";
  //$args = array($language->language);
  $sql = "SELECT x.loc, x.lastmod, x.changefreq, x.changecount, x.priority FROM {xmlsitemap} x
          WHERE x.status = 1";
  $args = array();
  $offset = max($chunk - 1, 0) * xmlsitemap_get_chunk_size();
  $limit = xmlsitemap_get_chunk_size();
  $query = db_query_range($sql, $args, $offset, $limit);

  xmlsitemap_generate_chunk_header($handle, $status);
  while ($link = db_fetch_array($query)) {
    $status &= (bool) fwrite($handle, '<url>');
    //$status &= fwrite($handle, '<loc>'. url($link['alias'], $url_options) .'</loc>');
    $status &= (bool) fwrite($handle, '<loc>'. url($link['loc'], $url_options) .'</loc>');
    if ($link['lastmod']) {
      $status &= (bool) fwrite($handle, '<lastmod>'. gmdate(DATE_W3C, $link['lastmod']) .'</lastmod>');
      // If the link has a lastmod value, update the changefreq so that links
      // with a short changefreq but updated two years ago show decay.
      //xmlsitemap_recalculate_changefreq($link);
      $link['changefreq'] = (REQUEST_TIME - $link['lastmod'] + $link['changefreq']) / 2;
    }
    if ($link['changefreq']) {
      $status &= (bool) fwrite($handle, '<changefreq>'. xmlsitemap_get_changefreq($link['changefreq']) .'</changefreq>');
    }
    if (isset($link['priority']) && $link['priority'] != 0.5) {
      // Don't output the priority value for links that have 0.5 priority. This
      // is the default 'assumed' value if priority is not included as per the
      // sitemaps.org specification.
      $status &= (bool) fwrite($handle, '<priority>'. number_format($link['priority'], 1) .'</priority>');
    }
    $status &= (bool) fwrite($handle, '</url>' . "\n");
  }
  $status &= (bool) fwrite($handle, '</urlset>');

  return $status;
}

/**
 * Generate the index sitemap.
 *
 * @param $handle
 *   A file system pointer resource that is typically created using fopen().
 * @param $status
 * @param $language
 *   A language object, defaults to the default language.
 */
function xmlsitemap_generate_index($handle, &$status, $language = NULL) {
  $url_options = xmlsitemap_get_url_options(array('language' => $language));
  $chunk_count = xmlsitemap_get_chunk_count(TRUE);

  xmlsitemap_generate_chunk_header($handle, $status, TRUE);
  for ($i = 1; $i <= $chunk_count; $i++) {
    $status &= (bool) fwrite($handle, '<sitemap>');
    $status &= (bool) fwrite($handle, '<loc>' . url('sitemap-' . $i . '.xml', $url_options) . '</loc>');
    // @todo Use the actual lastmod value of the chunk file.
    $status &= (bool) fwrite($handle, '<lastmod>' . gmdate(DATE_W3C, REQUEST_TIME) . '</lastmod>');
    $status &= (bool) fwrite($handle, '</sitemap>' . "\n");
  }
  $status &= (bool) fwrite($handle, '</sitemapindex>');

  return $status;
}

/**
 * Batch callback; clear sitemap links for a specific module.
 */
function xmlsitemap_rebuild_batch_clear($module, &$context) {
  module_invoke($module, 'xmlsitemap_links_clear');
  $context['message'] = t('Now clearing %module links.', array('%module' => $module));
}

/**
 * Batch callback; fetch and add the sitemap links for a specific module.
 */
function xmlsitemap_rebuild_batch_fetch($module, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['batch'] = module_hook($module, 'xmlsitemap_links_batch_info');
    if ($context['sandbox']['batch']) {
      $context['sandbox'] += module_invoke($module, 'xmlsitemap_links_batch_info');
    }
    else {
      $context['sandbox']['links'] = module_invoke($module, 'xmlsitemap_links');
      $context['sandbox']['max'] = count($context['sandbox']['links']);
    }
    $context['sandbox'] += array(
      'progress' => 0,
      'current' => 0,
    );
  }

  if ($context['sandbox']['batch']) {
    $links = module_invoke($module, 'xmlsitemap_links', $context['sandbox']['current'], xmlsitemap_var('batch_limit'));
  }
  else {
    $links = array_splice($context['sandbox']['links'], 0, xmlsitemap_var('batch_limit'));
  }

  foreach ($links as $link) {
    xmlsitemap_save_link($link);
    $context['sandbox']['progress']++;
    $context['sandbox']['current'] = $link['id'];
    $context['message'] = t('Now processing %module link @count.', array('%module' => $module, '@count' => $context['sandbox']['progress']));
  }

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch callback; generate the sitemap chunks for a language.
 */
function xmlsitemap_rebuild_batch_generate($language, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = xmlsitemap_get_chunk_count(TRUE);
    // If we have more than one chunk, we need to increment this value by one
    // since the index page (chunk 0) will also need to be generated.
    if ($context['sandbox']['max'] > 1) {
      $context['sandbox']['max']++;
    }
  }

  xmlsitemap_generate($context['sandbox']['progress'], $language);
  $context['sandbox']['progress']++;
  $context['message'] = t('Now generating @language sitemap page @chunk.', array('@language' => $language->name, '@chunk' => $context['sandbox']['progress']));

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch callback; sitemap rebuild finished.
 */
function xmlsitemap_rebuild_batch_finished($success, $results, $operations) {
  if ($success) {
    // Reset the rebuild flag since it was successful.
    variable_set('xmlsitemap_rebuild_needed', FALSE);
    drupal_set_message(t('The sitemap was rebuilt.'));
  }
  else {
    drupal_set_message(t('The sitemap was not successfully rebuilt.'), 'error');
  }
}

/**
 * Fetch a short blurb string about module maintainership and sponsors.
 *
 * This message will be FALSE in 'official' releases.
 */
function _xmlsitemap_get_blurb($check_version = TRUE) {
  static $blurb;

  if (!isset($blurb)) {
    $blurb = FALSE;
    $info = drupal_parse_info_file(drupal_get_path('module', 'xmlsitemap') . '/xmlsitemap.info');
    if (!$check_version || !isset($info['version']) || preg_match('/dev|alpha|beta/i', $info['version'])) {
      $sponsors = array(
        l('Symantec', 'http://www.symantec.com/'),
        l('WebWise Solutions', 'http://www.webwiseone.com/'),
        l('Volacci', 'http://www.volacci.com/'),
        l('lanetro', 'http://www.lanetro.com/'),
      );
      // Don't extract the following string for translation.
      $blurb = strtr('<p>Thank you for helping test the XML sitemap module rewrite. Please consider helping offset developer free time by <a href="@link-donate">donating</a> or if your company is interested in sponsoring the rewrite or a specific feature, please <a href="@link-contact">contact the developer</a>. Thank you to the following current sponsors: !sponsors, and all the indivduals that have donated. This message will not be seen in the stable versions.</p>', array('@link-review' => 'http://drupalmodules.com/module/xml-sitemap', '@link-donate' => 'http://davereid.chipin.com/', '@link-contact' => 'http://davereid.net/contact', '!sponsors' => implode(', ', $sponsors)));
    }
  }

  return $blurb;
}

/**
 * Check the status of all hook_requirements() from xmlsitemap modules.
 *
 * @param $return_only
 *   If TRUE, will return the result, otherwise it will show a message.
 * @return
 *   TRUE if there is a warning or error requirement, or FALSE otherwise.
 */
function xmlsitemap_check_status($return_only = FALSE) {
  // Load .install files
  include_once './includes/install.inc';
  drupal_load_updates();
  $warnings = FALSE;

  foreach (module_implements('requirements') as $module) {
    if (strpos($module, 'xmlsitemap') !== FALSE) {
      $requirements = module_invoke($module, 'requirements', 'runtime');
      if (drupal_requirements_severity($requirements) >= REQUIREMENT_WARNING) {
        $warnings = TRUE;
        break;
      }
    }
  }

  if ($warnings && !$return_only && user_access('administer site configuration')) {
    drupal_set_message(t('One or more problems were detected with your sitemap configuration. Please check the <a href="@status-report">status report</a> for more information.', array('@status-report' => url('admin/reports/status'))), 'warning', FALSE);
  }

  return $warnings;
}
