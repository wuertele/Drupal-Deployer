<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the xmlsitemap module.
 */

/**
 * Form builder; Administration settings form.
 *
 * @see system_settings_form()
 * @see xmlsitemap_settings_form_validate()
 */
function xmlsitemap_settings_form() {
  $form['xmlsitemap'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -10,
  );
  $form['xmlsitemap']['xmlsitemap_minimum_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Minimum sitemap lifetime'),
    '#options' => array(0 => t('No minimum')) + drupal_map_assoc(array(300, 900, 1800, 3600, 10800, 21600, 32400, 43200, 86400, 172800, 259200, 604800), 'format_interval'),
    '#default_value' => xmlsitemap_var('minimum_lifetime')
  );
  $form['xmlsitemap']['xmlsitemap_xsl'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include a stylesheet in the sitemaps.'),
    '#default_value' => xmlsitemap_var('xsl'),
    '#description' => t('Using the stylesheet will make it easier for non-computers to view and sort.')
  );
  $form['xmlsitemap']['xmlsitemap_chunk_size'] = array(
    '#type' => 'select',
    '#title' => t('Number of links in each sitemap page'),
    '#options' => array('auto' => t('Automatic')) + drupal_map_assoc(array(100, 500, 1000, 2500, 5000, 10000, 25000, 50000)),
    '#default_value' => xmlsitemap_var('chunk_size'),
    // @todo This description is not clear.
    '#description' => t('If there are problems with rebuilding the sitemap, you may want to manually set this value. If you have more than 50,000 links, an index with multiple sitemap pages will be generated. There is a maximum of 1000 sitemap pages.'),
  );
  $form['xmlsitemap']['xmlsitemap_batch_limit'] = array(
    '#type' => 'select',
    '#title' => t('Maximum number of sitemap links to process at once'),
    '#options' => drupal_map_assoc(array(5, 10, 25, 50, 100, 500, 1000, 2500)),
    '#default_value' => xmlsitemap_var('batch_limit'),
    '#description' => t('If you have problems running cron or rebuilding the sitemap, you may want to lower this value.'),
  );
  $form['xmlsitemap']['xmlsitemap_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Sitemap cache directory'),
    '#default_value' => xmlsitemap_var('path'),
    '#size' => 30,
    '#maxlength' => 255,
    '#description' => t('Subdirectory in the directory %dir where the sitemap data will be stored. This folder <strong>must not be shared</strong> with any other Drupal site or install using XML sitemap.', array('%dir' => file_directory_path() .'/')),
    '#required' => TRUE,
  );
  $form['xmlsitemap']['xmlsitemap_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#default_value' => xmlsitemap_var('base_url'),
    '#description' => t('This is the base URL for links generated in the sitemap.'),
    '#required' => TRUE,
  );
  // @todo Enable this feature when http://drupal.org/node/470602 is completed.
  $form['xmlsitemap']['xmlsitemap_languages'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Generate sitemaps for the following languages'),
    '#options' => function_exists('locale_language_list') ? locale_language_list() : array(language_default('language') => array(language_default('name'))),
    '#default_value' => xmlsitemap_var('languages'),
    //'#access' => module_exists('locale') && FALSE,
  );
  // Disable the default langage checkbox. It should always be enabled.
  $form['xmlsitemap']['xmlsitemap_languages'][language_default('language')] = array(
    '#type' => 'checkbox',
    '#title' => language_default('name'),
    '#return_value' => language_default('language'),
    '#default_value' => TRUE,
    '#disabled' => TRUE,
  );

  $form['frontpage'] = array(
    '#type' => 'fieldset',
    '#title' => t('Front page'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['frontpage']['xmlsitemap_frontpage_priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#options' => xmlsitemap_get_priority_options(),
    '#default_value' => xmlsitemap_var('frontpage_priority'),
  );
  $form['frontpage']['xmlsitemap_frontpage_changefreq'] = array(
    '#type' => 'select',
    '#title' => t('Change frequency'),
    '#options' => /*array(0 => t('Automatic')) +*/ xmlsitemap_get_changefreq_options(),
    '#default_value' => xmlsitemap_var('frontpage_changefreq'),
  );

  //$form['advanced'] = array(
  //  '#type' => 'fieldset',
  //  '#title' => t('Advanced'),
  //  '#collapsible' => TRUE,
  //  '#collapsed' => TRUE,
  //);

  $form['custom'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom links'),
    '#collapsible' => TRUE,
    '#collapsed' => !xmlsitemap_var('custom_links'),
  );
  $form['custom']['xmlsitemap_custom_links'] = array(
    '#type' => 'textarea',
    '#title' => t('Custom links to include in the sitemap'),
    '#default_value' => xmlsitemap_var('custom_links'),
    '#description' => t('Enter one path per line. Each path should be relative to the Drupal directory. For example: %example1.', array('%example1' => 'node/add')),
    '#wysiwyg' => FALSE,
    '#element_validate' => array('xmlsitemap_validate_custom_links'),
  );

  $form['#validate'][] = 'xmlsitemap_settings_form_validate';
  //$form['#submit'][] = 'xmlsitemap_form_submit_flag_regenerate';
  $form['#submit'][] = 'xmlsitemap_settings_form_submit';
  $form = system_settings_form($form);
  $form['buttons']['#weight'] = 10;
  $form['array_filter'] = array('#type' => 'value', '#value' => TRUE);
  return $form;
}

/**
 * Submit handler; Set the regenerate needed flag if variables have changed.
 *
 * This function needs to be called before system_settings_form_submit() or any
 * variable_set ()calls.
 */
function xmlsitemap_form_submit_flag_regenerate($form, $form_state) {
  foreach ($form_state['values'] as $variable => $value) {
    $stored_value = variable_get($variable, 'xmlsitemap_not_a_variable');
    if ($stored_value != 'xmlsitemap_not_a_variable' && $stored_value != $value) {
      variable_set('xmlsitemap_regenerate_needed', TRUE);
      return;
    }
  }
}

/**
 * Form validator; Check the sitemap files directory.
 *
 * @see xmlsitemap_settings_form()
 */
function xmlsitemap_settings_form_validate($form, &$form_state) {
  // Check if the xmlsitemap files directory has changed.
  $path_new = file_create_path($form_state['values']['xmlsitemap_path']);
  $path_old = file_create_path(xmlsitemap_var('path'));
  if ($path_new != $path_old && file_check_directory($path_new, FILE_CREATE_DIRECTORY, 'xmlsitemap_path')) {
    // If creating the new directory was successful, remove the old directory.
    xmlsitemap_clear_cache(TRUE, $path_old);
  }

  $base_url = &$form_state['values']['xmlsitemap_base_url'];
  $base_url = rtrim($base_url, '/');
  if ($base_url != '' && !valid_url($base_url, TRUE)) {
    form_set_error('xmlsitemap_base_url', t('Invalid base URL.'));
  }
}

/**
 * Element validator; Validates the custom links.
 */
function xmlsitemap_validate_custom_links($element, &$form_state) {
  $links = preg_split('/[\r\n]+/', $element['#value'], -1, PREG_SPLIT_NO_EMPTY);

  foreach ($links as $index => $link) {
    $normal_path = drupal_get_normal_path($link);
    if ($link != $normal_path) {
      drupal_set_message(t('The sitemap system stores system paths only, but will use the URL alias in the sitemap output. The custom link %link has been stored as %normal_path.', array('%link' => $link, '%normal_path' => $normal_path)));
      $links[$index] = $normal_path;
    }
  }
  $links = array_unique($links);

  // Save the normalized paths back into the form values for submission.
  $form_state['values']['xmlsitemap_custom_links'] = implode("\n", $links);

  // Test anonymous user access to the custom link paths.
  $menu_items = array();
  xmlsitemap_switch_user(0);
  foreach ($links as $link) {
    $menu_items[$link] = menu_get_item($link);
    // Since the menu item access results are cached, manually check the current path.
    if (strpos($link, 'admin/settings/xmlsitemap') === 0 && !user_access('administer xmlsitemap', drupal_anonymous_user())) {
      $menu_items[$link]['access'] = FALSE;
    }
  }
  xmlsitemap_restore_user();

  foreach ($links as $link) {
    if (xmlsitemap_load_link(array('loc' => $link, 'status' => TRUE))) {
      form_error($element, t('There is already an existing link in the sitemap with the path %link.', array('%link' => $link)));
      drupal_set_message(t('There is already an existing link in the sitemap with the path %link.', array('%link' => $link)), 'error', FALSE);
    }
    elseif (!$menu_items[$link] || !$menu_items[$link]['access']) {
      form_error($element, t('The custom link %link is either invalid or it cannot be accessed by anonymous users.', array('%link' => $link)));
      drupal_set_message(t('The custom link %link is either invalid or it cannot be accessed by anonymous users.', array('%link' => $link)), 'error', FALSE);
    }
  }
}

/**
 * Submit handler; save the custom links.
 *
 * @see xmlsitemap_settings_form()
 */
function xmlsitemap_settings_form_submit($form, $form_state) {
  db_query("DELETE FROM {xmlsitemap} WHERE type = 'custom'");
  $custom_links = preg_split('/[\r\n]+/', $form_state['values']['xmlsitemap_custom_links'], -1, PREG_SPLIT_NO_EMPTY);
  $id = 1;
  foreach ($custom_links as $custom_link) {
    xmlsitemap_save_link(array('type' => 'custom', 'id' => $id++, 'loc' => $custom_link));
  }
}

/**
 * Menu callback; Confirm rebuilding of the sitemap.
 *
 * @see xmlsitemap_rebuild_confirm_submit()
 */
function xmlsitemap_rebuild_confirm() {
  $modules = module_implements('xmlsitemap_links', TRUE);
  $form['modules'] = array(
    '#type' => 'select',
    '#title' => t('Select which modules\' links you would like to rebuild'),
    '#multiple' => TRUE,
    '#options' => drupal_map_assoc($modules),
    '#default_value' => $modules,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to rebuild the sitemap?'),
    'admin/settings/xmlsitemap',
    t("This action rebuilds your site's sitemap, and may be a lengthy process."),
    t('Rebuild sitemap'),
    t('Cancel')
  );
}

/**
 * Submit handler; Starts the sitemap rebuild batch.
 *
 * @see xmlsitemap_rebuild_confirm()
 * @see xmlsitemap_rebuild_batch()
 */
function xmlsitemap_rebuild_confirm_submit($form, &$form_state) {
  // Set the rebuild flag incase something fails during the rebuild.
  variable_set('xmlsitemap_rebuild_needed', TRUE);

  batch_set(xmlsitemap_rebuild_batch($form_state));
  $form_state['redirect'] = 'admin/settings/xmlsitemap';
}

/**
 * Batch information callback.
 */
function xmlsitemap_rebuild_batch($form_state) {
  $batch = array(
    'operations' => array(),
    'finished' => 'xmlsitemap_rebuild_batch_finished',
    'title' => t('Rebuilding Sitemap'),
    'file' => drupal_get_path('module', 'xmlsitemap') . '/xmlsitemap.admin.inc',
  );

  // Fetch all the sitemap links and save them to the {xmlsitemap} table.
  foreach ($form_state['values']['modules'] as $module) {
    $batch['operations'][] = array('xmlsitemap_rebuild_batch_clear', array($module));
    $batch['operations'][] = array('xmlsitemap_rebuild_batch_fetch', array($module));
  }

  // Generate all the sitemap pages.
  $batch['operations'][] = array('xmlsitemap_rebuild_batch_generate', array());
  //$languages = languages_list();
  //foreach (xmlsitemap_var('languages') as $language) {
  //  $batch['operations'][] = array('xmlsitemap_rebuild_batch_generate', array($languages[$language]));
  //}

  return $batch;
}

function xmlsitemap_rebuild_batch_clear($module, &$context) {
  module_invoke($module, 'xmlsitemap_links_clear');
  $context['message'] = t('Now clearing %module links.', array('%module' => $module));
}

function xmlsitemap_rebuild_batch_fetch($module, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['batch'] = module_hook($module, 'xmlsitemap_links_batch_info');
    if ($context['sandbox']['batch']) {
      $context['sandbox'] += module_invoke($module, 'xmlsitemap_links_batch_info');
    }
    else {
      $context['sandbox']['links'] = module_invoke($module, 'xmlsitemap_links');
      $context['sandbox']['max'] = count($context['sandbox']['links']);
    }
    $context['sandbox'] += array(
      'progress' => 0,
      'current' => 0,
    );
  }

  if ($context['sandbox']['batch']) {
    $links = module_invoke($module, 'xmlsitemap_links', $context['sandbox']['current'], xmlsitemap_var('batch_limit'));
  }
  else {
    $links = array_splice($context['sandbox']['links'], 0, xmlsitemap_var('batch_limit'));
  }

  foreach ($links as $link) {
    xmlsitemap_save_link($link);
    $context['sandbox']['progress']++;
    $context['sandbox']['current'] = $link['id'];
    $context['message'] = t('Now processing %module link @count.', array('%module' => $module, '@count' => $context['sandbox']['progress']));
  }

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch callback; generate all the sitemap chunks.
 *
 * @todo Split generation for each language?
 */
function xmlsitemap_rebuild_batch_generate(&$context) {
  if (!isset($context['sandbox']['progress'])) {
    _xmlsitemap_regenerate_before();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['languages'] = xmlsitemap_var('languages');
    $context['sandbox']['chunk_count'] = xmlsitemap_get_chunk_count(TRUE);
    // If we have more than one chunk, we need to increment this value by one
    // since the index page will also need to be generated.
    if ($context['sandbox']['chunk_count'] > 1) {
      $context['sandbox']['chunk_count']++;
    }
    $context['sandbox']['max'] = count($context['sandbox']['languages']) * $context['sandbox']['chunk_count'];
  }

  $languages = language_list();
  $language = $languages[$context['sandbox']['languages'][0]];
  $chunk = $context['sandbox']['progress'] % $context['sandbox']['chunk_count'];
  xmlsitemap_generate($chunk, $language);
  $context['sandbox']['progress']++;
  $context['message'] = t('Now generating @language sitemap page @chunk.', array('@language' => $language->name, '@chunk' => $chunk + 1));

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
    if ($context['sandbox']['progress'] % $context['sandbox']['chunk_count'] == 0) {
      // If we have generated all the chunks of a language's sitemap, proceed
      // to the next language.
      array_shift($context['sandbox']['languages']);
    }
  }
  else {
    _xmlsitemap_regenerate_after();
  }
}

/**
 * Batch callback; sitemap rebuild finished.
 */
function xmlsitemap_rebuild_batch_finished($success, $results, $operations) {
  if ($success) {
    // Reset the rebuild flag since it was successful.
    variable_set('xmlsitemap_rebuild_needed', FALSE);
    drupal_set_message(t('The <a href="@sitemap">sitemap</a> was rebuilt.', array('@sitemap' => url('sitemap.xml'))));
  }
  else {
    drupal_set_message(t('The <a href="@sitemap">sitemap</a> was not successfully rebuilt.', array('@sitemap' => url('sitemap.xml'))), 'error');
  }
}
