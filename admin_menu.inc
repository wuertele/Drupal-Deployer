<?php
// $Id$

/**
 * @file
 * Cached builder functions for Drupal Administration Menu.
 */

/**
 * Build the administration menu tree.
 *
 * This is only called if the menu has been modified, i.e. the menu cache has
 * been cleared.
 *
 * @param int $mid_admin
 *   The menu item id to use for the administration menu.
 */
function admin_menu_build($mid_admin) {
  global $_menu;
  $_admin_menu = array();
  
  // Retrieve the full menu structure.
  $_admin_menu = admin_menu_tree_all_data();
  
  // Adjust some menu items for better user experience.
  #admin_menu_adjust_items($_admin_menu);
  
  // Allow other modules to integrate with admin_menu.
  foreach (module_implements('admin_menu') as $module) {
    $function = $module .'_admin_menu';
    #$function($_admin_menu, TRUE);
  }
  
  return $_admin_menu;
}

/**
 * Get the data structure representing the administrative menu tree.
 *
 * @see menu_tree_all_data()
 */
function admin_menu_tree_all_data() {
  // chx way. 13/03/2008 sun
  // $item_admin = db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE link_path = 'admin' AND menu_name = 'navigation'"));
  $item_admin = db_fetch_array(db_query("SELECT * FROM {menu_links} WHERE router_path = 'admin'"));
  $item_admin = array_merge_recursive($item_admin, menu_get_item('admin'));

  // chx way. 13/03/2008 sun
  // $data['tree'] = menu_tree_all_data('navigation', $item_admin);
  // return $data['tree'];

  $args = $parents = array($item_admin['mlid']);
  $placeholders = implode(', ', array_fill(0, count($args), '%d'));
  
  // Collect all links as well as all of their children.
  do {
    $result = db_query("SELECT mlid FROM {menu_links} WHERE menu_name = '%s' AND has_children = 1 AND plid IN (". $placeholders .') AND mlid NOT IN ('. $placeholders .')', array_merge(array($item_admin['menu_name']), $args, $args));
    $num_rows = FALSE;
    while ($item = db_fetch_array($result)) {
      $args[] = $item['mlid'];
      $num_rows = TRUE;
    }
    $placeholders = implode(', ', array_fill(0, count($args), '%d'));
  } while ($num_rows);

  // Build db_query arguments array.
  array_unshift($args, $item_admin['menu_name']);
  
  // Until now, everything seems to be a wanted behaviour of the new menu system
  // in Drupal 6.  However, {menu_links} does not contain any local tasks, and
  // because of that, it's getting crude now.
  
  // Select the links from the table, and recursively build the tree. We
  // LEFT JOIN since there is no match in {menu_router} for an external
  // link.
  // Union select all local tasks from {menu_router}
  // - that start with 'admin/',
  // - using the mlid of the corresponding tab_parent path in {menu_links} as plid,
  // - using the depth of the tab_parent path in {menu_links} + 1 as depth.
  $types = array(MENU_LOCAL_TASK, MENU_DEFAULT_LOCAL_TASK);
  $args = array_merge($args, $types);
  $data['tree'] = menu_tree_data(db_query("(
    SELECT m.load_functions, m.to_arg_functions, m.access_callback, m.access_arguments, m.page_callback, m.page_arguments, m.title, m.title_callback, m.title_arguments, m.type, ml.*
    FROM {menu_links} ml LEFT JOIN {menu_router} m ON m.path = ml.router_path
    WHERE ml.menu_name = '%s' AND ml.plid IN (". $placeholders .")
  ) UNION (
    SELECT m.load_functions, m.to_arg_functions, m.access_callback, m.access_arguments, m.page_callback, m.page_arguments, m.title, m.title_callback, m.title_arguments, m.type,
      ml.menu_name, ml.mlid + 100000 as mlid, ml.plid, ml.link_path, ml.router_path, m.title as link_title, ml.options, ml.module, ml.hidden, ml.external, ml.has_children, ml.expanded, ml.weight, ml.depth + 1 as depth, ml.customized, ml.p1, ml.p2, ml.p3, ml.p4, ml.p5, ml.p6, ml.p7, ml.p8, ml.p9, ml.updated
    FROM {menu_router} m LEFT JOIN {menu_links} ml ON ml.router_path = m.tab_parent
    WHERE ml.router_path REGEXP '[^%%]*' AND m.path LIKE 'admin/%%' AND m.type IN (". db_placeholders($types) .")
  )
  ORDER BY p1 ASC, p2 ASC, p3 ASC, p4 ASC, p5 ASC, p6 ASC, p7 ASC, p8 ASC, p9 ASC", $args), $parents);
  
  $data['node_links'] = array();
  menu_tree_collect_node_links($data['tree'], $data['node_links']);
  
  // Check access for the current user to each item in the tree.
  menu_tree_check_access($data['tree'], $data['node_links']);

  return $data['tree'];
}

/**
 * Recursively adds items to the administration menu.
 *
 * Any accessible menu items are added, including local tasks.
 *
 * @param array $_admin_menu
 *   An array containing the complete administration menu structure, passed by
 *   reference.
 * @param array $item
 *   The (starting) menu item to use for the administration menu.
 */
function _admin_menu_get_children(&$_admin_menu, &$item) {
  global $_menu;
  
  if (isset($item['children'])) {
    foreach ($item['children'] as $child) {
      // Check access permissions.
      if (!variable_get('admin_menu_show_all', 0)) {
        $item_is_accessible = !isset($_menu['items'][$child]['access']) || (isset($_menu['items'][$child]['access']) && $_menu['items'][$child]['access']);
      }
      else {
        $item_is_accessible = TRUE;
      }
      // Check menu item type.
      $item_is_visible = $_menu['items'][$child]['type'] & (MENU_VISIBLE_IN_TREE | MENU_IS_LOCAL_TASK);
      
      // Create the child item if it is accessible and visible.
      // Additional condition to hide items linking to parent:
      //   && !($_menu['items'][$child]['type'] & MENU_LINKS_TO_PARENT)
      if ($item_is_accessible && $item_is_visible) {
        $_admin_menu[$child] = $_menu['items'][$child];
        admin_menu_item_url($_admin_menu, $child);
        
        // Recurse to child menu items.
        if (isset($_menu['items'][$child]['children'])) {
          _admin_menu_get_children($_admin_menu, $_admin_menu[$child]);
        }
        
        // Remove this child item if it is visible in the navigation.
        unset($_menu['visible'][$child]);
      }
      else {
        // Remove child menu item from parent's children list.
        $parent_id = $_menu['items'][$child]['pid'];
        if (isset($_menu['items'][$parent_id]['children']) && is_array($_menu['items'][$parent_id]['children'])) {
          $child_key = array_search($child, $_menu['items'][$parent_id]['children']);
          if ($child_key !== FALSE) {
            unset($_admin_menu[$parent_id]['children'][$child_key]);
          }
        }
      }
    }
  }
}

/**
 * Add some hard-coded features for better user experience.
 *
 * @param array $_admin_menu
 *   An array containing the complete administration menu structure, passed by
 *   reference.
 */
function admin_menu_adjust_items(&$_admin_menu) {
  global $_menu, $user, $base_url;
  
  // Add the icon containing special links.
  $mid_admin = $_admin_menu['index']['admin'];
  $mid_icon = admin_menu_add_item($_admin_menu, $mid_admin, array(
    'title' => theme('admin_menu_icon'),
    'path' => drupal_get_normal_path(variable_get('site_frontpage', 'node')),
    'weight' => -100,
    'class' => 'admin-menu-icon',
  ));
  $_admin_menu['index']['admin_menu_icon'] = $mid_icon;
  
  // Add 'administer' item to the icon menu.
  $mid_icon_admin = admin_menu_add_item($_admin_menu, $mid_icon, array('title' => 'Administer', 'path' => 'admin', 'weight' => 10));
  // ...and reset 'administer' menu item id in path index.
  $_admin_menu['index']['admin'] = $mid_admin;
  
  // Move 'By module' item if accessible, or remove it.
  $mid_bymodule = $_admin_menu['index']['admin/by-module'];
  if (user_access('administer site configuration')) {
    admin_menu_move_item($_admin_menu, $mid_bymodule, $mid_icon_admin);
  }
  else {
    admin_menu_remove_item($_admin_menu, 'admin/by-module');
  }
  
  // Remove 'By task' menu item.
  admin_menu_remove_item($_admin_menu, 'admin/by-task');
  
  // Add system update links.
  admin_menu_add_item($_admin_menu, $mid_icon, array('title' => 'Run cron', 'path' => 'admin/reports/status/run-cron', 'weight' => 50, 'query' => drupal_get_destination()));
  if ($user->uid == 1) {
    admin_menu_add_item($_admin_menu, $mid_icon, array('title' => 'Run updates', 'path' => $base_url .'/update.php', 'weight' => 50));
  }
  
  // Add links to drupal.org.
  if (user_access('display drupal links')) {
    $mid_drupal = admin_menu_add_item($_admin_menu, $mid_icon, array('title' => 'Drupal.org', 'path' => 'http://drupal.org', 'weight' => 100));
    admin_menu_add_item($_admin_menu, $mid_drupal, array('title' => 'Drupal issue queue', 'path' => 'http://drupal.org/project/issues/drupal'));
    
    // Add links to project issue queues.
    foreach (module_list(FALSE, FALSE, TRUE) as $module) {
      $info = drupal_parse_info_file(drupal_get_path('module', $module) .'/'. $module .'.info');
      if (isset($info['project']) && $info['project'] == 'drupal') {
        continue;
      }
      $url = 'http://drupal.org/project/issues/'. $module;
      // Filter project versions via query string not yet supported.
      // @see http://drupal.org/node/97569
      // $url .= !empty($info['version']) ? '/'. $info['version'] : '';
      admin_menu_add_item($_admin_menu, $mid_drupal, array('title' => '@title issue queue', 'title arguments' => array('@title' => $info['name']), 'path' => $url));
    }
  }
  
  // Add logout item.
  if ($user->uid > 0) {
    $mid_logout = $_menu['path index']['logout'];
    admin_menu_add_item($_admin_menu, $mid_admin, array(
      'title' => 'Logout @name',
      'title arguments' => array('@name' => $user->name),
      'path' => $_menu['items'][$mid_logout]['path'],
      'weight' => -100,
      'class' => 'admin-menu-action admin-menu-logout',
    ));
  }
  
  // Add 'Create <content_type>' menu items to Content management > Content.
  // If an user does not have 'administer nodes' permission, we need to copy
  // the 'Create content' item from the Navigation menu into 'Content
  // management' and append node/add/* items there instead.
  if (!user_access('administer nodes')) {
    $mid_content_management = $_admin_menu['index']['admin/content'];
    $mid_node_add = $_menu['path index']['node/add'];
    $create_content = $_menu['items'][$mid_node_add];
    $create_content['weight'] = 0;
    $mid_content = admin_menu_add_item($_admin_menu, $mid_content_management, $create_content);
  }
  else {
    $mid_content = $_admin_menu['index']['admin/content/node'];
  }
  $mid_node_add = $_menu['path index']['node/add'];
  admin_menu_copy_items($_admin_menu, $mid_node_add, $mid_content, 'Add @title');
}

/**
 * Recursively copy menu items from a source parent menu item to a target item.
 *
 * @param array $_admin_menu
 *   An array containing the complete administration menu structure, passed by
 *   reference.
 * @param int $source_pid
 *   A source parent menu item id from which children shall be copied.
 * @param int $target_pid
 *   A target parent menu item id.
 * @param string $title
 *   An optional string containing the token !title, that has already been
 *   passed through t(), which will be used to dynamically replace previous
 *   menu item titles.
 * @param bool $tree
 *   Whether to rebuild the complete hierarchy from the source parent menu item
 *   or copy menu items flattened. Defaults to TRUE.
 */
function admin_menu_copy_items(&$_admin_menu, $source_pid, $target_pid, $title = NULL, $tree = TRUE) {
  global $_menu;
  
  if (isset($_menu['items'][$source_pid]['children']) && isset($_admin_menu[$target_pid])) {
    foreach ($_menu['items'][$source_pid]['children'] as $mid) {
      $item = $_menu['items'][$mid];
      if (!$item['access']) {
        continue;
      }
      if (isset($title)) {
        $item['title'] = $title;
        $item['title arguments'] = array('@title' => $_menu['items'][$mid]['title']);
      }
      // Only add child to target if it does not already exist.
      if (!in_array($mid, $_admin_menu[$target_pid]['children'])) {
        admin_menu_add_item($_admin_menu, $target_pid, $item);
      }
      
      // Recurse into children.
      if (isset($_menu['items'][$mid]['children']) && count($_menu['items'][$mid]['children'])) {
        if ($tree) {
          admin_menu_copy_items($_admin_menu, $mid, $mid, $title);
        }
        else {
          admin_menu_copy_items($_admin_menu, $mid, $target_pid, $title, FALSE);
          unset($_admin_menu[$mid]['children']);
          // Note: Uncomment following lines to remove unnecessary parent items.
          // unset($_admin_menu[$target_pid]['children'][array_search($mid, $_admin_menu[$target_pid]['children'])]);
          // unset($_admin_menu[$mid]);
        }
      }
    }
  }
}

/**
 * Moves the child pointer of a menu item to a new parent.
 *
 * @param array $_admin_menu
 *   An array containing the complete administration menu structure, passed by
 *   reference.
 * @param int $mid
 *   The menu item id of the item to move.
 * @param int $pid
 *   The menu item id of the new parent.
 */
function admin_menu_move_item(&$_admin_menu, $mid, $pid) {
  global $_menu;
  
  if (isset($_admin_menu[$mid]) && isset($_admin_menu[$pid])) {
    // Remove current child pointer. Determine the path from the index
    // because it contains the unprocessed urls.
    $index = array_flip($_admin_menu['index']);
    admin_menu_remove_item($_admin_menu, $index[$mid]);
    
    // Insert new child pointer.
    $_admin_menu[$mid]['pid'] = $pid;
    $_admin_menu[$pid]['children'][] = $mid;
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Removes the child pointer for a menu item.
 *
 * @param array $_admin_menu
 *   An array containing the complete administration menu structure, passed by
 *   reference.
 * @param string $path
 *   The menu path to remove, e.g. 'foo/bar'.
 */
function admin_menu_remove_item(&$_admin_menu, $path) {
  global $_menu;
  
  $mid = $_admin_menu['index'][$path];
  if (isset($_admin_menu[$mid])) {
    $pid = $_admin_menu[$mid]['pid'];
    $child_key = array_search($mid, $_admin_menu[$pid]['children']);
    if ($child_key !== FALSE) {
      unset($_admin_menu[$pid]['children'][$child_key]);
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  return FALSE;
}

/**
 * Render an icon to display in the Administration Menu.
 *
 * @ingroup themeable
 */
function theme_admin_menu_icon() {
  return '<img class="admin-menu-icon" src="'. (theme_get_setting('toggle_favicon') ? theme_get_setting('favicon') : base_path() .'misc/favicon.ico') .'" width="16" height="16" alt="" />';
}

