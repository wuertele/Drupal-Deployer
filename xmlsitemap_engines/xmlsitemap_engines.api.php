<?php
// $Id$

/**
 * @file
 * Documentation for xmlsitemap_engines API.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter the list of sitemap engines.
 */
function hook_xmlsitemap_engines_alter(&$engines) {
  $engines['kitten_engine'] = array(
    'name' => t('Kitten Search'),
    'url' => 'http://kittens.com/ping?sitemap=[sitemap]',
  );
}

/**
 * @} End of "addtogroup hooks".
 */
