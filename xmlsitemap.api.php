<?php
// $Id$

/**
 * @file
 * Hooks provided by the XML sitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Provide information on the type of links this module provides.
 */
function hook_xmlsitemap_link_info() {
  return array(
    'mymodule' => array(
      'purge' => TRUE, // A boolean if this link type can be purged during a rebuild.
    ),
  );
}

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
 * Alter the query selecting data from {xmlsitemap} during sitemap generation.
 *
 * Do not alter LIMIT or OFFSET as the query will be passed through
 * db_query_range() with a set limit and offset.
 *
 * @param $query
 *   An array of a query object, keyed by SQL keyword (SELECT, FROM, WHERE, etc).
 * @param $args
 *   An array of arguments to be passed to db_query() with $query.
 * @param $language
 *   The language object being used for sitemap generation.
 */
function hook_xmlsitemap_query_alter(array &$query, array &$args, stdClass $language) {
  $query['WHERE'] .= " AND x.language = '%s'";
  $args[] = $language->language;
}

/**
 * @} End of "addtogroup hooks".
 */
