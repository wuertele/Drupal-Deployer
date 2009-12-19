<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the xmlsitemap_custom module.
 */

function xmlsitemap_custom_list_links() {
  $header = array(
    array('data' => t('Location'), 'field' => 'loc'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();

  $sql = "SELECT id, loc, priority FROM {xmlsitemap} WHERE type = 'custom'" . tablesort_sql($header);
  $query = db_query($sql);
  while ($link = db_fetch_object($query)) {
    $rows[] = array(
      l($link->loc, $link->loc),
      l(t('Edit'), 'admin/settings/xmlsitemap/custom/edit/' . $link->id, array('query' => drupal_get_destination())),
      l(t('Delete'), 'admin/settings/xmlsitemap/custom/delete/' . $link->id, array('query' => drupal_get_destination())),
    );
  }

  $rows[] = array(
    array(
      'data' => l(t('Add custom link'), 'admin/settings/xmlsitemap/custom/add', array('query' => drupal_get_destination())),
      'colspan' => 3,
    ),
  );

  $output = theme('table', $header, $rows);
  return $output;
}

function xmlsitemap_custom_edit_link_form($form_state, $link = array()) {
  module_load_include('inc', 'xmlsitemap', 'xmlsitemap.admin');

  $link += array(
    'id' => (int) db_result(db_query("SELECT MAX(id) FROM {xmlsitemap} WHERE type = 'custom'")) + 1,
    'loc' => '',
    'priority' => 0.5,
    'lastmod' => 0,
    'changefreq' => 0,
    'changecount' => 0,
    'language' => '',
  );

  $form['type'] = array(
    '#type' => 'value',
    '#value' => 'custom',
  );
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $link['id'],
  );
  $form['loc'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to link'),
    '#field_prefix' => url('', array('absolute' => TRUE)),
    '#default_value' => drupal_get_path_alias($link['loc'], $link['language']),
    '#required' => TRUE,
    '#size' => 30,
  );
  $form['xmlsitemap']['priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#options' => xmlsitemap_get_priority_options(),
    '#default_value' => number_format($link['priority'], 1),
    '#description' => t('The priority of this URL relative to other URLs on your site.'),
  );
  $form['xmlsitemap']['changefreq'] = array(
    '#type' => 'select',
    '#title' => t('Change frequency'),
    '#options' => array(0 => t('No value')) + xmlsitemap_get_changefreq_options(),
    '#default_value' => $link['changefreq'],
    '#description' => t('How frequently the page is likely to change. This value provides general information to search engines and may not correlate exactly to how often they crawl the page.'),
  );
  $languages = module_exists('locale') ? locale_language_list() : array();
  $form['language'] = array(
    '#type' => 'select',
    '#title' => t('Language'),
    '#default_value' => $link['language'],
    '#options' => array('' => t('Language neutral')) + $languages,
    '#access' => $languages,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 10,
  );
  $form['cancel'] = array(
    '#value' => l(t('Cancel'), 'admin/settings/xmlsitemap/custom'),
    '#weight' => 11,
  );
  $form['#redirect'] = 'admin/settings/xmlsitemap/custom';

  return $form;
}

function xmlsitemap_custom_edit_link_form_validate(&$form, &$form_state) {
  $link = &$form_state['values'];

  // Make sure we trim and normalize the path first.
  $link['loc'] = trim($link['loc']);
  $normal_path = drupal_get_normal_path($link['loc']);
  if ($link['loc'] != $normal_path) {
    drupal_set_message(t('The sitemap system stores system paths only, but will use the URL alias in the sitemap output. The custom link %link has been stored as %normal_path.', array('%link' => $link['loc'], '%normal_path' => $normal_path)));
    $link['loc'] = $normal_path;
  }

  // Test anonymous user access to the custom link paths.
  xmlsitemap_switch_user(0);
  $menu_item = menu_get_item($link['loc']);
  xmlsitemap_restore_user();

  // Since the menu item access results are cached, manually check the current path.
  if ($menu_item && strpos($link['loc'], 'admin/settings/xmlsitemap/custom') === 0 && !user_access('administer xmlsitemap', drupal_anonymous_user())) {
    $menu_item['access'] = FALSE;
  }

  if (db_result(db_query_range("SELECT 1 FROM {xmlsitemap} WHERE type <> 'custom' AND loc = '%s' AND status = 1 AND access = 1 AND language IN ('%s', '')", $link['loc'], $link['language'], 0, 1))) {
    form_set_error('loc', t('There is already an existing link in the sitemap with the path %link.', array('%link' => $link['loc'])));
  }
  elseif (empty($menu_item['access']) && !is_readable('./' . $link['loc'])) {
    // @todo Harden this file exists check to make sure we can't link to files
    // like .htaccess.
    form_set_error('loc', t('The custom link %link is either invalid or it cannot be accessed by anonymous users.', array('%link' => $link['loc'])));
  }
}

function xmlsitemap_custom_edit_link_form_submit(&$form, &$form_state) {
  $link = $form_state['values'];
  xmlsitemap_save_link($link);
  drupal_set_message(t('The custom link for %loc was saved.', array('%loc' => $link['loc'])));
}

function xmlsitemap_custom_delete_link_form($form_state, array $link) {
  $form['link'] = array(
    '#type' => 'value',
    '#value' => $link,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete the custom link for %loc?', array('%loc' => $link['loc'])),
    'admin/settings/xmlsitemap/custom',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function xmlsitemap_custom_delete_link_form_submit($form, &$form_state) {
  $link = $form_state['values']['link'];
  xmlsitemap_delete_link(array('type' => 'custom', 'id' => $link['id']));
  drupal_set_message(t('The custom link for %loc has been deleted.', array('%loc' => $link['loc'])));
  watchdog('xmlsitemap', 'The custom link for %loc has been deleted.', array('%loc' => $link['loc']), WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/settings/xmlsitemap/custom';
}
