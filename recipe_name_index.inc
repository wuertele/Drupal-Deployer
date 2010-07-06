<?php
// $Id$

/**
 * @file
 * recipe_name_index.inc - This is an include file containing most all of the recipe name index page functionality.
 */

function recipe_name_index_page() {
  drupal_add_css(drupal_get_path('module', 'recipe') .'/recipe_name_index.css', 'module');

  $node_list = recipe_get_alpha_nodes();
  $alpha_list = recipe_get_node_alpha_list($node_list);
  return theme('recipe_name_index_page', $alpha_list, $node_list);
}


function theme_recipe_name_index_page($alpha_list = NULL, $node_list = NULL) {

  // Render the alpha pager.
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

  if (isset($alpha_list['0-9'])) {
    $list[] = l('[0-9]', 'recipe/byname', array('fragment' => 'alpha_0-9'));
  }
  else {
    $list[] = '[0-9]';
  }



  $output .= implode(" - ", $list);
  $output .=  '</div>';

  $output .= '<div class="recipe_name_list">';

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
          $output1 .= '<fieldset><legend><a name="alpha_'. $node->letter . '">'. strtoupper($node->letter) .'</a></legend><p>';
          $last_alpha = $node->letter;
        }
        $output1 .= l($node->title, 'node/'. $node->nid) . '<br/>';
      }
      $output1 .= "</p></fieldset>";
      $output .= $output1;
    }
    else {
      $output .= theme('box', t('No matching recipes'));
    }
  }

  $output .= "</div>";

  drupal_set_title(t('Find by recipe name'));
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


function recipe_get_node_alpha_list(&$node_list = NULL) {
  $alpha_list = array();
  foreach ($node_list as $n) {
    $letter = '';
    if ( preg_match('/([A-Za-z0-9])/', $n->title, $matches) ) {
      $letter = strtolower($matches[1]);
    }
    else {
      $letter = strtolower(substr($n->title, $i, 1));
    }

    if ( is_numeric($letter) ) {
      $letter = '0-9';
    }

    $n->letter = $letter;
    $alpha_list[$letter] = 1;
  }

  usort($node_list, 'node_list_cmp');
  return $alpha_list;
}


function node_list_cmp($a, $b) {
  // Sort first by letter.
  if ( $a->letter !=  $b->letter ) {
    return ($a->letter < $b->letter) ? -1 : 1;
  }

  // Next by title
  if ( $a->title !=  $b->title ) {
    return ($a->title < $b->title) ? -1 : 1;
  }

  // They are the same.
  return 0;
}
