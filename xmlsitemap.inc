<?php
// $Id$

/**
 * @file
 * Miscellaneous functions for the xmlsitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * Given an internal Drupal path, return the alias for the path.
 *
 * This is similar to drupal_get_path_alias(), but designed to fetch all alises
 * at once so that only one database query is executed instead of several or
 * possibly thousands during sitemap generation.
 *
 * @param $path
 *   An internal Drupal path.
 * @param $language
 *   A language code to look use when looking up the paths.
 */
function xmlsitemap_get_path_alias($path, $language) {
  static $aliases;
  static $last_language;

  if (!isset($aliases)) {
    $aliases['all'] = array();
    $query = db_query("SELECT src, dst FROM {url_alias} WHERE language = '' ORDER BY pid");
    while ($alias = db_fetch_array($query)) {
      $aliases['all'][$alias['src']] = $alias['dst'];
    }
  }
  if ($language && $last_language != $language) {
    unset($aliases[$last_language]);
    $aliases[$language] = array();
    $query = db_query("SELECT src, dst FROM {url_alias} WHERE language = '%s' ORDER BY pid", $language);
    while ($alias = db_fetch_array($query)) {
      $aliases[$language][$alias['src']] = $alias['dst'];
    }
    $last_language = $language;
  }

  if ($language && isset($aliases[$language][$path])) {
    return $aliases[$language][$path];
  }
  elseif (isset($aliases['all'][$path])) {
    return $aliases['all'][$path];
  }
  else {
    return $path;
  }
}

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
  // Attempt to increase the available processing time and memory limit.
  @set_time_limit(240);
  _xmlsitemap_set_memory_limit();

  // Set a timer so we can track how long this takes.
  timer_start('xmlsitemap_regenerate');

  // Get the current memory usage so we can track how much memory is used.
  _xmlsitemap_get_memory_usage(TRUE);

  // Clear all cached sitemap files.
  xmlsitemap_clear_directory();
  xmlsitemap_check_directory();
}

function _xmlsitemap_get_memory_usage($start = FALSE) {
  static $memory_start;
  $current = 0;
  if (function_exists('memory_get_peak_usage')) {
    $current = memory_get_peak_usage(TRUE);
  }
  if (function_exists('memory_get_usage')) {
    $current = version_compare(PHP_VERSION, '5.2') ? memory_get_usage(TRUE) : memory_get_usage();
  }
  if (!isset($memory_start) || $start) {
    $memory_start = $current;
  }
  return $current - $memory_start;
}

function _xmlsitemap_get_optimal_memory_limit() {
  static $optimal_limit;
  if (!isset($optimal_limit)) {
    // Set the base memory amount from the provided core constant.
    $optimal_limit = parse_size(DRUPAL_MINIMUM_PHP_MEMORY_LIMIT);

    // Add memory based on the chunk size.
    $optimal_limit += xmlsitemap_get_chunk_size() * 500;

    // Add memory for storing the url aliases.
    $aliases = db_result(db_query("SELECT COUNT(pid) FROM {url_alias}"));
    $optimal_limit += $aliases * 250;
  }
  return $optimal_limit;
}

/**
 * Calculate the optimal memory level for sitemap generation.
 */
function _xmlsitemap_set_memory_limit() {
  $memory_limit = @ini_get('memory_limit');
  if ($memory_limit && $memory_limit != -1) {
    $optimal_limit = _xmlsitemap_get_optimal_memory_limit();
    if (parse_size($memory_limit) < $optimal_limit) {
      @ini_set('memory_limit', $optimal_limit);
    }
  }
}

/**
 * Perform operations after rebuilding the sitemap.
 */
function _xmlsitemap_regenerate_after() {
  // Show a watchdog message that the sitemap was regenerated.
  watchdog('xmlsitemap',
    'XML sitemap files regenerated in @timer ms. Peak memory usage: @memory-peak.',
    array(
      '@timer' => timer_read('xmlsitemap_regenerate'),
      '@memory-peak' => format_size(memory_get_peak_usage(TRUE)),
    ),
    WATCHDOG_NOTICE,
    l(t('View sitemap'), 'sitemap.xml')
  );

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
    // Don't bother translating this string.
    trigger_error('Improper condition hit in xmlsitemap_generate(). Chunk: ' . $chunk . ', Chunk Count: ' . xmlsitemap_get_chunk_count());
    return FALSE;
  }

  if (!isset($language)) {
    $language = language_default();
  }

  $file = xmlsitemap_get_chunk_file($chunk, $language->language);

  if (!$handle = fopen($file, 'wb')) {
    trigger_error(t('Could not open file @file for writing.', array('@file' => $file)));
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
    trigger_error(t('Unknown error occurred while writing to file @file.', array('@file' => $file)));
  }
  elseif (xmlsitemap_var('gz')) {
    $file_gz = xmlsitemap_get_chunk_file($chunk, $language->language, 'gz');
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
 * @param $type
 * @param $handle
 *   A file system pointer resource that is typically created using fopen().
 * @param $status
 * @param $language
 */
function xmlsitemap_generate_chunk_header($type, $handle, &$status, $language) {
  $output = '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL;

  // Add the stylesheet link.
  if (xmlsitemap_var('xsl')) {
    $xsl_url = url('sitemap.xsl', array('language' => $language, 'base_url' => xmlsitemap_var('base_url')));
    $output .= '<?xml-stylesheet type="text/xsl" href="' . $xsl_url . '"?>' . PHP_EOL;
  }

  $output .= '<' . $type . ' xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' . PHP_EOL;

  // This is the full XML header required for schema validation.
  //$schemas = array('sitemapindex' => 'siteindex.xsd', 'urlset' => 'sitemap.xsd');
  //$output .= '<' . $type . ' xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"' . PHP_EOL;
  //$output .= '  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"' . PHP_EOL;
  //$output .= '  xsi:schemaLocation="http://www.sitemaps.org/schemas/sitemap/0.9' . PHP_EOL;
  //$output .= '  http://www.sitemaps.org/schemas/sitemap/0.9/' . $schemas[$type] . '">' . PHP_EOL;

  $status &= (bool) fwrite($handle, $output);
  return $status;
}

/**
 * Generate one page (chunk) of the sitemap.
 *
 * @param $handle
 *   A file system pointer resource that is typically created using fopen().
 * @param $status
 *   A boolean that will be altered by reference with the success status of
 *   writing to $handle.
 * @param $chunk
 *   An integer representing the integer of the sitemap page chunk.
 * @param $language
 *   A language object for the sitemap chunk.
 */
function xmlsitemap_generate_chunk($handle, &$status, $chunk, $language) {
  static $languages;
  $last_url = '';

  if (!isset($languages)) {
    $languages = language_list();
    $languages[''] = language_default();
  }

  $query = array(
    'SELECT'   => 'SELECT x.loc, x.lastmod, x.changefreq, x.changecount, x.priority, x.language',
    'FROM'     => 'FROM {xmlsitemap} x',
    'WHERE'    => 'WHERE x.access = 1 AND x.status = 1',
    'ORDER BY' => 'ORDER BY x.language, x.loc',
  );
  $args = array();

  // Allow other modules to alter the sitemap query SQL and arguments.
  $data = &$query;
  $data['__drupal_alter_by_ref'] = array(&$args);
  drupal_alter('xmlsitemap_query', $data, $language);

  $sql = implode($query, ' ');
  $url_options = xmlsitemap_get_url_options(array('alias' => TRUE));
  $offset = max($chunk - 1, 0) * xmlsitemap_get_chunk_size();
  $limit = xmlsitemap_get_chunk_size();
  $query = db_query_range($sql, $args, $offset, $limit);

  // Add the XML header and XSL if desired.
  xmlsitemap_generate_chunk_header('urlset', $handle, $status, $language);

  while ($link = db_fetch_array($query)) {
    $url_options['language'] = $link['language'] ? $languages[$link['language']] : $language;
    $link['alias'] = xmlsitemap_get_path_alias($link['loc'], $url_options['language']->language);
    $link_url = url($link['alias'], $url_options);

    // Skip this link if it was a duplicate of the last one.
    // @todo Figure out a way to do this before generation so we can report
    // back to the user about this.
    if ($link_url == $last_url) {
      continue;
    }
    else {
      $last_url = $link_url;
    }

    $link_output = '<url><loc>' . $link_url . '</loc>';
    if ($link['lastmod']) {
      $link_output .= '<lastmod>' . gmdate(DATE_W3C, $link['lastmod']) . '</lastmod>';
      // If the link has a lastmod value, update the changefreq so that links
      // with a short changefreq but updated two years ago show decay.
      // We use abs() here just incase items were created on this same cron run
      // because lastmod would be greater than REQUEST_TIME.
      $link['changefreq'] = (abs(REQUEST_TIME - $link['lastmod']) + $link['changefreq']) / 2;
    }
    if ($link['changefreq']) {
      $link_output .= '<changefreq>' . xmlsitemap_get_changefreq($link['changefreq']) . '</changefreq>';
    }
    if (isset($link['priority']) && $link['priority'] != 0.5) {
      // Don't output the priority value for links that have 0.5 priority. This
      // is the default 'assumed' value if priority is not included as per the
      // sitemaps.org specification.
      $link_output .= '<priority>' . number_format($link['priority'], 1) . '</priority>';
    }
    $link_output .= '</url>' . PHP_EOL;
    $status &= (bool) fwrite($handle, $link_output);
  }

  // Close the XML file.
  $status &= (bool) fwrite($handle, '</urlset>' . PHP_EOL);

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
function xmlsitemap_generate_index($handle, &$status, $language) {
  $url_options = xmlsitemap_get_url_options(array('language' => $language, 'alias' => TRUE));
  $chunk_count = xmlsitemap_get_chunk_count(TRUE);

  // Add the XML header and XSL if desired.
  xmlsitemap_generate_chunk_header('sitemapindex', $handle, $status, $language);

  for ($i = 1; $i <= $chunk_count; $i++) {
    $output = '<sitemap>';
    $output .= '<loc>' . url('sitemap-' . $i . '.xml', $url_options) . '</loc>';
    // @todo Use the actual lastmod value of the chunk file.
    $output .= '<lastmod>' . gmdate(DATE_W3C, REQUEST_TIME) . '</lastmod>';
    $output .= '</sitemap>' . PHP_EOL;
    $status &= (bool) fwrite($handle, $output);
  }

  // Close the XML file.
  $status &= (bool) fwrite($handle, '</sitemapindex>' . PHP_EOL);

  return $status;
}

/**
 * Batch information callback.
 */
function xmlsitemap_rebuild_batch($modules = array(), $save_custom = FALSE) {
  $batch = array(
    'operations' => array(),
    'finished' => 'xmlsitemap_rebuild_batch_finished',
    'title' => t('Rebuilding Sitemap'),
    'file' => drupal_get_path('module', 'xmlsitemap') . '/xmlsitemap.inc',
  );

  // Purge any links first.
  $batch['operations'][] = array('xmlsitemap_rebuild_batch_clear', array($modules, $save_custom));

  // Fetch all the sitemap links and save them to the {xmlsitemap} table.
  foreach ($modules as $module) {
    if (module_hook($module, 'xmlsitemap_links')) {
      $batch['operations'][] = array('xmlsitemap_rebuild_batch_fetch', array($module));
    }
  }

  // Generate all the sitemap pages.
  $batch['operations'][] = array('_xmlsitemap_regenerate_before', array());
  $languages = language_list();
  foreach (xmlsitemap_var('languages') as $language) {
    $batch['operations'][] = array('xmlsitemap_rebuild_batch_generate', array($languages[$language]));
  }
  $batch['operations'][] = array('_xmlsitemap_regenerate_after', array());

  return $batch;
}

/**
 * Batch callback; clear sitemap links for modules.
 */
function xmlsitemap_rebuild_batch_clear($modules, $save_custom, &$context) {
  $purge = array();
  foreach ($modules as $module) {
    $types = module_invoke($module, 'xmlsitemap_link_info');
    foreach ($types as $type => $info) {
      if ($info['purge']) {
        $purge[] = $type;
      }
    }
  }

  if ($purge) {
    $where = array();
    $where[] = 'type IN (' . db_placeholders($purge, 'varchar') . ')';

    // If we want to save the custom data, make sure to exclude any links
    // that are not using default inclusion or priority.
    if ($save_custom) {
      $where[] = 'status_override = 0';
      $where[] = 'priority_override = 0';
    }

    $sql = "DELETE FROM {xmlsitemap} WHERE " . implode(' AND ', $where);
    db_query($sql, $purge);
  }

  $context['message'] = t('Purging links.');
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
    if (!$check_version || (($version = _xmlsitemap_get_version()) && preg_match('/dev|unstable|alpha|beta|HEAD/i', $version))) {
      $sponsors = array(
        l('Symantec', 'http://www.symantec.com/'),
        l('WebWise Solutions', 'http://www.webwiseone.com/'),
        l('Volacci', 'http://www.volacci.com/'),
        l('lanetro', 'http://www.lanetro.com/'),
        l('Coupons Dealuxe', 'http://couponsdealuxe.com/'),
      );
      // Don't extract the following string for translation.
      $blurb = '<p>Thank you for helping test the XML sitemap module rewrite. Please consider helping offset developer free time by <a href="http://davereid.chipin.com/">donating</a> or if your company is interested in sponsoring the rewrite or a specific feature, please <a href="http://davereid.net/contact">contact the developer</a>.</p><p>Thank you to the following current sponsors: ' . implode(', ', $sponsors) . ', and all the indivduals that have donated. This message will not be seen in the stable versions.</p>';
      //http://drupalmodules.com/module/xml-sitemap
    }
  }

  return $blurb;
}

function _xmlsitemap_get_version() {
  static $version;
  if (!isset($version)) {
    $modules = module_rebuild_cache();
    $version = $modules['xmlsitemap']->info['version'];
  }
  return $version;
}
