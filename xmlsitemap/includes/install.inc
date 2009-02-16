<?php
// $Id$

/**
 * @file
 * Support file for modules installation.
 */

/**
 * @addtogroup xmlsitemap
 * @{
 */

/**
 * Register a module in the xmlsitemap_modules table.
 * @param $module
 *  The module name, without extension.
 * @param $info
 *   An object containing data about the module.
 * @return
 *   The ID associated with the module.
 */
function xmlsitemap_register_module($module, $info) {
  if (($mid = db_result(db_query("SELECT mid FROM {xmlsitemap_modules} WHERE name = '%s'", $module))) !== FALSE) {
    return $mid;
  }
  if (drupal_write_record('xmlsitemap_modules', $info)) {
    return $info->mid;
  }
  return 0;
}