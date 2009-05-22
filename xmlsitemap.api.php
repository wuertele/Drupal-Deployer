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
    'type' => 'mymodule',
    'id' => 1,
    'loc' => 'mymodule/menu/path',
    'lastmod' => 346245692,
    'changefreq' => 4600,
  );

  return $links;
}

/**
 * Provide batch information for hook_xmlsitemap_links().
 *
 * It is highly recommended that if your module has a lot of items that could
 * be sitemap links, that you implement this hook.
 *
 * All you need to do to implement this hook is add the required $context
 * information.
 *
 * The optional current value will provide the offset parameter to
 * hook_xmlsitemap_links() and should get records that are greater than this
 * value. The default value is 0.
 *
 * The max (count) value will allow the batch to know when it is finished. This
 * value is required.
 */
function hook_xmlsitemap_links_batch_info() {
  return array(
    'current' => 0,
    // This value is used to start selecting items (WHERE id > current).
    'max' => db_result(db_query("SELECT COUNT(id) FROM {mymodule}")),
    // This should be the total number of items to process.
  );
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
  );
}

/**
 * @} End of "addtogroup hooks".
 */
