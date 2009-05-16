<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the xmlsitemap_engines module.
 */

/**
 * Admin settings form.
 *
 * @todo Minimum submission times.
 */
function xmlsitemap_engines_settings_form($form_state) {
  $form['#engines'] = xmlsitemap_engines_get_engines();

  $form['xmlsitemap_engines_submit'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Submit the sitemap to the following engines:'),
    '#default_value' => xmlsitemap_engines_var('submit'),
  );
  foreach ($form['#engines'] as $engine => $engine_details) {
    $form['xmlsitemap_engines_submit']['#options'][$engine] = $engine_details['name'];
  }

  $form['xmlsitemap_engines_custom_urls'] = array(
    '#type' => 'textarea',
    '#title' => t('Custom submission URLs'),
    '#description' => t('Enter one custom submission URL per line. The token [sitemap] will be replaced with the URL to your sitemap. For example: %example-before would become %example-after.', array('%example-before' => 'http://mysearchengine.com/ping?[sitemap]', '%example-after' => xmlsitemap_engines_prepare_url('http://mysearchengine.com/ping?[sitemap]'))),
    '#default_value' => xmlsitemap_engines_var('custom_urls'),
    '#rows' => 2,
    '#wysiwyg' => FALSE,
  );

  $form['#validate'][] = 'xmlsitemap_engines_settings_form_validate';
  $form['#submit'][] = 'xmlsitemap_engines_settings_form_submit';
  return system_settings_form($form);
}

function xmlsitemap_engines_settings_form_validate(&$form, $form_state) {
  $custom_urls = &$form_state['values']['xmlsitemap_engines_custom_urls'];
  $custom_urls = preg_split('/[\r\n]+/', $custom_urls, -1, PREG_SPLIT_NO_EMPTY);
  foreach ($custom_urls as $custom_url) {
    $url = xmlsitemap_engines_prepare_url($custom_url);
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
