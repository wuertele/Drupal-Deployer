<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the xmlsitemap module.
 *
 * @ingroup xmlsitemap
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
  );
  $form['xmlsitemap']['xmlsitemap_minimum_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Minimum sitemap lifetime'),
    '#options' => array(0 => t('No minimum')) + drupal_map_assoc(array(300, 900, 1800, 3600, 10800, 21600, 32400, 43200, 86400, 172800, 259200, 604800), 'format_interval'),
    '#default_value' => xmlsitemap_var('minimum_lifetime')
  );
  $form['xmlsitemap']['xmlsitemap_xsl'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include a stylesheet in the sitemaps for humans.'),
    '#default_value' => xmlsitemap_var('xsl'),
    '#description' => t('Using the stylesheet will add formatting and tables with sorting to make it easier to view the XML sitemap data instead of viewing raw XML output. Search engines will ignore any formatting.')
  );
  $form['xmlsitemap']['xmlsitemap_languages'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Generate sitemaps for the following languages'),
    '#options' => array(language_default('language') => language_default('name')),
    '#default_value' => xmlsitemap_var('languages'),
    '#process' => array('expand_checkboxes', '_xmlsitemap_process_language_checkboxes'),
    '#description' => !module_exists('xmlsitemap_i18n') ? t('To enable multilingual features, enable the XML sitemap internationalization module.') : '',
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => !variable_get('xmlsitemap_developer_mode', FALSE),
    '#weight' => 10,
  );
  //$form['advanced']['xmlsitemap_gz'] = array(
  //  '#type' => 'checkbox',
  //  '#title' => t('Generate additional compressed sitemaps using gzip.'),
  //  '#default_value' => xmlsitemap_var('gz'),
  //  '#disabled' => !function_exists('gzencode'),
  //);
  $form['advanced']['xmlsitemap_chunk_size'] = array(
    '#type' => 'select',
    '#title' => t('Number of links in each sitemap page'),
    '#options' => array('auto' => t('Automatic (recommended)')) + drupal_map_assoc(array(100, 500, 1000, 2500, 5000, 10000, 25000, 50000)),
    '#default_value' => xmlsitemap_var('chunk_size'),
    // @todo This description is not clear.
    '#description' => t('If there are problems with rebuilding the sitemap, you may want to manually set this value. If you have more than 50,000 links, an index with multiple sitemap pages will be generated. There is a maximum of 1000 sitemap pages.'),
  );
  $form['advanced']['xmlsitemap_batch_limit'] = array(
    '#type' => 'select',
    '#title' => t('Maximum number of sitemap links to process at once'),
    '#options' => drupal_map_assoc(array(5, 10, 25, 50, 100, 250, 500, 1000, 2500, 5000)),
    '#default_value' => xmlsitemap_var('batch_limit'),
    '#description' => t('If you have problems running cron or rebuilding the sitemap, you may want to lower this value.'),
  );
  file_check_directory(xmlsitemap_get_directory(), FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS, 'xmlsitemap_path');
  $form['advanced']['xmlsitemap_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Sitemap cache directory'),
    '#default_value' => variable_get('xmlsitemap_path', 'xmlsitemap'),
    '#size' => 30,
    '#maxlength' => 255,
    '#description' => t('Subdirectory where the sitemap data will be stored. This folder <strong>must not be shared</strong> with any other Drupal site or install using XML sitemap.'),
    '#field_prefix' =>  file_directory_path() . '/',
    '#required' => TRUE,
  );
  $form['advanced']['xmlsitemap_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#default_value' => xmlsitemap_var('base_url'),
    '#size' => 30,
    '#description' => t('This is the base URL for links generated in the sitemap.'),
    '#required' => TRUE,
  );
  $form['advanced']['xmlsitemap_developer_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable developer mode.'),
    '#default_value' => variable_get('xmlsitemap_developer_mode', FALSE),
    '#description' => t('Exposes additional settings intended for development.'),
  );

  $form['frontpage'] = array(
    '#type' => 'fieldset',
    '#title' => t('Front page'),
    '#description' => t('The front page path can be changed at <a href="@url-frontpage">@url-frontpage</a>.', array('@url-frontpage' => url('admin/settings/site-information'))),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 20,
    '#group' => 'xmlsitemap_settings',
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
    '#options' => xmlsitemap_get_changefreq_options(),
    '#default_value' => xmlsitemap_var('frontpage_changefreq'),
  );

  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';
  $form['#validate'][] = 'xmlsitemap_settings_form_validate';
  $form['#submit'][] = 'xmlsitemap_settings_form_submit';
  $form['array_filter'] = array('#type' => 'value', '#value' => TRUE);

  $form = system_settings_form($form);
  $form['buttons']['#weight'] = 100;
  return $form;
}

/**
 * Show a link to each languages' sitemap and disable the default language
 * checkbox.
 */
function _xmlsitemap_process_language_checkboxes($element) {
  foreach (element_children($element) as $key) {
    if ($key == language_default('language')) {
      $element[$key]['#disabled'] = TRUE;
      $element[$key]['#default_value'] = TRUE;
      $element[$key]['#weight'] = -1;
    }
    $link = url('sitemap.xml', array('absolute' => TRUE, 'language' => xmlsitemap_language_load($key)));
    $element[$key]['#description'] = l($link, $link);
  }
  return $element;
}

/**
 * Form validator; Check the sitemap files directory.
 *
 * @see xmlsitemap_settings_form()
 */
function xmlsitemap_settings_form_validate($form, &$form_state) {
  // Check that the chunk size will not create more than 1000 chunks.
  $chunk_size = $form_state['values']['xmlsitemap_chunk_size'];
  if ($chunk_size != 'auto' && $chunk_size != 50000 && (xmlsitemap_get_link_count() / $chunk_size) > 1000) {
    form_set_error('xmlsitemap_chunk_size', t('The sitemap page link count of @size will create more than 1,000 sitemap pages. Please increase the link count.', array('@size' => $chunk_size)));
  }

  $base_url = &$form_state['values']['xmlsitemap_base_url'];
  $base_url = rtrim($base_url, '/');
  if ($base_url != '' && !valid_url($base_url, TRUE)) {
    form_set_error('xmlsitemap_base_url', t('Invalid base URL.'));
  }
}

/**
 * Submit handler;
 *
 * @see xmlsitemap_settings_form()
 */
function xmlsitemap_settings_form_submit($form, $form_state) {
  // Save any changes to the frontpage link.
  xmlsitemap_save_link(array('type' => 'frontpage', 'id' => 0, 'loc' => ''));
}

/**
 * Menu callback; Confirm rebuilding of the sitemap.
 *
 * @see xmlsitemap_rebuild_form_submit()
 */
function xmlsitemap_rebuild_form() {
  if (!$_POST && !xmlsitemap_var('rebuild_needed')) {
    if (!xmlsitemap_var('regenerate_needed')) {
      drupal_set_message(t('Your sitemap is up to date and does not need to be rebuilt.'), 'error');
    }
    else {
      $_REQUEST += array('destination' => 'admin/settings/xmlsitemap');
      drupal_set_message(t('A rebuild is not necessary. If you are just wanting to regenerate the XML sitemap files, you can <a href="@link-cron">run cron manually</a>.', array('@link-cron' => url('admin/reports/status/run-cron', array('query' => drupal_get_destination())))), 'warning');
    }
  }

  // Show only the modules that implement the 6.x-2.x hooks.
  $modules = module_implements('xmlsitemap_link_info', TRUE);

  $form['modules'] = array(
    '#type' => 'select',
    '#title' => t("Select which modules' links you would like to rebuild"),
    '#description' => t('If no modules are selected, the sitemap files will just be regenerated.'),
    '#multiple' => TRUE,
    '#options' => drupal_map_assoc($modules),
    '#default_value' => xmlsitemap_var('rebuild_needed') || !xmlsitemap_var('developer_mode') ? $modules : array(),
    '#access' => xmlsitemap_var('developer_mode'),
  );
  $form['save_custom'] = array(
    '#type' => 'checkbox',
    '#title' => t('Save and restore any custom inclusion and priority links.'),
    '#default_value' => TRUE,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to rebuild the sitemap?'),
    'admin/settings/xmlsitemap',
    '',
    t('Rebuild sitemap'),
    t('Cancel')
  );
}

/**
 * Submit handler; Starts the sitemap rebuild batch.
 *
 * @see xmlsitemap_rebuild_form()
 * @see xmlsitemap_rebuild_batch()
 */
function xmlsitemap_rebuild_form_submit($form, &$form_state) {
  // Set the rebuild flag incase something fails during the rebuild.
  variable_set('xmlsitemap_rebuild_needed', TRUE);

  module_load_include('inc', 'xmlsitemap');
  $batch = xmlsitemap_rebuild_batch($form_state['values']['modules'], $form_state['values']['save_custom']);
  batch_set($batch);
  $form_state['redirect'] = 'admin/settings/xmlsitemap';
}

/**
 * Create a list of items that can be included in the sitemap.
 *
 * @param $form
 *   An array with the form to add the type summary.
 * @param $type
 *   An array of details about the type with the following key/value pairs:
 *     type: The {xmlsitemap}.type value
 *     title: The title of the type (usually plural form)
 *     item_title: The title of each individual item of this type (singular)
 *     access: An optional boolean that if TRUE will create links to each
 *             item's edit page as specified by the link key in $items.
 * @param $items
 *   An array of items with the following keys:
 *     'name': The translated name of the item.
 *     'link': The path to edit the item. If $type['access'] is not TRUE, just
               the item's name will be shown instead of a link.
 *     'status': The item's default inclusion status.
 *     'priority': The items default priority.
 */
function xmlsitemap_add_form_type_summary(&$form, $type, $items) {
  $form[$type['type']] = array(
    '#type' => 'fieldset',
    '#title' => $type['title'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'xmlsitemap_settings',
  );

  $header = array(
    $type['item_title'],
    t('Inclusion'),
    t('Priority'),
  );
  $priorities = xmlsitemap_get_priority_options(NULL, FALSE);
  $statuses = xmlsitemap_get_status_options(NULL);

  $rows = array();
  foreach ($items as $item_type => $item) {
    $rows[] = array(
      !empty($type['access']) ? l($item['name'], $item['link'], array('query' => drupal_get_destination())) : check_plain($item['name']),
      $statuses[$item['status'] ? 1 : 0],
      $priorities[number_format($item['priority'], 1)],
    );
  }

  $form[$type['type']]['list'] = array(
    '#value' => theme('table', $header, $rows),
  );
}

/**
 * Add the link type XML sitemap options to the link type's form.
 *
 * @todo Abstract the submission and link updating for this submit?
 */
function xmlsitemap_add_form_type_options(&$form, $module, $options) {
  $form['xmlsitemap'] = array(
    '#type' => 'fieldset',
    '#title' => t('XML sitemap'),
    '#description' => t('Changing these type settings will affect any items of this type that have either inclusion or priority set to default.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#access' => user_access('administer xmlsitemap'),
    '#group' => 'additional_settings',
  );
  $form['xmlsitemap']['xmlsitemap_' . $module . '_status'] = array(
    '#type' => 'select',
    '#title' => t('Inclusion'),
    '#options' => xmlsitemap_get_status_options(),
    '#default_value' => $options['status'],
  );
  $form['xmlsitemap']['xmlsitemap_' . $module . '_priority'] = array(
    '#type' => 'select',
    '#title' => t('Default priority'),
    '#options' => xmlsitemap_get_priority_options(),
    '#default_value' => $options['priority'],
  );

  // Hide the priority field if the link is excluded.
  if (module_exists('ctools')) {
    ctools_include('dependent');
    $form['xmlsitemap']['xmlsitemap_' . $module . '_priority'] += array(
      '#process' => array('form_expand_ahah', 'ctools_dependent_process'),
      '#dependency' => array('edit-xmlsitemap-' . $module . '-status' => array(1)),
    );
  }
}

/**
 * Add a link's XML sitemap options to the link's form.
 */
function xmlsitemap_add_form_link_options(&$form, $link) {
  $form['xmlsitemap'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('XML sitemap'),
    '#collapsible' => TRUE,
    '#collapsed' => !$link['status_override'] && !$link['priority_override'],
    '#access' => user_access('administer xmlsitemap'),
    '#group' => 'additional_settings',
    '#attached' => array(
      'js' => array(
        'vertical-tabs' => drupal_get_path('module', 'xmlsitemap') . '/xmlsitemap.js',
      ),
    ),
  );

  // Show a warning if the link is not accessible and will not be included in
  // the sitemap.
  if (!$link['access']) {
    $form['xmlsitemap']['warning'] = array(
      '#type' => 'markup',
      '#prefix' => '<p><strong>',
      '#suffix' => '</strong></p>',
      '#value' => ('This item is not currently visible to anonymous users, so it will not be included in the sitemap.'),
    );
  }

  // Status field (inclusion/exclusion)
  $form['xmlsitemap']['status'] = array(
    '#type' => 'select',
    '#title' => t('Inclusion'),
    '#options' => xmlsitemap_get_status_options($link['status_default']),
    '#default_value' => $link['status_override'] ? $link['status'] : 'default',
  );
  $form['xmlsitemap']['status_default'] = array(
    '#type' => 'value',
    '#value' => $link['status_default'],
  );
  $form['xmlsitemap']['status_override'] = array(
    '#type' => 'value',
    '#value' => $link['status_override'],
  );

  // Priority field
  $form['xmlsitemap']['priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#options' => xmlsitemap_get_priority_options($link['priority_default']),
    '#default_value' => $link['priority_override'] ? number_format($link['priority'], 1) : 'default',
    '#description' => t('The priority of this URL relative to other URLs on your site.'),
  );
  $form['xmlsitemap']['priority_default'] = array(
    '#type' => 'value',
    '#value' => $link['priority_default'],
  );
  $form['xmlsitemap']['priority_override'] = array(
    '#type' => 'value',
    '#value' => $link['priority_override'],
  );

  // Other persistent fields.
  //$form['xmlsitemap']['lastmod'] = array(
  //  '#type' => 'value',
  //  '#value' => $node->xmlsitemap['lastmod'],
  //);
  //$form['xmlsitemap']['changefreq'] = array(
  //  '#type' => 'value',
  //  '#value' => $node->xmlsitemap['changefreq'],
  //);
  //$form['xmlsitemap']['changecount'] = array(
  //  '#type' => 'value',
  //  '#value' => $node->xmlsitemap['changecount'],
  //);

  // Hide the priority field if the link is excluded.
  if (module_exists('ctools')) {
    ctools_include('dependent');
    $default = $link['status_default'] ? array('default', 1) : array(1);
    $form['xmlsitemap']['priority'] += array(
      '#process' => array('form_expand_ahah', 'ctools_dependent_process'),
      '#dependency' => array('edit-xmlsitemap-status' => $default),
    );
  }

  // Add the submit handler to adjust the default values if selected.
  if (!in_array('xmlsitemap_process_form_link_options', $form['#submit'])) {
    array_unshift($form['#submit'], 'xmlsitemap_process_form_link_options');
  }
}

/**
 * Get a list of priority options.
 *
 * @param $default
 *   Include a 'default' option.
 * @param $guides
 *   Add helpful indicators for the highest, middle and lowest values.
 * @return
 *   An array of options.
 */
function xmlsitemap_get_priority_options($default = NULL, $guides = TRUE) {
  $options = array();
  $priorities = array(
    '1.0' => t('1.0'),
    '0.9' => t('0.9'),
    '0.8' => t('0.8'),
    '0.7' => t('0.7'),
    '0.6' => t('0.6'),
    '0.5' => t('0.5'),
    '0.4' => t('0.4'),
    '0.3' => t('0.3'),
    '0.2' => t('0.2'),
    '0.1' => t('0.1'),
    '0.0' => t('0.0'),
  );

  if (isset($default)) {
    $default = number_format($default, 1);
    $options['default'] = t('Default (@value)', array('@value' => $priorities[$default]));
  }

  // Add the rest of the options.
  $options += $priorities;

  if ($guides) {
    $options['1.0'] .= ' ' . t('(highest)');
    $options['0.5'] .= ' ' . t('(normal)');
    $options['0.0'] .= ' ' . t('(lowest)');
  }

  return $options;
}

/**
 * Get a list of priority options.
 *
 * @param $default
 *   Include a 'default' option.
 * @return
 *   An array of options.
 *
 * @see _xmlsitemap_translation_strings()
 */
function xmlsitemap_get_status_options($default = NULL) {
  $options = array();
  $statuses = array(
    1 => t('Included'),
    0 => t('Excluded'),
  );

  if (isset($default)) {
    $default = $default ? 1 : 0;
    $options['default'] = t('Default (@value)', array('@value' => drupal_strtolower($statuses[$default])));
  }

  $options += $statuses;

  return $options;
}
