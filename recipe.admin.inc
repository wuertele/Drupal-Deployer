<?php
// $Id$


/**
 * @file
 * recipe.admin.inc -contains all admin pages, settings, and validate.
 *
 * This file will only be parsed when processing admin menu options.
 * See http://drupal.org/node/146172
 */


/**
 * Settings form for menu callback
 */
function recipe_admin_settings() {
  $form['recipe_help'] = array(
    '#type' => 'textarea',
    '#title' => t('Explanation or submission guidelines'),
    '#default_value' => variable_get('recipe_help', ''),
    '#cols' => 55,
    '#rows' => 4,
    '#description' => t('This text will be displayed at the top of the recipe submission form. Useful for helping or instructing your users.'),
  );

  $form['recipe_fraction_display'] = array(
    '#type' => 'textfield',
    '#title' => t('Fractions display string'),
    '#default_value' => variable_get('recipe_fraction_display', t('{%d }%d&frasl;%d')),
    '#size' => 35,
    '#maxlength' => 255,
    '#description' => t('How fractions should be displayed. Leave blank to display as decimals. Each incidence of %d will be replaced by the whole number, the numerator, and the denominator in that order. Anything between curly braces will not be displayed when the whole number is equal to 0. Recommended settings are "{%d }%d&amp;frasl;%d" or "{%d }&lt;sup&gt;%d&lt;/sup&gt;/&lt;sub&gt;%d&lt;/sub&gt;"'),
  );

  $form['recipe_unit_display'] = array(
    '#type' => 'radios',
    '#title' => t('Ingredient unit display'),
    '#default_value' => variable_get('recipe_unit_display', 0),
    '#options' => array(t('Abbreviation'), t('Full name')),
    '#description' => t('Display ingredient units like Tbsp or Tablespoon.'),
    '#required' => TRUE,
  );

  $form['recipe_preferred_system_of_measure'] = array(
    '#type' => 'radios',
    '#title' => t('Preferred system of measure'),
    '#default_value' => variable_get('recipe_preferred_system_of_measure', 0),
    '#options' => array(t('U.S. customary units'), t('Metric')),
    '#description' => t('Which system of measure should be preferred where it is ambiguous.'),
    '#required' => TRUE,
  );

  $form['recipe_summary_box'] = array(
    '#type' => 'fieldset',
    '#title' => t('Recipe summary box'),
  );
  $form['recipe_summary_box']['recipe_summary_box_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show recipe summary box'),
    '#return_value' => 1,
    '#default_value' => variable_get('recipe_summary_box_enable', 1),
    '#description' => t('Show the recipe summary box on recipe view pages.  Contains the yield, source, and prep time values.'),
    '#required' => FALSE,
  );
  $form['recipe_summary_box']['recipe_summary_box_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Box title'),
    '#default_value' => variable_get('recipe_summary_box_title', t('Summary')),
    '#size' => 35,
    '#maxlength' => 255,
    '#description' => t('Title of the recipe summary box on recipe view pages.'),
  );


  $form['recipe_recent_box'] = array(
    '#type' => 'fieldset',
    '#title' => t('Recent recipe box'),
  );
  $form['recipe_recent_box']['recipe_recent_box_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show recent recipes box'),
    '#return_value' => 1,
    '#default_value' => variable_get('recipe_recent_box_enable', 1),
    '#description' => t('Show the recent recipes box on the recipes menu page.'),
    '#required' => FALSE,
  );
  $form['recipe_recent_box']['recipe_recent_box_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Box title'),
    '#default_value' => variable_get('recipe_recent_box_title', t('Latest recipes')),
    '#size' => 35,
    '#maxlength' => 255,
    '#description' => t('Title of the recent recipes box on the recipes index page.'),
  );
  $form['recipe_recent_box']['recipe_recent_display'] = array(
    '#type' => 'select',
    '#title' => t('Recipes to display'),
    '#default_value' => variable_get('recipe_recent_display', 5),
    '#options' => drupal_map_assoc(array(0, 5, 10, 15)),
    '#description' => t('Sets the number of recent recipes that will be displayed in the Recent Recipes box. (0 = not displayed).'),
  );

  return system_settings_form($form);
}


/**
 * Implementation of hook_settings()
 */
function recipe_index_settings_form() {
  $form = array();
  if (!module_exists('recipe')) {
    drupal_set_message(t('The &quot;recipe&quot; module is disabled or not installed. Recipes will not function until this is corrected. Check the availability of that module, and enable if needed, in the %modules.', array('%modules' => l(t('modules administration page'), 'admin/modules'))), 'error');
  }

  $form['recipe_bycat'] = array(
    '#type' => 'fieldset',
    '#title' => t('Category index settings'),
    '#description' => t('If you don\'t want the menu items, you can go to admin/build/menu and disable them.'),
  );
  $form['recipe_bycat']['recipe_index_depth'] = array(
    '#type' => 'select',
    '#title' => t('Term depth'),
    '#default_value' => variable_get('recipe_index_depth', 4),
    '#options' => array(1 => "1", 2 => "2", 3 => "3", 4 => "4", 5 => "5", 6 => "6", 1000 => t('All Levels')),
    '#description' => t('Defines how many levels of terms should be displayed on a category index page.  Applies to hierarchical vocabularies.'),
  );
  $form['recipe_bycat']['recipe_index_catalog_show_child_category_links'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show recipes in child categories'),
    '#return_value' => 1,
    '#default_value' => variable_get('recipe_index_catalog_show_child_category_links', 1),
    '#description' => t('List recipes in the current category and its children down to the selected Term Depth(above).'),
  );

  // The setting above affect the menu system.
  menu_rebuild();

  return system_settings_form($form);
}


function recipe_io_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);
  return theme('node_add_list', $content);
}


function recipe_export_multi($type = NULL) {

  // load supported formats
  $formats = module_invoke_all('recipeio', 'export_multi');
  $o = 'Supported Output Formats:<br/>';
  if ( $type === NULL ) {
    foreach ( $formats as $key => $format ) {
      if ($format) {
        $o .= l($format['format_name'], "admin/content/recipe/export_multi/$key") . "<br/>";
      }
    }
    return $o;
  }

  // normalize typed urls
  $type = drupal_strtolower($type);

  // If callback exists, call it, otherwise error out.
  if (isset($formats[$type]) && function_exists($formats[$type]['callback'])) {
    echo call_user_func($formats[$type]['callback']);
  }
  else {
    drupal_set_message(t('Unknown export format(%the_format).', array('%the_format' => $type)), 'error');
    drupal_not_found();
  }
}

function recipe_import_multi($type = NULL) {

  // load supported formats
  $formats = module_invoke_all('recipeio', 'import_multi');
  $o = 'Supported Input Formats:<br/>';
  if ( $type === NULL ) {
    foreach ( $formats as $key => $format ) {
      if ($format) {
        $o .= l($format['format_name'], "admin/content/recipe/import_multi/$key") . "<br/>";
      }
    }
    return $o;
  }

  // normalize typed urls
  $type = drupal_strtolower($type);

  // If callback exists, call it, otherwise error out.
  if (isset($formats[$type]) && function_exists($formats[$type]['callback'])) {
    echo call_user_func($formats[$type]['callback']);
  }
  else {
    drupal_set_message(t('Unknown export format(%the_format).', array('%the_format' => $type)), 'error');
    drupal_not_found();
  }
}

