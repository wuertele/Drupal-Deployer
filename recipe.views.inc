<?php
// $Id$

/**
 * @file
 * recipe.views.inc - Views2 support for Recipe.
 *
 * Views support for the recipe module includes recipe lists, ingredient lists, and unit lists.
 *
 * Added by Scott Prive Dec 2009
 */


/* Recipe Schema: This will probably get out of date(it already is!), but once this works better let's remove this.

node.nid +--------------+     +------------------+     +-------------+     +--------------+
     ^   | recipe       |     | _node_ingredient |     | _ingredient |     | _unit        |
     |   +--------------+     +------------------+     +-------------+     +--------------+
     +---| nid          |<--  | id               |   +-| id          |  +--| id           |
         | source       |  +--| nid              |   | | name        |  |  | name         |
         | yield        |     | unit_id          |<-+| | link        |  |  | abbreviation |
         | instructions |     | quantity         |  || +-------------+  |  | metric       |
         | notes        |     | ingredient_id    |<-|+                  |  | type         |
         | preptime     |     +------------------+  +-------------------+  +--------------+
         +--------------+

 TODO:
  1) Impliment a  pre_render_list based handler function, to get all recipe ingredients based on a recipe nid.
  The idea is to have that fetch the ingredients, so I avoid the duplicate fields.
  (Anything that's not an ingredient should appear once, but gets duplicated).
  views_handler_field_amazon_participant.inc contains an example solution.

*/



/**
 * Implementation of hook_views_data().
 */
function recipe_views_data() {


  /*
   * Recipe table section.
   */

  // Group
  $data['recipe']['table']['group'] = t('Recipe');

  /* Base
   * It doesn't really make sense to include the recipe table by itself.  It doesn't even include the recipe title.
   * Since we include the recipe table joined to node you get all of the recipe fields anyway.
   */

  // Joins
  $data['recipe']['table']['join']['node'] = array(
      'left_field'    => 'nid',
      'field'         => 'nid',
  );

  // Fields
  $data['recipe']['source'] = array(
    'title' => t('Source'),
    'help' => t("Optional. Does anyone else deserve credit for this recipe?"),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe']['yield'] = array(
    'title' => t('Yield'),
    'help' => t("The number of servings for this recipe."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe']['instructions'] = array(
    'title' => t('Instructions'),
    'help' => t("The instructions for this recipe."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe']['notes'] = array(
    'title' => t('Notes'),
    'help' => t("The notes for this recipe."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe']['preptime'] = array(
    'title' => t('Preptime'),
    'help' => t("The preptime for this recipe."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );


  /*
   * Ingredient table section.
   */

  // Group
  $data['recipe_ingredient']['table']['group'] = t('Recipe');

  // Base
  $data['recipe_ingredient']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Recipe ingredient'),
    'help' => t("The recipe ingredients list."),
    'weight' => -10,
  );

  // Joins

  // Fields
  $data['recipe_ingredient']['name'] = array(
    'title' => t('Ingredient Name'),
    'help' => t("An ingredient name."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );


  /*
   * Unit table section.
   */

  // Group
  $data['recipe_unit']['table']['group'] = t('Recipe');

  // Base
  $data['recipe_unit']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Recipe Unit'),
    'help' => t("The recipe unit list."),
    'weight' => -10,
  );

  // Joins

  // Fields
  $data['recipe_unit']['id'] = array(
    'title' => t('Recipie unit ID (recipe_unit.id)'),
    'help' => t('The id (recipe_unit.id) of the recipe unit.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'name',
      'numeric' => TRUE,
      'validate type' => 'id',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe_unit']['name'] = array(
    'title' => t('Unit Name'),
    'help' => t("The recipe measurement unit name."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe_unit']['abbreviation'] = array(
    'title' => t('Unit Abbreviation'),
    'help' => t("The abbreviation for the recipe unit."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe_unit']['metric'] = array(
    'title' => t('Is Metric?'),
    'help' => t('Denotes if measurement unit is metric.'),
    // Information for displaying the id
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['recipe_unit']['type'] = array(
    'title' => t('Type'),
    'help' => t("The unittype for this ingredient."),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );


  return $data;
}
