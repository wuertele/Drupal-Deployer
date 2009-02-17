<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the robotstxt module.
 */

/**
 * Administration settings form.
 *
 * @see system_settings_form()
 */
function robotstxt_admin_settings() {
  $base = variable_get('robotstxt', FALSE);

  if ($base === FALSE) {
    $base = _robotstxt_get_file_contents();
  }

  $form['robotstxt'] = array(
    '#type' => 'textarea',
    '#title' => t('Contents of robots.txt'),
    '#default_value' => $base,
    '#cols' => 60,
    '#rows' => 20,
    '#wysiwyg' => FALSE,
  );

  return system_settings_form($form);
}
