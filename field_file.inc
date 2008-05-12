<?php

/**
 * Load a file object from the database.
 *
 * @param $file_id
 *   A numeric file id or string containing the file path.
 * @param $reset
 *   Whether to reset the internal file_load cache.
 */
function field_file_load($file_id, $reset = NULL) {
  static $files = array();

  if ($reset) {
    $files = array();
  }

  if (is_numeric($file_id)) {
    if (isset($files[$file_id])) {
      return clone($files[$file_id]);
    }
    $file = db_fetch_object(db_query('SELECT f.* FROM {files} f WHERE f.fid = %d', $file_id));
  }
  else {
    $file = db_fetch_object(db_query("SELECT f.* FROM {files} f WHERE f.filepath = '%s'", $file_id));
  }

  module_invoke_all('file', 'load', $file);

  // Cache the fully loaded value.
  $files[$file->fid] = clone($file);
  return (array)$file;
}


/**
 * Update an field item file. Delete marked items if neccessary and set new items as permamant.
 *
 * @param $node
 *    Node object this file is be associated with.
 * @param $file
 *    File to be inserted, passed by reference since fid should be attached.
 * @return array
 */
function field_file_save($node, &$file) {
  // If this item is marked for deletion.
  if (!empty($file['flags']['delete'])) {
    // if we're creating a new revision, return an empty array so CCK will remove the item.
    if ($node->old_vid) return array();
    // otherwise delete the file and return an empty array.
    if (_imagefield_file_delete($file)) return array();
  }

  // set permanent status on files if unset. 
  if (empty($file['status'])) {
    file_set_status((object)$file, FILE_STATUS_PERMANENT);
    return $file;
  }
}

/**
 * Delete a field file and its database record.
 *
 * @param $path 
 *   A file object.
 * @param $force
 *   Force File Deletion ignoring reference counting.
 * @return mixed 
 *   TRUE for success, Array for reference count block, or FALSE in the event of an error.
 */
function field_file_delete($file, $force = FALSE) {
  $file = (object)$file;
  // If any module returns a value from the reference hook, the 
  // file will not be deleted from Drupal, but file_delete will
  // return a populated array that tests as TRUE.
  if (!$force && $references = module_invoke_all('file', 'references', $file)) {
    return $references;
  }

  // Let other modules clean up on delete.
  module_invoke_all('file', 'delete', $file, $field);
 
  // Make sure the file is deleted before removing its row from the 
  // database, so UIs can still find the file in the database.
  if (file_delete($file->filepath)) {
    db_query('DELETE FROM {files} WHERE fid = %d', $file->fid);
    return TRUE;
  }
  return FALSE;
}
