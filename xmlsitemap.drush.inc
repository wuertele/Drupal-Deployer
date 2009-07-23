<?php
// $Id$

/**
 * @file
 * Drush integration functions for the xmlsitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * Implementation of hook_drush_command().
 */
function xmlsitemap_drush_command() {
  $items['xmlsitemap regenerate'] = array(
    'description' => 'Regenerate the XML sitemap files.',
    'callback' => 'drush_xmlsitemap_regenerate',
  );
  return $items;
}

function drush_xmlsitemap_regenerate() {
  module_load_include('inc', 'xmlsitemap');
  xmlsitemap_regenerate();

  $vars = array(
    '@timer' => timer_read('xmlsitemap_regenerate'),
    '@memory-usage' => format_size(_xmlsitemap_get_memory_usage()),
    '@memory-peak' => format_size(memory_get_peak_usage(TRUE)),
    '@memory-limit' => format_size(parse_size(@ini_get('memory_limit'))),
    '@memory-optimal' => format_size(_xmlsitemap_get_optimal_memory_limit()),
  );
  drush_print(dt('XML sitemap files regenerated in @timer ms. Memory used: @memory-usage, peak: @memory-peak.', $vars));
}
