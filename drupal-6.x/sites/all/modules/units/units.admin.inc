<?php
// $Id$

/**
 * @file
 * Units admin UI.
 */

/**
 * Units configuration form
 */
function units_admin_settings() {
  $form = array();
  $description = t('Configure site-wide list of enabled units. Modules that integrate with Units module may use this list.
  <br/>If nothing is selected in ALL categories, all units will be enabled.'); 
  $form['description'] = array(
    '#value' => $description,
  );
  $units = units_get_units();
  $categories = array();
  foreach($units as $id => $unit) {
    $categories[$unit['category_id']] = $unit['category'];
  }
  foreach($categories as $cat_id => $desc) {
    $form['category_' .$cat_id] = array(
      '#type' => 'fieldset',
      '#title' => $desc,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    foreach($units as $id => $unit) {
      if ($unit['category_id'] == $cat_id) {
        $form['category_' .$cat_id]['enable_'. $id] = array(
          '#type' => 'checkbox',
          '#title' => $unit['fullname'],
          '#default_value' => in_array($id, units_get_enabled_units()) ? 1 : 0,
        );
      }
    }
  }
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('units_admin_settings_submit'),
  );
  return $form;
}

/**
 * Submit callback for units admin settings form. 
 */
function units_admin_settings_submit($form, &$form_state) {
  $units = units_get_units();
  $enabled = array();
  foreach ($units as $id => $unit) {
    if (isset($form_state['values']['enable_'. $id]) && $form_state['values']['enable_'. $id] == 1) {
      $enabled[] = $id;
    }
  }
  variable_set('units_enabled_units', $enabled);
  drupal_set_message(t('Enabled units list has been saved.'));
  $form_state['redirect'] = 'admin/content/units';  
}
