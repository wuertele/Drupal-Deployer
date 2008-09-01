<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  Link general file functionalities to services module.
 */

/**
 * Returns an array of base64 encoded files attached to a node
 *
 * @param $nid
 *   Number. Node ID
 * @return
 *   Array. A list of all files from the given node
 */
function file_service_get_files($nid) {
  $node = node_load($nid);
  if (isset($node->files)) {
    $files = array();
    foreach ($node->files as $file) {
      //rebuild the files array so it only contains files we know we're allowed to list
      if ($file->list) {
        $files[] = $file;
      }
    }
    if (count($files) > 0) {
      $send = array();
      foreach ($files as $file) {
        $file = array_shift($files);
        $filepath = variable_get('file_service_fullpath', '/') . $file->filepath;
        $binaryfile = fopen($filepath, 'rb');
        $send[$file->fid]['file'] = base64_encode(fread($binaryfile, filesize($filepath)));
        $send[$file->fid]['filename'] = $file->filename;
        $send[$file->fid]['uid'] = $file->uid;
        $send[$file->fid]['filemime'] = $file->filemime;
        $send[$file->fid]['filesize'] = $file->filesize;
        $send[$file->fid]['status'] = $file->status;
        $send[$file->fid]['timestamp'] = $file->timestamp;
        fclose($binaryfile);
      }
    }
  }

  //don't fail silently if there are no files!
  if (!$send) {
    $send = array();
    $send[] = 'NO FILES';
  }

  return $send;
}

/**
 * Access callback function for the files service
 */
function file_service_get_files_access() {
  return user_access('get binary files');
}

/**
 * Admin form for file service
 */
function file_service_admin_form() {
  // Define our admin form
  $form = array();
  $form['fullpath'] = array(
    '#title' => t('Path to Drupal installation'),
    '#type' => 'textfield',
    '#required' => 'true',
    '#default_value' => variable_get('file_service_fullpath', '/'),
  );
  $form['submit'] = array(
    '#value' => 'Save settings',
    '#type' => 'submit',
  );

  return $form;
}

function file_service_admin_form_submit(&$form, &$form_state) {
  variable_set('file_service_fullpath', $form_state['values']['fullpath']);
  drupal_set_message(t('Settings saved.'));
}
