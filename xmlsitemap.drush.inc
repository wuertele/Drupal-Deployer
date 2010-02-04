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
  $items['xmlsitemap-regenerate'] = array(
    'description' => 'Regenerate the XML sitemap files.',
    'drupal dependencies' => array('xmlsitemap'),
  );
  $items['xmlsitemap-rebuild'] = array(
    'description' => 'Dump and re-process all possible XML sitemap data, and then regenerate the files.',
    'drupal dependencies' => array('xmlsitemap'),
  );
  $items['xmlsitemap-index'] = array(
    'description' => 'Process un-indexed XML sitemap links.',
    'drupal dependencies' => array('xmlsitemap'),
    'options' => array(
      '--limit' => 'The limit of links of each type to process. Default value: ' . variable_get('xmlsitemap_batch_limit', 100),
    ),
  );
  return $items;
}

/**
 * Regenerate the sitemap files from existing data.
 */
function drush_xmlsitemap_regenerate() {
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
  module_load_include('inc', 'xmlsitemap');
  timer_start('xmlsitemap_rebuild');

  // Set the rebuild flag incase something fails during the rebuild.
  variable_set('xmlsitemap_rebuild_needed', TRUE);

  // Build the batch array.
  $modules = module_implements('xmlsitemap_link_info');
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

/**
 * Process un-indexed XML sitemap links.
 */
function drush_xmlsitemap_index() {
  $limit = (int) drush_get_option('limit', variable_get('xmlsitemap_batch_limit', 100));
  module_invoke_all('xmlsitemap_index_links', $limit);
}
