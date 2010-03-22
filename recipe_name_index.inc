<?php
// $Id$

/**
 * @file
 * recipe_name_index.inc - This is an include file containing most all of the recipe category index page functionality.
 */

function recipe_name_index_page() {
  drupal_add_css(drupal_get_path('module', 'recipe') .'/recipe_name_index.css', 'module');

  $node_list = recipe_get_alpha_nodes();
  $alpha_list = recipe_get_alpha_list($node_list);
  return theme('recipe_name_index_page', $alpha_list, $node_list);
}


function theme_recipe_name_index_page($alpha_list = NULL, $node_list = NULL) {

  // Render the term tree first.
  $output = '<div class="recipe_name_alpha_list">';
  $list = array();
  foreach (range('a', 'z') as $letter ) {
    if (isset($alpha_list[$letter])) {
      $list[] = l(strtoupper($letter), 'recipe/byname', array('fragment' => 'alpha_'. $letter));
    }
    else {
      $list[] = $letter;
    }

  }
  $output .= implode(" - ", $list);
  $output .=  '</div>';

  $output .= '<div class="recipe_name_nodes">';

  // Render the node_list next.
  if ($node_list != NULL) {
    if ( count($node_list) > 0 ) {
      $output1 = '';
      $last_alpha = '-';
      foreach ($node_list as $node) {
        if ( $node->letter != $last_alpha ) {
          if ( $last_alpha != '-' ) {
            $output1 .= "</p></fieldset>";
          }
          $output1 .= "<fieldset><legend>$node->letter</legend><p>";
          $last_alpha = $node->letter;
        }
        $output1 .= '<a name="alpha_'. $node->letter . '"></a>' . l($node->title, 'node/'. $node->nid) . '<br/>';
      }
      $output1 .= "</p></fieldset>";
      $output .= theme('box', t('Recipes'), $output1);
    }
    else {
      $output .= theme('box', t('No matching recipes'));
    }
  }

  $output .= "</div>";

  // Set-up the breadcrumb tail.
  if ($tree['current_ingredient']) {
    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), NULL);
    $breadcrumb[] = l(t('Recipes'), 'recipe');
    $breadcrumb[] = l(t('By name'), 'recipe/byname');
    drupal_set_breadcrumb($breadcrumb);
  }

  drupal_set_title(t('Find by name'));
  return $output;
}


/**
 * Get recipes that have these ingredients.
 *
 * @return
 *   A database query result suitable for use the node_title_list.
 */
function recipe_get_alpha_nodes() {
  $list = array();
  $result = db_query("SELECT DISTINCT n.nid, n.title, n.sticky FROM {node} n WHERE n.type='recipe' AND n.status=1 ORDER BY n.title, n.sticky DESC");
  while ($node = db_fetch_object($result)) {
    $list[] = $node;
  }
  return $list;
}


function recipe_get_alpha_list(&$node_list = NULL) {
  $alpha_list = array();
  foreach ($node_list as $n) {
    $letter = strtolower(substr($n->title, 0, 1));
    $n->letter = $letter;
    $alpha_list[$letter] = 1;
  }
  return $alpha_list;
}


function my_title_list($node_list, $title = NULL) {
  $items = array();
  foreach ($node_list as $node) {
    $items[] = '<a name="alpha_'. $node->letter . '"></a>' . l($node->title, 'node/'. $node->nid);
  }

  return theme('node_list', $items, $title);
}
