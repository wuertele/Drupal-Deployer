<?php
// $Id$

function site_verify_list() {
  $engines = site_verify_get_engines();

  $headers = array(
    t('Engine'),
    t('Verification'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();

  $query = db_query("SELECT svid, engine, file, meta FROM {site_verify}");
  while ($record = db_fetch_object($query)) {
    $engines[$record->engine]['used'] = TRUE;
    $rows[] = array(
      $engines[$record->engine]['name'],
      t('Yes'),
      l('Edit', 'admin/build/site-verify/edit/' . $record->svid),
      l('Delete', 'admin/build/site-verify/delete/' . $record->svid),
    );
  }

  foreach ($engines as $engine => $details) {
    if (empty($details['used']) || $engine == 'custom') {
      $rows[] = array(
        $details['name'],
        '',
        l('Add', 'admin/build/site-verify/add/' . $engine),
        '',
      );
    }
  }

  $output = theme('table', $headers, $rows);
  return $output;
}

function site_verify_edit_form($form_state, $record = NULL) {
  $engines = site_verify_get_engines();

  if (is_string($record)) {
    if (array_key_exists($record, $engines)) {
      $record = array('engine' => $record);
    }
    else {
      return drupal_not_found();
    }
  }

  $record += array(
    'svid' => NULL,
    'file' => '',
    'file_contents' => t('This is a verification page.'),
    'meta' => '',
  );

  $engine = site_verify_get_engine($record['engine']);

  $form['svid'] = array(
    '#type' => 'value',
    '#value' => $record['svid'],
  );
  $form['engine'] = array(
    '#type' => 'value',
    '#value' => $record['engine'],
  );
  $form['engine_name'] = array(
    '#type' => 'item',
    '#title' => t('Search engine'),
    '#value' => $engine['name'],
  );
  $form['meta'] = array(
    '#type' => 'textfield',
    '#title' => t('Verification META tag'),
    '#default_value' => $record['meta'],
    '#description' => t('This is the full meta tag provided for verification. This value will be inserted in the <a href="@front-page">front page</a>.', array('@front-page' => url())),
    '#element_validate' => $engine['meta_validate'],
    '#access' => $engine['meta'],
  );

  $form['file_upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload an existing verification file'),
    '#description' => t('If you have been provided with an actual file, you can simply upload the file.'),
    '#access' => $engine['file'],
  );
  $form['file'] = array(
    '#type' => 'textfield',
    '#title' => t('Verification file'),
    '#default_value' => $record['file'],
    '#description' => t('The name of the HTML verification file you were asked to upload.'),
    '#element_validate' => $engine['file_validate'],
    '#access' => $engine['file'],
  );
  $form['file_contents'] = array(
    '#type' => 'textarea',
    '#title' => t('Verification file contents'),
    '#default_value' => $record['file_contents'],
    '#element_validate' => $engine['file_contents_validate'],
    '#wysiwyg' => FALSE,
    '#access' => $engine['file_contents'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 10,
  );
  $form['cancel'] = array(
    '#value' => l(t('Cancel'), 'admin/build/site-verify'),
    '#weight' => 11,
  );

  if (!variable_get('clean_url', 0)) {
    drupal_set_message(t('Using verification files will not work if <a href="@clean-urls">clean URLs</a> are disabled.', array('@clean-urls' => url('admin/settings/clean-url'))), 'error', FALSE);
    $form['file']['#disabled'] = TRUE;
    $form['file_contents']['#disabled'] = TRUE;
    $form['file_upload']['#disabled'] = TRUE;
  }

  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['#redirect'] = 'admin/build/site-verify';
  return $form;
}

function site_verify_edit_form_validate($form, &$form_state) {
  // Import the uploaded verification file.
  if ($file = file_save_upload('file_upload', array(), FALSE, FILE_EXISTS_REPLACE)) {
    $contents = @file_get_contents($file->filepath);
    file_delete($file->filepath);
    if ($contents === FALSE) {
      drupal_set_message(t('The verification file import failed, because the file %filename could not be read.', array('%filename' => $file->filename)), 'error');
    }
    else {
      $form_state['values']['file'] = $file->filename;
      if ($form['file_contents']['#access']) {
        $form_state['values']['file_contents'] = $contents;
      }
      drupal_set_message(t('The verification file <a href="@filename">@filename</a> was successfully imported.', array('@filename' => $file->filename)));
    }
  }
}

function site_verify_edit_form_submit($form, $form_state) {
  if ($form_state['values']['svid']) {
    drupal_write_record('site_verify', $form_state['values'], array('svid'));
  }
  else {
    drupal_write_record('site_verify', $form_state['values']);
  }

  // The menu needs to be rebuilt if the page URL has changed or reset.
  variable_set('menu_rebuild_needed', TRUE);
}

function site_verify_delete_form($form_state, $record) {
  $record['engine_details'] = site_verify_get_engine($record['engine']);

  $form['record'] = array(
    '#type' => 'value',
    '#value' => $record,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete the site verification for %engine?', array('%engine' => $record['engine_details']['name'])),
    'admin/build/site-verify',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function site_verify_delete_form_submit($form, &$form_state) {
  $record = $form_state['values']['record'];
  db_query("DELETE FROM {site_verify} WHERE svid = %d", $record['svid']);
  drupal_set_message(t('Verification for %engine has been deleted.', array('%engine' => $record['engine_details']['name'])));
  watchdog('site_verify', 'Verification for %engine deleted.', array('%engine' => $record['engine_details']['name']), WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/build/site-verify';
}

function site_verify_validate_meta_google($element, &$form_state) {
  $value = strtolower(trim($element['#value']));
  if ($value != '' && !preg_match('%\A<meta name="verify-v1" content="[\S]+" />\Z%', $value)) {
    form_error($element, t('Invalid verification meta tag.'));
  }
}

function site_verify_validate_page_google($element, &$form_state) {
  $value = strtolower(trim($element['#value']));
  if ($value != '' && !preg_match('%\Agoogle[\da-f]+\.html\Z%', $value)) {
    form_error($element, t('Invalid verification file.'));
  }
}
