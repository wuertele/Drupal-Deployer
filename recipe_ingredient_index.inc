<?php
// $Id$

/**
 * @file
 * recipe_ingredient_index.inc - This is an include file containing most all of the recipe category index page functionality.
 */

function recipe_ingredient_index_page() {
  drupal_add_css(drupal_get_path('module', 'recipe') .'/recipe_ingredient_index.css', 'module');

  $tree = array('current_ingredient' => NULL, 'ingredient_list' => array(), 'node_list' => array());
  // This is the incoming ingredient id.
  // You are viewing an ingredient, don't get the ingredient list
  if ($iid = intval(arg(2))) {
    $tree['current_ingredient'] = array_shift(recipe_get_ingredients($iid));
  }
  // You are in the top level, get the ingredient list.
  else {
    $tree['ingredient_list'] = recipe_get_ingredients();
  }

  // You are on the top level, don't display every recipe.
  if ( $tree['current_ingredient'] == NULL ) {

  }
  // You have an ingredient id, build the node list.
  else {
    $tree['node_list'] = recipe_get_nodes_for_ingredients(array($iid));
  }

  return theme('recipe_ingredient_index_page', $tree);
}


function theme_recipe_ingredient_index_page($tree = NULL) {
  // Render the term tree first.
  $output = '<div class="recipe_ingredient_list">';
  $output1 = '<ul>';
  foreach ($tree['ingredient_list'] as $ingredient) {
    $output1 .= '<li>' . l($ingredient->name, 'recipe/bying/'.$ingredient->id) . '</li>';
  }
  $output1 .= '</ul>';
  if ($tree['current_ingredient'] != NULL) {
    $output .= theme('box', t('Ingredients: %ingredient_name', array('%ingredient_name' => $tree['current_ingredient']->name)), $output1);
  }
  else {
    $output .= theme('box', t('Ingredients'), $output1);
  }
  $output .=  '</div>';

  // Set-up the right column.
  $output .= '<div class="recipe_ingredient_nodes">';

  // Render the node_list next.
  if ($tree['node_list']) {
    $list = node_title_list($tree['node_list']);
    if ( strlen($list) > 0 ) {
      $output .= theme('box', t('Matching recipes'), $list);
    }
    else {
      $output .= theme('box', t('No matching recipes'));
    }
  }

  $output .= "</div>";

  // Set-up the breadcrumb tail.


  drupal_set_title(t('Find by'));
  return $output;
}


/**
 * Get recipes that have these ingredients.
 *
 * @return
 *   A database query result suitable for use the node_title_list.
 */
function recipe_get_nodes_for_ingredients($ids = array()) {
  if ( count($ids) == 0 ) {
    return FALSE;
  }
  $placeholders = implode(',', array_fill(0, count($ids), "%d"));
  $sql = "SELECT DISTINCT n.nid, n.title, n.sticky FROM {node} n, {recipe_node_ingredient} rni WHERE n.nid=rni.nid AND rni.ingredient_id IN ($placeholders) AND n.type='recipe' AND n.status=1 ORDER BY n.sticky DESC, n.title";
  return db_query($sql, $ids);
}


function recipe_get_ingredients($iid = NULL) {
  if ( $iid == NULL ) {
    $result = db_query("SELECT DISTINCT ri.id, ri.name FROM {recipe_ingredient} ri, {recipe_node_ingredient} rni WHERE ri.id=rni.ingredient_id ORDER BY name");
  }
  else {
    $result = db_query("SELECT DISTINCT ri.id, ri.name FROM {recipe_ingredient} ri, {recipe_node_ingredient} rni  WHERE ri.id=rni.ingredient_id AND ri.id=%d ORDER BY name", $iid);
  }
  $list = array();
  while ($row = db_fetch_object($result)) {
    $list[] = $row;
  }
  return $list;
}