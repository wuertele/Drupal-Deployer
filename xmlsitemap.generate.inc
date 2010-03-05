<?php
// $Id$

/**
 * @file
 * Sitemap generation and rebuilding functions for the xmlsitemap module.
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

function xmlsitemap_get_url_options_from_context(array $context) {
  $options = module_invoke_all('xmlsitemap_context_url_options', $context);
  drupal_alter('xmlsitemap_context_url_options', $options, $context);
  return $options;
}

/**
 * @todo Replace with table of sitemap objects.
 */
function xmlsitemap_get_contexts() {
  $contexts = array();

  if (module_exists('xmlsitemap_i18n')) {
    foreach (xmlsitemap_var('languages') as $langcode) {
      $contexts[] = array('language' => $langcode);
    }
  }
  else {
    // Add an empty, default context.
    $contexts[] = array();
  }

  $hashes = array_map('md5', array_map('serialize', $contexts));
  $contexts = array_combine($hashes, $contexts);

  return $contexts;
}

/**
 * Delete and regenerate the sitemap files.
 */
function xmlsitemap_regenerate() {
  _xmlsitemap_regenerate_before();

  // Generate the sitemap pages.
  $contexts = xmlsitemap_get_contexts();
  $chunk_count = xmlsitemap_get_chunk_count(TRUE);
  foreach ($contexts as $context) {
    // Generate index.
    if ($chunk_count > 1) {
      xmlsitemap_generate('index', $context);
    }
    for ($i = 1; $i <= $chunk_count; $i++) {
      xmlsitemap_generate($i, $context);
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

  // Clear the maximum chunk and file size variables.
  variable_set('xmlsitemap_max_chunks', 0);
  variable_set('xmlsitemap_max_filesize', 0);
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
    // Set the base memory am