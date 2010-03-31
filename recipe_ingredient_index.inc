<?php
// $Id$

/**
 * @file
 * recipe_ingredient_index.inc - This is an include file containing most all of the recipe category index page functionality.
 */

function recipe_ingredient_index_page() {
  drupal_add_css(drupal_get_path('module', 'recipe') .'/recipe_ingredient_index.css', 'module');

  $page_args = array('current_ingredient' => FALSE, 'ingredient_list' => array(), 'node_list' => array(), 'alpha_list' => array());
  // This is the incoming ingredient id.
  // You are viewing an ingredient, don't get the ingredient list
  if ($iid = intval(arg(2))) {
    $page_args['current_ingredient'] = array_shift(recipe_get_ingredients($iid));
  }

  // You are in the top level, get the ingredient list.
  else {
    $page_args['ingredient_list'] = recipe_get_ingredients();
    $page_args['alpha_list'] = recipe_get_ingredient_alpha_list($page_args['ingredient_list']);
  }

  // You have an ingredient id, build the node list.
  if ( $page_args['current_ingredient'] ) {
    $page_args['node_list'] = recipe_get_nodes_for_ingredients(array($page_args['current_ingredient']->id));
  }

  return theme('recipe_ingredient_index_page', $page_args);
}


function theme_recipe_ingredient_index_page($page_args = NULL) {
  $output = '';


  if (!$page_args['current_ingredient']) {

    // Render the alpha pager.
    $output .= '<div class="recipe-ingredient-alpha">';
    $list = array();
    foreach (range('a', 'z') as $letter ) {
      if (isset($page_args['alpha_list'][$letter])) {
        $list[] = l(strtoupper($letter), 'recipe/bying', array('fragment' => 'alpha_'. $letter));
      }
      else {
        $list[] = $letter;
      }

    }
    $output .= implode(" - ", $list);
    $output .=  '</div>';

    // Render the ingredient_list.
    if ($page_args['ingredient_list'] != NULL) {
      if ( count($page_args['ingredient_list']) > 0 ) {
        $output1 = '<div class="recipe-ingredient-list">';
        $last_alpha = '-';
        foreach ($page_args['ingredient_list'] as $ingredient) {
          if ( $ingredient->letter != $last_alpha ) {
            if ( $last_alpha != '-' ) {
              $output1 .= "</p></fieldset>";
            }
            $output1 .= '<fieldset><legend><a name="alpha_'. $ingredient->letter . '">'. strtoupper($ingredient->letter) .'</a></legend><p>';
            $last_alpha = $ingredient->letter;
          }
          $output1 .= l($ingredient->name, 'recipe/bying/'. $ingredient->id) . '<br/>';
        }
        $output1 .= "</p></fieldset>";
        $output1 .= '</div>';
        $output .= $output1;
      }
      else {
        $output .= theme('box', t('No matching ingredients'));
      }
    }
  }

  else {
    // render the matching node list.
    if ($page_args['node_list']) {
      $output .= '<div class="recipe-ingredient-nodes">';
      $output .= '<fieldset><legend>'. $page_args['current_ingredient']->name .'</legend><p>';
      foreach ($page_args['node_list'] as $node) {
        $output .= l($node->title, 'node/'. $node->nid) . '<br/>';
      }
      $output .= '</p></fieldset>';
      $output .= "</div>";
    }
  }

  // Set-up the breadcrumb tail.
  if ($page_args['current_ingredient']) {
    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), NULL);
    $breadcrumb[] = l(t('Recipes'), 'recipe');
    $breadcrumb[] = l(t('By ingredient'), 'recipe/bying');
    drupal_set_breadcrumb($breadcrumb);
  }

  drupal_set_title(t('Find by ingredient name'));
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
  $list = array();
  $placeholders = implode(',', array_fill(0, count($ids), "%d"));
  $sql = "SELECT DISTINCT n.nid, n.title, n.sticky FROM {node} n, {recipe_node_ingredient} rni WHERE n.nid=rni.nid AND rni.ingredient_id IN ($placeholders) AND n.type='recipe' AND n.status=1 ORDER BY n.sticky DESC, n.title";
  $result = db_query($sql, $ids);
  while ($node = db_fetch_object($result)) {
    $list[] = $node;
  }
  return $list;
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


function recipe_get_ingredient_alpha_list(&$ingredient_list = NULL) {
  $alpha_list = array();
  foreach ($ingredient_list as $i) {
    $letter = strtolower(substr($i->name, 0, 1));
    $i->letter = $letter;
    $alpha_list[$letter] = 1;
  }
  return $alpha_list;
}
