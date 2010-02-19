<?php
// $Id$


function recipe_index_page_form() {
  drupal_add_css(drupal_get_path('module', 'recipe') .'/recipe_index.css', 'module');
  $form = array();
  $form['#theme'] = 'recipe_index_page_form';

  // This array will hold the level we are showing
  // as $tid => $name
  $terms = array();
  $args = explode(',',arg(1));
  foreach ($args as $i=>$term) {
    if (intval($term)) {
      $tid = intval($term);
      $term = taxonomy_get_term($tid);
      $terms[$tid] = $term->name;
    }
  }
  if (count($terms)) {
    $top_level = false;
  } else {
    $top_level = true;
    // Go get all the top-level terms for applicable vocabs
    $vocabs = taxonomy_get_vocabularies('recipe');
    // If no vocabs are defined, we can't go any further, so abort out
    // with an administrator warning.
    if (! count($vocabs)) {
      $vars = array();
      $msg = "No categories are defined for the recipe module.";
      if (user_access('administer taxonomy')) {
        $vars['!admin_link'] = l(t('category administration'),'admin/content/taxonomy');
        $msg .= " (go to !admin_link)";
      }
      $form['message'] = array(
        '#type'=>'markup',
        '#value'=>t($msg, $vars),
      );
      watchdog('recipe', $msg, $vars, WATCHDOG_WARNING, l(t('administer'), 'admin/content/taxonomy'));
      return $form;
    }
    foreach ($vocabs as $vocab) {
      $tree = taxonomy_get_tree($vocab->vid, 0, -1, 1);
      foreach ($tree as $term) {
        $terms[$term->tid] = $term->name;
      }
    }
  }

  // At this point, the array $terms contains $tid=>$name for all of the
  // terms at the current level. For the top level, it will often have
  // multiple elements. For lower levels, typically only one unless the
  // search specified a comma-separated list in the URL, e.g.,
  // path "links/weblink/2,5,7" .

  $all_parents = array();
  $weight = 0;
  foreach ($terms as $tid=>$name) {
    $node_count = taxonomy_term_count_nodes($tid);
    $node_count = '('. $node_count .')';
    $element = array('#tree'=>true, '#theme'=>'recipe_index_category', '#weight'=>$weight++);
    $element['term'] = array(
      '#type' => 'markup',
      '#tree' => true,
      '#value' => l($name . $node_count, 'recipe/'.$tid),
    );
    $term_nodes[$tid] = array();
    $result = db_query("SELECT n.nid, n.title FROM {node} n, {term_node} tn WHERE n.nid=tn.tid AND tn.tid=%d AND n.type='recipe' AND n.status=1 ORDER BY n.title", $tid);
    while ($node = db_fetch_object($result)) {
      $term_nodes[$tid][] = $node;
    }
    $children = taxonomy_get_children($tid);
    $element['children'] = array(
      '#theme' => 'recipe_index_category_children',
      '#tree' => true,
    );
    $weight2 = 0;
    foreach ($children as $child) {
      $node_count = taxonomy_term_count_nodes($child->tid);
      $node_count = '('. $node_count .')';
      $element['children'][] = array(
        '#type' => 'markup',
        '#value' => l($child->name . $node_count, 'recipe/'.$child->tid),
        '#weight' => $weight2++,
        '#prefix' => '<li class="recipe_index_category_child">',
        '#suffix' => '</li>',
      );
    }
    $parents = taxonomy_get_parents($tid);
    foreach ($parents as $parent) {
      $all_parents[$parent->tid] = l($parent->name, 'recipe/'.$parent->tid);
    }
    $form[$tid] = $element;
  }

  if ($top_level) {
    $title = t('Recipes');
  } else {
    $title = t('Recipes: %topics', array('%topics'=>implode(', ',$terms)));
    $breadcrumbs = drupal_get_breadcrumb();
    $breadcrumbs[] = l(t('Recipes'), 'recipe/index');
    if (count($all_parents)) {
      $breadcrumbs[] = implode(', ', $all_parents);
    }
    drupal_set_breadcrumb($breadcrumbs);
  }
  drupal_set_title($title);

  // List the nodes at the current level

  $terms_to_list = array();
  if(variable_get('recipe_index_catalog_show_child_category_links','1')) {
    $terms_to_list = $terms;
    if (!$top_level) {
      $terms_to_list += $children;
    }
  } else {
    if (!$top_level)
      $terms_to_list[$tid] = $term;
  }

  $form['nodes'] = array(
    '#tree' => true,
  );
  $i = 0;
  if (count($terms_to_list)) {
    $sql = "SELECT DISTINCT n.nid, n.title FROM {node} n, {term_node} tn WHERE n.nid=tn.nid AND tn.tid IN (%s) AND n.type='recipe' AND n.status=1 ORDER BY n.title";
    $result = db_query($sql, implode(',', array_keys($terms_to_list)));
    while ($node = db_fetch_object($result)) {
      $node_html = node_view(node_load($node->nid), true, false, true);
      $form['nodes'][$i++] = array(
        '#type' => 'markup',
        '#value' => $node_html,
      );
    }
  } else {
    $html .= t("<p>There are no recipes defined at this level of the category tree.</p>\n");
  }

  return $form;
}

/**
 * Display a view of the recipes
 */
function recipe_index_page() {
  $html = "";
  $html .= drupal_get_form('recipe_index_page_form');
  return $html;
}

function theme_recipe_index_category_children($children) {
  $kids = "";
  foreach ($children as $i=>$kid) {
    if (is_int($i)) {
      $kids .= drupal_render($kid);
    }
  }
  if (! empty($kids)) {
    $output = '<ul class="recipe_index_category_children">';
    $output .= $kids;
    $output .= '</ul>';
  }
  return $output;
}

function theme_recipe_index_category($element) {
//print("_category<br>");
  $output = '<div class="recipe_index_category">';
  $output .= '<div class="recipe_index_category_parent">' . drupal_render($element['term']) . '</div>';
  if (count($element['children'])) {
    $output .= drupal_render($element['children']);
  }
  $output .= '</div>';
  return $output;
}

function theme_recipe_index_page_form($form) {
  // First, check for a message. If that's set, then nothing else applies.
  if (is_array($form['message'])) {
    return drupal_render($form['message']);
  }
  $output = "<div class='recipe_links_section'>";
  if (user_access('create recipes')) {
    $output .= l('Add a new Recipe', 'node/add/recipe');
  }
  $output .= "</div>";
  $output .= '<h3 class="recipe_index_section">' . t('Categories') . '</h3>';

  $columns = variable_get('recipe_index_category_cols','2');
  // Current column (0, 1,...$columns-1)
  $col = 0;
  $row = 0;
  $table = array();
  foreach ($form as $i=>$element) {
    if (is_int($i)) {
      if (! is_array($table[$row])) {
        $table[$row] = array();
        $table[$row]['data'] = array();
      }
      $table[$row]['data'][$col++] = drupal_render($element);
      if ($col >= $columns) {
        $col = 0;
        $row++;
      }
    }
  }
  // Finish out a row
  while ($col < $columns) {
    $table[$row]['data'][$col++] = '';
  }
  // Apply row attributes
  foreach (array_keys($table) as $i) {
    $table[$i]['class'] = 'recipe_index_category_row';
    $table[$i]['valign'] = 'top';   // STUB for testing without CSS file
  }
  $output .= theme('table',NULL,$table);

  if ( strlen(arg(1)) == 0 ) {
    if (variable_get('recipe_recent_box_enable', 1)) {
      // Add latest recipes and xml feed icon.
      $output .= theme('box', variable_get('recipe_recent_box_title', t('Latest recipes')), node_title_list(recipe_get_latest(variable_get('recipe_recent_display', '5'))) . theme('recipe_more_info', theme('feed_icon', url("recipe/feed"), t('Syndicate'))));
    }
  }
  else {
    // Add the recipes at this level and (optionally) child level
    $node_html = '';
    if (is_array($form['nodes']) && count($form['nodes']) > 0) {
      foreach ($form['nodes'] as $i=>$element) {
        if (is_int($i)) {
          $node_html .= drupal_render($element);
        }
      }
    }

    if ($node_html != '') {
      $output .= '<h3 class="recipe_index_section">' . t('Recipes') . '</h3>';
      $output .= $node_html;
    } else {
      $output .= '<p>' . t('(No recipes in this category)') . '</p>';
    }
  }

  return $output;
}


/**
 * Get the latest recipes.
 *
 * @return
 *   A database query result.
 */
function recipe_get_latest($count = 0) {
  $sql = "SELECT nid, title, sticky, created FROM {node} n WHERE status = 1 AND type = 'recipe' ORDER BY sticky DESC, created DESC";
  return db_query_range($sql, 0, $count);
}


/**
 * Displays more information content, such as "more" links, and feed images.
 *
 * @return
 *   Formatted string containing the output.
 */
function theme_recipe_more_info($content) {
  return '<div class="recipe-more-link">' . $content . '</div>';
}
