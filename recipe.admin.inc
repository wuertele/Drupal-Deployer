<?php
// $Id$


/**
 * @file
 * recipe.admin.inc - admin menu hook 
 */


/**
 * Settings form for menu callback
 */
function recipe_admin_settings() {
  $form['recipe_index_depth'] = array(
    '#type' => 'select',
    '#title' => t('Index depth'),
    '#default_value' => variable_get('recipe_index_depth', 0),
    '#options' => array(0 => t('All Terms'), 1 => "1", 2 => "2", 3 => "3", 4 => "4", 5 => "5", 6 => "6"),
    '#description' => t('Defines how many levels of terms should be displayed on any given recipe index page. For example, if you select 1 then only one level of the recipe index tree will be displayed at a time.'),
  );
  $form['recipe_recent_box_enable'] = array(
    '#type' => 'radios',
    '#title' => t('Recent recipes box'),
    '#default_value' => variable_get('recipe_recent_box_enable', 1),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('Enables or disables the recent recipes box on the recipes index page.'),
    '#required' => FALSE,
  );
  $form['recipe_recent_box_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Box title'),
    '#default_value' => variable_get('recipe_recent_box_title', t('Latest recipes')),
    '#size' => 35,
    '#maxlength' => 255,
    '#description' => t('Title of the recent recipes box on the recipes index page.'),
  );
  $form['recipe_recent_display'] = array(
    '#type' => 'select',
    '#title' => t('Recipes to display'),
    '#default_value' => variable_get('recipe_recent_display', 5),
    '#options' => drupal_map_assoc(array(0, 5, 10, 15)),
    '#description' => t('Sets the number of recent recipes that will be displayed in the Recent Recipes box. (0 = not displayed).'),
  );
  $form['recipe_help'] = array(
    '#type' => 'textarea',
    '#title' => t('Explanation or submission guidelines'),
    '#default_value' => variable_get('recipe_help', ''),
    '#cols' => 55,
    '#rows' => 4,
    '#description' => t('This text will be displayed at the top of the recipe submission form. Useful for helping or instructing your users.'),
  );

  $form['recipe_ingredient_system'] = array(
    '#type' => 'radios',
    '#title' => t('Ingredient entering system'),
    '#default_value' => variable_get('recipe_ingredient_system', 'complex'),
    '#options' => array(
      'simple' => t('Simple'),
      'complex' => t('Complex'),
    ),
    '#description' => t('The simple ingredient system allows all ingredients to be entered on one line. The complex system forces the user to separate the quantity and units from the ingredient.'),
  );
  $form['recipe_fraction_display'] = array(
    '#type' => 'textfield',
    '#title' => t('Fractions display string'),
    '#default_value' => variable_get('recipe_fraction_display', t('{%d }%d&frasl;%d')),
    '#size' => 35,
    '#maxlength' => 255,
    '#description' => t('How fractions should be displayed. Leave blank to display as decimals. Each incidence of %d will be replaced by the whole number, the numerator, and the denominator in that order. Anything between curly braces will not be displayed when the whole number is equal to 0. Recommended settings are "{%d }%d&amp;frasl;%d" or "{%d }&lt;sup&gt;%d&lt;/sup&gt;/&lt;sub&gt;%d&lt;/sub&gt;"'),
  );
  $form['recipe_export_html_enable'] = array(
    '#type' => 'radios',
    '#title' => t('Export to HTML'),
    '#default_value' => variable_get('recipe_export_html_enable', 1),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('Enables or disables the Export as HTML link.'),
    '#required' => FALSE,
  );
  $form['recipe_export_recipeml_enable'] = array(
    '#type' => 'radios',
    '#title' => t('Export to RecipeML'),
    '#default_value' => variable_get('recipe_export_recipeml_enable', 1),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('Enables or disables the Export as RecipeML link.'),
    '#required' => FALSE,
  );
  $form['recipe_hide_summary'] = array(
    '#type' => 'radios',
    '#title' => t('Hide Summary'),
    '#default_value' => variable_get('recipe_hide_summary', 0),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('Hides or shows the Summary/Yield form.'),
    '#required' => FALSE,
  );
  return system_settings_form($form);
}
