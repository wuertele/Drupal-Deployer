<?php
// $Id$

/**
 * @file
 * Documentation for xmlsitemap API.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Retrieve a array of links to include in the sitemap.
 *
 * @return
 *   An array of link arrays with the following keys and values:
 *   - 'type' => The type of link (node, user, kitten, etc.).
 *   - 'id' => The ID of the link ($node->nid, $user->uid, etc.).
 *   - 'loc' => The un-aliased Drupal path to the item.
 *   - 'lastmod' => The UNIX timestmap of when the item was last modified.
 *   - 'changefreq' => The interval, in seconds, between the last set of changes.
 *   - 'priority' => An optional priority value between 0.0 and 1.0.
 *
 * @see hook_xmlsitemap_links_clear()
 */
function hook_xmlsitemap_links() {
  $links = array();

  $links[] = array(
    'type' => 'kitten',
    'id' => 1,
    'loc' => 'mymodule/menu/path',
    'lastmod' => 346245692,
    'changefreq' => 4600,
  );

  return $links;
}

/**
 * Batch version of hook_xmlsitemap_links().
 *
 * It is highly recommended that if your module has a lot of items that could
 * be sitemap links, that you implement this hook.
 */
function hook_xmlsitemap_links_batch(&$context) {

}

/**
 * Clear the links created by hook_xmlsitemap_links().
 *
 * @see hook_xmlsitemap_links()
 */
function hook_xmlsitemap_links_clear() {
  db_query("DELETE FROM {xmlsitemap} WHERE type = 'mymodule'");
}

/**
 * Alter the data of a sitemap link before the link is saved.
 *
 * @param $link
 *   An array with the data of the sitemap link.
 */
function hook_xmlsitemap_link_alter(&$link) {
  if ($link['type'] == 'mymodule') {
    $link['priority'] += 0.5;
  }
}

/**
 * Alter the list of sitemap engines.
 */
function hook_xmlsitemap_engines_alter(&$engines) {
  $engines['kitten_engine'] = array(
    'name' => t('Kitten Search'),
    'url' => 'http://kittens.com/ping?sitemap=[sitemap]',
    'verify_url' => FALSE,
    'verify_key' => FALSE,
  );
}

/**
 * @} End of "addtogroup hooks".
 */
