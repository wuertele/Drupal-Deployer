<?php
// $Id$

/**
 * The key function that builds the menu links whenever there is a menu rebuild.
 */
function _admin_menu_rebuild_links() {
  // Get the newly rebuilt menu
  $menu = menu_router_build();
  // Add normal and suggested items as links.
  $menu_links = array();
  foreach ($menu as $path => $item) {
    // Exclude menu callbacks, include items below admin/* and node/add/*.
    if ($item['type'] != MENU_CALLBACK && (($item['_parts'][0] == 'admin' && count($item['_parts']) > 1) || (strpos($path, 'node/add') !== FALSE))) {
      // TODO: handle local tasks with wildcards
      if (!strpos($path, '%')) {
        $item = admin_menu_link_build($item);
        $menu_links[$path] = $item;
        $sort[$path] = $item['_number_parts'];
      }
    }
  }
  $deleted = admin_menu_adjust_items($menu_links, $sort);
  if ($menu_links) {
    // Make sure no child comes before its parent.
    array_multisort($sort, SORT_NUMERIC, $menu_links);

    foreach ($menu_links as $item) {
      admin_menu_link_save($item);
    }
  }
  // Allow modules to add more links. If you want to alter links saved by
  // admin_menu, use hook_menu_link_alter() and look for
  // $item['module'] == 'admin_menu'
  $links = module_invoke_all('admin_menu', $deleted);
  foreach ($links as $item) {
    admin_menu_link_save($item);
  }
}

/**
 * Prepare a menu link from basic information formatted for a router item.
 */
function admin_menu_link_build($item) {
  $item['module'] = 'admin_menu';
  $item['menu_name'] = 'admin_menu';
  $item += array(
    'link_title' => $item['title'],
    'link_path' => $item['path'],
    'hidden' => 0,
    'options' => array(),
  );
  $item['options']['alter'] = TRUE;
  // DAM does not output item descriptions to prevent mouseover clashes and
  // increase page loading performance.  However, the following code shows how
  // link attributes can be added (for ajaxified DAM functionality later).
  /*
  if (!empty($item['description'])) {
    $item['options']['attributes']['title'] = $item['description'];
  }
  */
  if (!empty($item['query'])) {
    $item['options']['query'] = $item['query'];
  }
  return $item;
}

/**
 * Convenience function that looks up the plid if $item['parent_path'] is set.
 */
function admin_menu_link_save($item) {
  $item = admin_menu_link_build($item);

  // Check whether we are able to update an existing item.
  $existing_item = db_fetch_array(db_query("SELECT mlid, plid, has_children FROM {menu_links} WHERE link_path = '%s' AND menu_name = '%s'", $item['link_path'], 'admin_menu'));
  if ($existing_item) {
    $item['mlid'] = $existing_item['mlid'];
    $item['plid'] = $existing_item['plid'];
    $item['has_children'] = $existing_item['has_children'];
  }

  // Look up the parent path for both new and existing links, since the parent
  // may change.
  if (isset($item['parent_path'])) {
    if ($item['parent_path'] == '<root>') {
      // <root> means that we want the link at the top level.
      $item['plid'] = 0;
    }
    else {
      $plid = db_result(db_query("SELECT mlid from {menu_links} WHERE link_path = '%s' AND menu_name = '%s'", $item['parent_path'], 'admin_menu'));
      if ($plid) {
        $item['plid'] = $plid;
      }
    }
  }

  menu_link_save($item);
}

/**
 * Implementation of hook_admin_menu().
 *
 * @param &$deleted
 *   Array of links under admin/* that were removed by admin_menu_adjust_items().
 *   If one of these links is added back, it should be removed from the array.
 */
function admin_menu_admin_menu(&$deleted) {
  global $base_url;

  $links = array();
  $icon_path = drupal_get_normal_path(variable_get('site_frontpage', 'node'));

  // Add 'administer' item to the icon menu.
  $links[] = array(
    'title' => 'Administer',
    'path' => 'admin',
    'weight' => 10,
    'parent_path' => $icon_path,
  );

  // Add system update links.
  $links[] = array(
    'title' => 'Run cron',
    'path' => 'admin/reports/status/run-cron',
    'weight' => 50,
    'parent_path' => $icon_path,
  );
  $links[] = array(
    'title' => 'Run updates',
    'path' => $base_url .'/update.php',
    'weight' => 50,
    'parent_path' => $icon_path,
  );

  // Move 'By module' item into Site configuration.
  if (isset($deleted['admin/by-module'])) {
    $deleted['admin/by-module']['parent_path'] = 'admin/settings';
    $deleted['admin/by-module']['weight'] = -10;
    $links[] = $deleted['admin/by-module'];
    unset($deleted['admin/by-module']);
  }

  // Add link to drupal.org.
  $links[] = array(
    'title' => 'Drupal.org',
    'path' => 'http://drupal.org',
    'weight' => 100,
    'parent_path' => $icon_path,
  );
  // Add links to project issue queues.
  $links[] = array(
    'title' => 'Drupal issue queue',
    'path' => 'http://drupal.org/project/issues/drupal',
    'weight' => -10,
    'parent_path' => 'http://drupal.org',
  );
  foreach (module_list(FALSE, FALSE, TRUE) as $module) {
    $info = drupal_parse_info_file(drupal_get_path('module', $module) .'/'. $module .'.info');
    if (!isset($info['project']) || $info['project'] == 'drupal') {
      continue;
    }
    $url = 'http://drupal.org/project/issues/'. $info['project'];
    $links[] = array(
      'title' => check_plain($info['name']) . ' issue queue',
      'path' => $url,
      'parent_path' => 'http://drupal.org',
    );
  }

  // Add 'Create <content-type>' items to Content management menu.
  if (isset($deleted['node/add'])) {
    $deleted['node/add']['parent_path'] = 'admin/content';
    $deleted['node/add']['weight'] = 0;
    $links[] = $deleted['node/add'];
    unset($deleted['node/add']);
  }
  foreach($deleted as $path => $item) {
    if (strpos($path, 'node/add') !== FALSE) {
      $links[] = $deleted[$path];
      unset($deleted[$path]);
    }
  }
  // Make sure longer paths are after shorter ones
  ksort($deleted);
  foreach (node_get_types('types', NULL, TRUE) as $type) {
    $type_url_str = str_replace('_', '-', $type->type);
    $type_path = 'admin/content/node-type/' . $type_url_str;
    $links[$type_path] = array(
      'title' => 'Edit @content-type',
      'path' => $type_path,
      'parent_path' => 'admin/content/types',
      'options' => array('t' => array('@content-type' => $type->name)),
    );
    unset($deleted['admin/content/node-type/' . $type_url_str . '/edit']);
    // CCK and other modules adding to node types handled here.
    foreach($deleted as $path => $item) {
      if (strpos($path, $type_path) !== FALSE) {
        // Logically, parent path can never go shorter than $type_path.
        $i = $item['_number_parts'] - 1;
        do {
          $parent_path = implode('/', array_slice($item['_parts'], 0, $i));
          --$i;
        } while (!isset($links[$parent_path]) && $i);
        $item['parent_path'] = $parent_path;
        $links[$path] = $item;
        unset($deleted[$path]);
      }
    }
  }

  // Add devel module links
  if (module_exists('devel')) {
    // Add variable editor.
    $links[] = array(
      'title' => 'Variable editor',
      'path' => 'devel/variable',
      'weight' => 20,
      'parent_path' => $icon_path,
    );
    // Add clear-cache.
    $links[] = array(
      'title' => 'Empty cache',
      'path' => 'devel/cache/clear',
      'weight' => 20,
      'parent_path' => $icon_path,
    );
    // Add switch_user items.
    if ($devel_user_links = module_invoke('devel', 'switch_user_list')) {
      foreach ($devel_user_links as $link) {
        if (preg_match('!href="'. base_path() .'([^\?]+)\?([^"]+)" title="([^"]+)">((<em>)?[^<]+(</em>)?)!', $link, $match)) {
          $links[] = array(
            'title' => $match[4],
            'description' => $match[3],
            'path' => $match[1],
            'weight' => 20,
            'query' => 'destination',
            'parent_path' => 'logout',
            'options' => array('html' => TRUE),
          );
        }
      }
    }
  }

  return $links;
}

/**
 * Add some hard-coded features for better user experience.
 *
 * @param array $menu_links
 *   An array containing the complete administration menu structure, passed by
 *   reference.
 * @param array $sort
 *   An array containing the # parts of each link - must be updated if a link
 *   is added.
 * @return
 *   An array of links that were removed from $menu_links.
 */
function admin_menu_adjust_items(&$menu_links, &$sort) {
  global $user, $base_url;
  $links = array();
  $deleted = array();

  // Change or remove items, or add new top-level items.
  $deleted['admin/by-module'] = $menu_links['admin/by-module'];
  unset($menu_links['admin/by-module'], $sort['admin/by-module']);
  $deleted['admin/by-task'] = $menu_links['admin/by-task'];
  unset($menu_links['admin/by-task'], $sort['admin/by-task']);

  // Remove certain links to re-position them in admin_menu_admin_menu().
  foreach ($menu_links as $path => $link) {
    // Remove links below
    // - admin/content/node-type/*
    // - node/add*
    if (strpos($path, 'admin/content/node-type/') !== FALSE || strpos($path, 'node/add') !== FALSE) {
      $deleted[$path] = $link;
      unset($menu_links[$path], $sort[$path]);
    }
  }

  // Add the icon containing special links.
  $links[] = array(
    'title' => theme('admin_menu_icon'),
    'path' => drupal_get_normal_path(variable_get('site_frontpage', 'node')),
    'weight' => -100,
    'parent_path' => '<root>',
    'options' => array('extra class' => 'admin-menu-icon', 'html' => TRUE),
  );
  // Add link to show current authenticated/anonymous users - we will add the
  // data dynamically in the _alter hook.
  $links[] = array(
    'title' => 'icon_users',
    'path' => 'user',
    'weight' => -90,
    'parent_path' => '<root>',
    'options' => array('extra class' => 'admin-menu-action admin-menu-icon admin-menu-users', 'html' => TRUE),
  );
  $links[] = array(
    'title' => 'Log out @username',
    'path' => 'logout',
    'weight' => -100,
    'parent_path' => '<root>',
    // Note: @username is dynamically replaced by default, we just invoke
    // replacement by setting the 't' key here.
    'options' => array('extra class' => 'admin-menu-action admin-menu-logout', 't' => array()),
  );
  foreach ($links as $item) {
    $path = $item['path'];
    $item = admin_menu_link_build($item);
    $menu_links[$path] = $item;
    $sort[$path] = 1;
  }

  return $deleted;
}

