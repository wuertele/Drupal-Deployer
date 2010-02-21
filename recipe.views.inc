<?php

// $Id$
// views2 integration with Recipe 1.x for Drupal 6

/**
 * @file
 * recipe.views.inc - Views2 support for recipe 1.x under Drupal 6.x.
 *
 * Added by Scott Prive Dec 2009
 *
 */

/* TODO:
  1) Impliment a  pre_render_list based handler function, to get all recipe ingredients based on a recipe nid.
  The idea is to have that fetch the ingredients, so I avoid the duplicate fields.
  (Anything that's not an ingredient should appear once, but gets duplicated).
  views_handler_field_amazon_participant.inc contains an example solution.


/**
 * Implementation of hook_views_data().
 */

function recipe_views_data() {
    /* joins
      
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
    */
    
    // BASE TABLES
    $data['recipe']['table']['base'] = array(
        'field' => 'nid',
        'title' => t('RecipeNode'),
        'help' => t("Nodes are a Drupal site's primary content. Recipes are also nodes."),
        'weight' => -10,
    );
    $data['recipe_node_ingredient']['table']['base'] = array(
        'field' => 'nid',
        'title' => t('RecipeNodeIngredient'),
        'help' => t("Nodes are a Drupal site's primary content. Recipes are also nodes."),
        'weight' => -10,
    );
    
    // JOINS
    // join Recipe nid to Node nid
    $data['recipe']['table']['join']['node'] = array(
        'left_field'    => 'nid',
        'field'         => 'nid',
    );
    //join Recipe Node Ingredient's nid to Node.nid
    $data['recipe_node_ingredient']['table']['join']['recipe'] = array(
        'left_field'    => 'nid',
        'field'         => 'nid',
    );
    $data['recipe_ingredient']['table']['join']['recipe'] = array(
        'left_table'    => 'recipe_node_ingredient',
        'left_field'    => 'ingredient_id',
        'field'         => 'id',
    ); 
    $data['recipe_unit']['table']['join']['recipe'] = array(
        'left_table'    => 'recipe_node_ingredient',
        'left_field'    => 'unit_id',
        'field'         => 'id',
    );
    

    
    // ----------------------------------------------------------------
    // recipe table
    //mysql> desc recipe
    //+--------------+------------------+------+-----+---------+----------------+
    //| Field        | Type             | Null | Key | Default | Extra          |
    //+--------------+------------------+------+-----+---------+----------------+
    //| nid          | int(10) unsigned | NO   | PRI | NULL    | auto_increment | 
    //| source       | varchar(255)     | YES  |     | NULL    |                | 
    //| yield        | int(11)          | NO   |     | NULL    |                | 
    //| instructions | text             | YES  |     | NULL    |                | 
    //| notes        | text             | YES  |     | NULL    |                | 
    //| preptime     | int(10) unsigned | YES  |     | 0       |                | 
    //+--------------+------------------+------+-----+---------+----------------+
    //mysql> select * from recipe;
    //+-----+-----------------------------+-------+----------------------------+------------------+----------+
    //| nid | source                      | yield | instructions               | notes            | preptime |
    //+-----+-----------------------------+-------+----------------------------+------------------+----------+
    //|   2 | This is an original recipe. |     5 | Instructions go here...... | Notes go here... |        5 | 
    //+-----+-----------------------------+-------+----------------------------+------------------+----------+
    //1 row in set (0.00 sec)
    
    // Define the base group of this table. Fields that don't
    // have a group defined will go into this field by default.
    $data['recipe']['table']['group'] = t('Recipe');

    // table recipe
    $data['recipe']['nid'] = array(
        'title' => t('Recipe NID'),
        'help' => t("The node id for this recipe."), // The help that appears on the UI,
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
            'name field' => 'title', // the field to display in the summary.
            'numeric' => TRUE,
            'validate type' => 'nid',
        ),
        // Information for accepting a nid as an argument
        'argument' => array(
            'handler' => 'views_handler_argument_node_nid',
        ),
        // Information for accepting a nid as a filter
        'filter' => array(
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );
    
    $data['recipe']['source'] = array(
        'title' => t('Source'),
        'help' => t("Optional. Does anyone else deserve credit for this recipe?"), // The help that appears on the UI,
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
        'help' => t("The number of servings for this recipe."), // The help that appears on the UI,
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
        'help' => t("The instructions for this recipe."), // The help that appears on the UI,
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
        'help' => t("The notes for this recipe."), // The help that appears on the UI,
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
        'help' => t("The preptime for this recipe."), // The help that appears on the UI,
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
    
    //    mysql> desc recipe_node_ingredient;
    //+---------------+------------------+------+-----+---------+----------------+
    //| Field         | Type             | Null | Key | Default | Extra          |
    //+---------------+------------------+------+-----+---------+----------------+
    //| id            | int(10) unsigned | NO   | PRI | NULL    | auto_increment | 
    //| nid           | int(10) unsigned | NO   |     | NULL    |                | 
    //| unit_id       | int(10) unsigned | NO   |     | NULL    |                | 
    //| quantity      | float            | YES  |     | NULL    |                | 
    //| ingredient_id | int(10) unsigned | NO   |     | NULL    |                | 
    //+---------------+------------------+------+-----+---------+----------------+
    //+----+-----+---------+----------+---------------+
    //| id | nid | unit_id | quantity | ingredient_id |
    //+----+-----+---------+----------+---------------+
    //|  1 |   2 |      15 |        1 |             1 | 
    //|  2 |   2 |      14 |        6 |             2 | 
    //|  3 |   2 |      18 |        5 |             3 | 
    //+----+-----+---------+----------+---------------+

    $data['recipe_node_ingredient']['table']['group'] = t('RecipeNodeIngredient');
    
    //not needed? raises join error if used
    $data['recipe_node_ingredient']['id'] = array(
        'title' => t('Ingredient ID (recipe_node_ingredient.id)'),
        'help' => t('The unique ID (recipe_node_ingredient.id) of the ingredient.'), // The help that appears on the UI,
        // Information for displaying the id
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );
    
    $data['recipe_node_ingredient']['nid'] = array(
        'title' => t('Ingredient NID'),
        'help' => t('The recipe NID using this ingredient.'), // The help that appears on the UI,
        // Information for displaying the id
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );
    
    $data['recipe_node_ingredient']['unit_id'] = array(
        'title' => t('Unit ID'),
        'help' => t('The measurement type this ingredient was measured with.'), // The help that appears on the UI,
        // Information for displaying the id
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );
    
    $data['recipe_node_ingredient']['quantity'] = array(
        'title' => t('Quantity'),
        'help' => t('The amount used of a given measurement.'), // The help that appears on the UI,
        // Information for displaying the id
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );
    
    // todo: this can sometimes cause an error if used as a Field
    $data['recipe_node_ingredient']['ingredient_id'] = array(
        'title' => t('Recipie node ing. id'),
        'help' => t('The particular ingredient id (recipe_node_ingredient.ingredient_id).'), // The help that appears on the UI,
        // Information for displaying the id
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
    );
    

    
    // table recipe_ingredient
    //    mysql> desc recipe_ingredient;
    //+-------+------------------+------+-----+---------+----------------+
    //| Field | Type             | Null | Key | Default | Extra          |
    //+-------+------------------+------+-----+---------+----------------+
    //| id    | int(10) unsigned | NO   | PRI | NULL    | auto_increment | 
    //| name  | varchar(255)     | YES  |     | NULL    |                | 
    //| link  | int(10) unsigned | NO   |     | NULL    |                | 
    //+-------+------------------+------+-----+---------+----------------+
    //3 rows in set (0.00 sec)
    
    $data['recipe_ingredient']['table']['group'] = t('RecipeIngredient');
    
    $data['recipe_ingredient']['id'] = array(
        'title' => t('Ingredient ID (recipe_ingredient.id)'),
        'help' => t('The id (recipe_ingredient.id) of the ingredient.'), // The help that appears on the UI,
        // Information for displaying the nid
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        // Information for accepting a nid as an argument
        'argument' => array(
            'handler' => 'views_handler_argument_node_nid',
            'name field' => 'name', // the field to display in the summary.
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
    
    $data['recipe_ingredient']['name'] = array(
        'title' => t('Ingredient Name'),
        'help' => t("An ingredient name."), // The help that appears on the UI,
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
    
    
    
    // mysql> desc recipe_unit;
    //+--------------+------------------+------+-----+---------+----------------+
    //| Field        | Type             | Null | Key | Default | Extra          |
    //+--------------+------------------+------+-----+---------+----------------+
    //| id           | int(10) unsigned | NO   | PRI | NULL    | auto_increment | 
    //| name         | varchar(255)     | YES  |     | NULL    |                | 
    //| abbreviation | varchar(8)       | YES  |     | NULL    |                | 
    //| metric       | int(10) unsigned | NO   |     | 0       |                | 
    //| type         | varchar(6)       | NO   |     | Mass    |                | 
    //+--------------+------------------+------+-----+---------+----------------+
    // mysql> select * from recipe_unit;
    //+----+---------------------+--------------+--------+--------+
    //| id | name                | abbreviation | metric | type   |
    //+----+---------------------+--------------+--------+--------+
    //|  1 | Slice               | sli          |      0 | Unit   | 
    //|  2 | Unit                |              |      0 | Unit   | 
    //|  3 | Clove               | clv          |      0 | Unit   | 
    //|  4 | Pinch               | pn           |      0 | Unit   | 
    //|  5 | Package             | pk           |      0 | Unit   | 
    //|  6 | Can                 | cn           |      0 | Unit   | 
    //|  7 | Drop                | dr           |      0 | Unit   | 
    //|  8 | Bunch               | bn           |      0 | Unit   | 
    //|  9 | Dash                | ds           |      0 | Unit   | 
    //| 10 | Carton              | ct           |      0 | Unit   | 
    //| 11 | Cup                 | c            |      0 | Unit   | 
    //| 12 | Tablespoon          | T            |      0 | Volume | 
    //| 13 | Teaspoon            | t            |      0 | Volume | 
    //| 14 | Pound               | lb           |      0 | Mass   | 
    //| 15 | Ounce               | oz           |      0 | Mass   | 
    //| 16 | Pint                | pt           |      0 | Volume | 
    //| 17 | Quart               | q            |      0 | Volume | 
    //| 18 | Gallon              | gal          |      0 | Volume | 
    //| 19 | Milligram           | mg           |      1 | Mass   | 
    //| 20 | Centigram           | cg           |      1 | Mass   | 
    //| 21 | Gram                | g            |      1 | Mass   | 
    //| 22 | Kilogram            | kg           |      1 | Mass   | 
    //| 23 | Millilitre          | ml           |      1 | Volume | 
    //| 24 | Centilitre          | cl           |      1 | Volume | 
    //| 25 | Litre               | l            |      1 | Volume | 
    //| 26 | Decilitre           | dl           |      1 | Volume | 
    //| 27 | Tablespoon (Metric) | tbsp         |      1 | Volume | 
    //| 28 | Teaspoon (Metric)   | tsp          |      1 | Volume | 
    //| 29 | Unknown             |              |      0 | Unit   | 
    //+----+---------------------+--------------+--------+--------+


    $data['recipe_unit']['table']['group'] = t('RecipeUnit');
    $data['recipe_unit']['id'] = array(
        'title' => t('Recipie unit ID (recipe_unit.id)'),
        'help' => t('The id (recipe_unit.id) of the recipe unit.'), // The help that appears on the UI,
        'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
        ),
        'argument' => array(
            'handler' => 'views_handler_argument_node_nid',
            'name field' => 'name', // the field to display in the summary.
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
        'help' => t("The recipe measurement unit name."), // The help that appears on the UI,
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
        'help' => t("The abbreviation for the recipe unit."), // The help that appears on the UI,
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
        'help' => t('Denotes if measurement unit is metric.'), // The help that appears on the UI,
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
        'help' => t("The unittype for this ingredient."), // The help that appears on the UI,
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