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
  $form['xmlsitemap']['xmlsitemap_batch_limit'] = array(
    '#type' => 'select',
    '#title' => t('Maximum number of sitemap links to process at once'),
    '#options' => drupal_map_assoc(array(10, 25, 50, 100, 500)),
    '#default_value' => xmlsitemap_var('batch_limit'),
    '#description' => t('If you have problems running cron or rebuilding the sitemap, you may want to lower this value.'),
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
    '#options' => array('auto' => t('Automatic')) + drupal_map_assoc(array(500, 1000, 2500, 5000, 10000, 25000, 50000)),
    '#default_value' => xmlsitemap_var('chunk_size'),
    // @todo This description is not clear.
    '#description' => t('If there are problems with rebuilding the sitemap, you may want to manually set this value. If you have more than 50,000 links, an index with multiple sitemap pages will be generated. There is a maximum of 1000 sitemap pages.'),
  );
  $form['xmlsitemap']['xmlsitemap_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Sitemap file cache directory'),
    '#default_value' => xmlsitemap_var('path'),
    '#size' => 30,
    '#maxlength' => 255,
    '#description' => t('Subdirectory in the directory %dir where the sitemap data will be stored.', array('%dir' => file_directory_path() .'/')),
  );

  $form['frontpage'] = array(
    '#type' => 'fieldset',
    '#title' => t('Front page'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['frontpage']['xmlsitemap_frontpage_priority'] = array(
    '#type' => 'select',
    '#title' => t('Frontpage priority'),
    '#options' => xmlsitemap_get_priority_options(),
    '#default_value' => xmlsitemap_var('frontpage_priority'),
  );
  $form['frontpage']['xmlsitemap_frontpage_changefreq'] = array(
    '#type' => 'select',
    '#title' => t('Frontpage change frequency'),
    '#options' => /*array(0 => t('Automatic')) +*/ xmlsitemap_get_changefreq_options(),
    '#default_value' => xmlsitemap_var('frontpage_changefreq'),
  );

  $form['custom'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom links'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['custom']['xmlsitemap_custom_links'] = array(
    '#type' => 'textarea',
    '#title' => t('Custom links to include in the sitemap'),
    '#default_value' => xmlsitemap_var('custom_links'),
    '#description' => t('Enter one path per line. Each path should be relative to the Drupal directory. For example: %example1.', array('%example1' => 'node/add')),
    '#wysiwyg' => FALSE,
  );

  $form['#validate'][] = 'xmlsitemap_settings_form_validate';
  $form = system_settings_form($form);
  $form['buttons']['#weight'] = 10;
  return $form;
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
    xmlsitemap_clean_directory($path_old, TRUE);
  }

  // Validate the custom links.
  db_query("DELETE FROM {xmlsitemap} WHERE type = 'custom'");
  $custom_links = preg_split('/[\r\n]+/', $form_state['values']['xmlsitemap_custom_links'], -1, PREG_SPLIT_NO_EMPTY);
  foreach ($custom_links as $index => $custom_link) {
    $normal_path = trim(drupal_get_normal_path($custom_link));
    if ($custom_link != $normal_path) {
      drupal_set_message(t('The sitemap system stores system paths only, but will use the URL alias in the sitemap output. The custom link %custom_link has been stored as %normal_path.', array('%custom_link' => $custom_link, '%normal_path' => $normal_path)));
      $custom_links[$index] = $normal_path;
    }

    // Test that the anonymous users can access the path.
    xmlsitemap_switch_user(0);
    $menu_item = menu_get_item($normal_path);
    xmlsitemap_restore_user();
    if (!$menu_item || !$menu_item['access']) {
      form_set_error('xmlsitemap_custom_links', t('The custom link %link is either invalid or it cannot be accessed by anonymous users.', array('%link' => $normal_path)));
    }
    elseif (xmlsitemap_load_link(array('loc' => $normal_path))) {
      // Check if there is a sitemap link with the same location already.
      form_set_error('xmlsitemap_custom_links', t('There is already an existing link in the sitemap with the path %link.', array('%link' => $normal_path)));
    }
    else {
      $customid = 1;
      xmlsitemap_save_link(array('type' => 'custom', 'id' => $customid++, 'loc' => $normal_path));
    }
  }
  $form_state['values']['xmlsitemap_custom_links'] = implode("\n", $custom_links);
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
  _xmlsitemap_rebuild_before();
  batch_set(xmlsitemap_rebuild_batch($form_state));
  $form_state['redirect'] = 'admin/settings/xmlsitemap';
}

/**
 * Perform operations before rebuilding the sitemap.
 */
function _xmlsitemap_rebuild_before() {
  // Increase the processing time since this might take a little bit.
  @set_time_limit(240);

  // Delete all sitemap data from the sitemap files directory.
  $path = file_create_path(xmlsitemap_var('path'));
  xmlsitemap_clean_directory($path, FALSE);
  //file_check_directory($path, FILE_CREATE_DIRECTORY);

  // Set the the sitemap will have to be rebuilt.
  variable_set('xmlsitemap_rebuild_needed', TRUE);

  // Save custom data into a temporary table so it can be re-loaded later.
  //db_query_temporary("SELECT type, id, priority, changefreq, changecount FROM {xmlsitemap}", 'xmlsitemap_temp');
}

/**
 * Perform operations after rebuilding the sitemap.
 */
function _xmlsitemap_rebuild_after() {
  // Reset the needs rebuild and regenerate flags.
  variable_set('xmlsitemap_rebuild_needed', FALSE);
  variable_set('xmlsitemap_regenerate_needed', FALSE);

  // If the chunk count has changed, we will need to rebuild the menu.
  variable_set('menu_rebuild_needed', TRUE);
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
  //foreach (module_implements('xmlsitemap_links') as $module) {
  foreach ($form_state['values']['modules'] as $module) {
    $batch['operations'][] = array('xmlsitemap_rebuild_batch_clear', array($module));
    if (module_hook($module, 'xmlsitemap_links_batch')) {
      $batch['operations'][] = array($module .'_xmlsitemap_links_batch', array());
    }
    else {
      $batch['operations'][] = array('xmlsitemap_rebuild_batch_fetch', array($module));
    }
  }

  // Generate all the sitemap pages.
  $batch['operations'][] = array('xmlsitemap_rebuild_batch_generate', array());

  return $batch;
}

function xmlsitemap_rebuild_batch_clear($module, &$context) {
  module_invoke($module, 'xmlsitemap_links_clear');
  $context['message'] = t('Now clearing %module links.', array('%module' => $module));
}

function xmlsitemap_rebuild_batch_fetch($module, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['links'] = module_invoke($module, 'xmlsitemap_links');
    $context['sandbox']['max'] = count($context['sandbox']['links']);
  }

  $links = array_splice($context['sandbox']['links'], 0, xmlsitemap_var('batch_limit'));
  foreach ($links as $link) {
    xmlsitemap_save_link($link);
    $context['sandbox']['progress']++;
  }

  //$context['results'][] = $module;
  $context['message'] = t('Now processing %module link @count.', array('%module' => $module, '@count' => $context['sandbox']['progress']));

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch callback; generate one sitemap page.
 *
 * @todo Fix the max/progress adjustments.
 */
function xmlsitemap_rebuild_batch_generate(&$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = xmlsitemap_get_chunk_count(TRUE) + 1;
  }

  $chunk = &$context['sandbox']['progress'];
  xmlsitemap_generate($chunk);
  $chunk++;
  $context['message'] = t('Now generating sitemap page @chunk.', array('@chunk' => $chunk));

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch callback; sitemap rebuild finished.
 */
function xmlsitemap_rebuild_batch_finished($success, $results, $operations) {
  if ($success) {
    _xmlsitemap_rebuild_after();
    drupal_set_message(t('The <a href="@sitemap">sitemap</a> was rebuilt.', array('@sitemap' => url('sitemap.xml'))));
  }
  else {
    drupal_set_message(t('The <a href="@sitemap">sitemap</a> was not successfully rebuilt.', array('@sitemap' => url('sitemap.xml'))), 'error');
  }
}
