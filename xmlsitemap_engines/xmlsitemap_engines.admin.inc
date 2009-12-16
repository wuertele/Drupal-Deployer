<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the xmlsitemap_engines module.
 */

/**
 * Admin settings form.
 */
function xmlsitemap_engines_settings_form() {
  $form['#engines'] = xmlsitemap_engines_get_engines();
  $form['xmlsitemap_engines_engines'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Submit the sitemap to the following engines'),
    '#default_value' => xmlsitemap_engines_var('engines'),
  );
  foreach ($form['#engines'] as $engine => $engine_details) {
    $form['xmlsitemap_engines_engines']['#options'][$engine] = $engine_details['name'];
  }

  $form['xmlsitemap_engines_minimum_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Do not submit more often than every'),
    '#options' => /*array(0 => t('No minimum')) +*/ drupal_map_assoc(array(300, 900, 1800, 3600, 10800, 21600, 32400, 43200, 86400, 172800, 259200, 604800, 604800 * 2, 604800 * 4), 'format_interval'),
    '#default_value' => xmlsitemap_engines_var('minimum_lifetime'),
    //'#description' => t('The sitemap will only be submitted if it has been updated since the last submission.'),
  );
  $form['xmlsitemap_engines_submit_updated'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only submit if the sitemap has been updated since the last submission.'),
    '#default_value' => xmlsitemap_engines_var('submit_updated'),
  );

  $form['xmlsitemap_engines_custom_urls'] = array(
    '#type' => 'textarea',
    '#title' => t('Custom submission URLs'),
    '#description' => t('Enter one custom submission URL per line. The token [sitemap] will be replaced with the URL to your sitemap. For example: %example-before would become %example-after.', array('%example-before' => 'http://mysearchengine.com/ping?[sitemap]', '%example-after' => xmlsitemap_engines_prepare_url('http://mysearchengine.com/ping?[sitemap]', url('sitemap.xml', xmlsitemap_get_url_options())))),
    '#default_value' => xmlsitemap_engines_var('custom_urls'),
    '#rows' => 2,
    '#wysiwyg' => FALSE,
  );

  $form['#validate'][] = 'xmlsitemap_engines_settings_form_validate';
  $form['#submit'][] = 'xmlsitemap_engines_settings_form_submit';
  $form['array_filter'] = array('#type' => 'value', '#value' => TRUE);
  return system_settings_form($form);
}

function xmlsitemap_engines_settings_form_validate($form, &$form_state) {
  $custom_urls = &$form_state['values']['xmlsitemap_engines_custom_urls'];
  $custom_urls = preg_split('/[\r\n]+/', $custom_urls, -1, PREG_SPLIT_NO_EMPTY);
  foreach ($custom_urls as $custom_url) {
    $url = xmlsitemap_engines_prepare_url($custom_url, '');
    if (!valid_url($url, TRUE)) {
      form_set_error('xmlsitemap_engines_custom_urls', t('Invalid custom URL %url.', array('%url' => $custom_url)));
    }
  }
  $custom_urls = implode("\n", $custom_urls);
}

function xmlsitemap_engines_settings_form_submit($form, $form_state) {
  // Since verification URLs may have changed, we need to flag a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);
}
