<?php
// $Id$

/**
 * @file
 * Drush integration functions for the xmlsitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * Implements hook_drush_command().
 */
function xmlsitemap_drush_command() {
  $items['xmlsitemap regenerate'] = array(
    'description' => 'Regenerate the XML sitemap files.',
    'callback' => 'drush_xmlsitemap_regenerate',
  );
  $items['xmlsitemap rebuild'] = array(
    'description' => 'Dump and re-process all possible XML sitemap data, and then regenerate the files.',
    'callback' => 'drush_xmlsitemap_rebuild',
  );
  return $items;
}

/**
 * Check that the sitemap files directory exists and is writable.
 */
function drush_xmlsitemap_check_directory() {
  $dir = file_create_path(xmlsitemap_var('path'));
  if (!file_check_directory($dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    drush_die(dt('The XML sitemap cache directory @directory was not found was not writable.', array('@directory' => $dir)));
  }
}

/**
 * Regenerate the sitemap files from existing data.
 */
function drush_xmlsitemap_regenerate() {
  drush_xmlsitemap_check_directory();
  module_load_include('inc', 'xmlsitemap');
  xmlsitemap_regenerate();

  $vars = array(
    '@timer' => timer_read('xmlsitemap_regenerate'),
    '@memory-peak' => format_size(memory_get_peak_usage(TRUE)),
   );
  drush_print(dt('XML sitemap files regenerated in @timer ms. Peak memory usage: @memory-peak.', $vars));
}

/**
 * Dump and rebuild all the sitemap data, then regenerate the files.
 */
function drush_xmlsitemap_rebuild() {
  drush_xmlsitemap_check_directory();
  module_load_include('inc', 'xmlsitemap');
  timer_start('xmlsitemap_rebuild');

  // Set the rebuild flag incase something fails during the rebuild.
  variable_set('xmlsitemap_rebuild_needed', TRUE);

  // Build the batch array.
  $modules = module_implements('xmlsitemap_links_clear', TRUE);
  $batch = xmlsitemap_rebuild_batch($modules, TRUE);
  $batch['progressive'] = FALSE;
  batch_set($batch);

  // We need to manually set the progressive variable again.
  // @todo Remove when http://drupal.org/node/638712 is fixed.
  $batch =& batch_get();
  $batch['progressive'] = FALSE;

  // Run the batch process.
  batch_process();

  $vars = array(
    '@timer' => timer_read('xmlsitemap_rebuild'),
    '@memory-peak' => format_size(memory_get_peak_usage(TRUE)),
   );
  drush_print(dt('XML sitemap files rebuilt in @timer ms. Peak memory usage: @memory-peak.', $vars));
}
