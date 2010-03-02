<?php
// $Id$

/**
 * @file
 * recipe_category_index.inc - This is an include file containing most all of the recipe category index page functionality.
 */

function recipe_category_index_page() {
  drupal_add_css(drupal_get_path('module', 'recipe') .'/recipe_category_index.css', 'module');

  // You are viewing a term
  $tree = array('current_term' => FALSE, 'term_tree' => array(), 'node_list' => FALSE, 'recent_list' => FALSE);
  $term = NULL;
  if ($tid = intval(arg(2))) {
    $term = taxonomy_get_term($tid);
    $vocabs = taxonomy_get_vocabularies('recipe');
    if (isset($vocabs[$term->vid])) {
      $tree['current_term'] = $term;
      $tree['term_tree'][] = taxonomy_get_tree($term->vid, $tid, -1, variable_get('recipe_index_depth', 4));
    }
  }
  // You are in the top level.
  else {
    if ($vocabs = taxonomy_get_vocabularies('recipe')) {
      foreach ($vocabs as $vocab) {
        $tree['term_tree'][] = taxonomy_get_tree($vocab->vid, 0, -1, variable_get('recipe_index_depth', 4));
      }
    }
  }

  // You are on the top level, don't display every recipe.
  if ( $term == NULL ) {
    // Nothing right now.
  }
  // You are view a term, build the node list.
  else {
    if ( variable_get('recipe_index_catalog_show_child_category_links', 1) == 0 ) {
      $tree['node_list'] = recipe_get_nodes_for_terms(array($term->tid));
    }
    else {
      $term_list = array($term->tid);
      foreach ($tree['term_tree'] as $vocab) {
          foreach ($vocab as $t) {
            $term_list[] = $t->tid;
          }
      }
      $tree['node_list'] = recipe_get_nodes_for_terms($term_list);
    }
  }

  return theme('recipe_category_index_page', $tree);
}


function theme_recipe_category_index_page($tree = NULL) {

  // Render the term tree first.
  $output = '<div class="recipe_index_category_list">';
  $output1 = '';
  foreach ($tree['term_tree'] as $vocab) {
    $output1 .= '<ul class="recipe_index_category_parents">';
    $depth = -1;
    foreach ($vocab as $term) {
      // You are just starting.
      if ( $depth == -1 ) {
        // Do nothing but prevent further checks
      }
      // You need to close the previous list item
      elseif ($term->depth == $depth) {
        $output1 .= '</li>';
      }
      // Go down a level.
      elseif ($term->depth > $depth) {
        $output1 .= '<ul class="recipe_index_category_children">';
      }
      // Go up some levels.
      while ($term->depth < $depth) {
        $output1 .=  '</li></ul>';
        $depth--;
      }

      $output1 .=  '<li>' . l($term->name, 'recipe/bycat/' . $term->tid);
      $depth = $term->depth;
    }
    // Clean up if you ended at a lower level.
    while ($depth > 0) {
      $output1 .=  '</li></ul>';
      $depth--;
    }
    $output1 .=  '</li></ul>';
  }
  if ($tree['current_term']) {
    $output .= theme('box', t('Categories: %term_name', array('%term_name' => $tree['current_term']->name)), $output1);
  }
  else {
    $output .= theme('box', t('Categories'), $output1);
  }
  $output .=  '</div>';

  // Set-up the right column.
  $output .= '<div class="recipe_index_node_lists">';

  // Render the node_list.
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
  if ($tree['current_term']) {
    drupal_set_title(t('%term', array('%term' => $tree['current_term']->name)));
    // Make the breadcrumbs pretty.
    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), NULL);
    $breadcrumb[] = l(t('Recipes'), 'recipe');
    $breadcrumb[] = l(t('By category'), 'recipe/bycat');
    if ($parents = taxonomy_get_parents_all($tree['current_term']->tid)) {
      // You don't want the current term to show up in the breadcrumbs.
      array_shift($parents);
      $parents = array_reverse($parents);
      foreach ($parents as $p) {
        $breadcrumb[] = l($p->name, 'recipe/bycat/'. $p->tid);
      }
    }
    drupal_set_breadcrumb($breadcrumb);
  }

  drupal_set_title(t('Find by'));
  return $output;
}


/**
 * Get recipes that match these terms.
 *
 * @return
 *   A database query result suitable for use the node_title_list.
 */
function recipe_get_nodes_for_terms($terms = array()) {
  if ( count($terms) == 0 ) {
    return FALSE;
  }
  $placeholders = implode(',', array_fill(0, count($terms), "%d"));
  $sql = "SELECT DISTINCT n.nid, n.title, n.sticky FROM {node} n, {term_node} tn WHERE n.nid=tn.nid AND tn.tid IN ($placeholders) AND n.type='recipe' AND n.status=1 ORDER BY n.sticky DESC, n.title";
  return db_query($sql, $terms);
}
