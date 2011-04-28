<?php
// $Id$

/**
 * @file
 * Page callbacks for recipe index page.
 */

function recipe_landing_page() {
  return theme('recipe_landing_page');
}


function theme_recipe_landing_page() {
  drupal_set_title(t("Recipes Menu"));

  // Add the recipe.css file for this page.
  drupal_add_css(drupal_get_path('module', 'recipe') .'/recipe.css');

  // Get a list of current sub-menus and render them as a list.
  $item = menu_get_item();
  $content = system_admin_menu_block($item);
  $output = '<div class="recipe_index_list">';
  $output .= theme('node_add_list', $content);

  // Add create recipes link if user has perms.
  if (user_access('create recipes')) {
    $output .= '<dl class="node-type-list"><dt>'. l('Add a new Recipe', 'node/add/recipe') .'</dt><dd>' . t('Add a new recipe to the site.') .'</dd></dl>';
  }

  $output .= '</div>';

  // Render the recent recipes list.
  if (variable_get('recipe_recent_box_enable', 1) == 1) {
    $node_list = recipe_get_latest(variable_get('recipe_recent_display', 5));
    $output .= '<div class="recipe_index_recent">';
    $output .= theme('box', variable_get('recipe_recent_box_title', t('Latest recipes')), node_title_list($node_list));
    $output .= '</div>';
  }

  return $output;
}
