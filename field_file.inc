<?php

/**
 * Load a file object from the database.
 *
 * @param $fid
 *   A numeric file id or string containing the file path.
 *
 * @param $reset
 *   Whether to reset the internal file_load cache.
 */
function field_file_load($fid, $reset = NULL) {
  static $files = array();
  
  if (empty($fid)) return array();

  // Reset internal cache.
  if ($reset) {
    $files = array();
  }

  // Serve file from internal cache if available.
  if (empty($files[$fid])) {
    if (is_numeric($fid)) {
      $file = db_fetch_object(db_query('SELECT f.* FROM {files} f WHERE f.fid = %d', $fid));
    }
    else {
      $file = db_fetch_object(db_query("SELECT f.* FROM {files} f WHERE f.filepath = '%s'", $fid));
    }

    if (!$file) {
      return array();
    }  
    module_invoke_all('file', 'load', $file);

    // Cache the fully loaded value by both fid and filepath.
    // use non-copying objects to save memory.
    $files[$fid] = $file;
  }  
  
  // cast to array for field. hook_file expects objects as well as core file functions.
  return (array)$files[$fid];
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
    // cast to object since core functions us objects.
    $file = (object)$file;
    file_set_status($file, FILE_STATUS_PERMANENT);
    $file = (array)$file;
  }
  return $file;
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

/**
 * A silent version of file.inc:file_check_directory it's only talkative
 * on errors.
 *
 * Check that the directory exists and is writable. Directories need to
 * have execute permissions to be considered a directory by FTP servers, etc.
 *
 * @param $directory A string containing the name of a directory path.
 * @param $mode A Boolean value to indicate if the directory should be created
 *   if it does not exist or made writable if it is read-only.
 * @param $form_item An optional string containing the name of a form item that
 *   any errors will be attached to. This is useful for settings forms that
 *   require the user to specify a writable directory. If it can't be made to
 *   work, a form error will be set preventing them from saving the settings.
 * @return FALSE when directory not found, or TRUE when directory exists.
 */
function field_file_check_directory(&$directory, $mode = 0, $form_item = NULL) {
  $directory = rtrim($directory, '/\\');

  // Check if directory exists.
  if (!is_dir($directory)) {
    if (($mode & FILE_CREATE_DIRECTORY) && @mkdir($directory)) {
      @chmod($directory, 0775); // Necessary for non-webserver users.
    }
    else {
      if ($form_item) {
        form_set_error($form_item, t('The directory %directory does not exist.', array('%directory' => $directory)));
      }
      watchdog('file system', 'The directory %directory does not exist.', array('%directory' => $directory), WATCHDOG_ERROR);
      return FALSE;
    }
  }

  // Check to see if the directory is writable.
  if (!is_writable($directory)) {
    if (($mode & FILE_MODIFY_PERMISSIONS) && !@chmod($directory, 0775)) {
      if ($form_item) {
        form_set_error($form_item, t('The directory %directory is not writable', array('%directory' => $directory)));
      }  
      watchdog('file system', 'The directory %directory is not writable, because it does not have the correct permissions set.', array('%directory' => $directory), WATCHDOG_ERROR);
      return FALSE;
    }
  }
  if ((file_directory_path() == $directory || file_directory_temp() == $directory) && !is_file("$directory/.htaccess")) {
    $htaccess_lines = "SetHandler Drupal_Security_Do_Not_Remove_See_SA_2006_006\nOptions None\nOptions +FollowSymLinks";
    if (($fp = fopen("$directory/.htaccess", 'w')) && fputs($fp, $htaccess_lines)) {
      fclose($fp);
      chmod($directory .'/.htaccess', 0664);
    }
    else {
      $message =  "Security warning: Couldn't write .htaccess file. Please create a .htaccess file in your %directory directory which contains the following lines: <code>!htaccess</code>";
      $repl =  array('%directory' => $directory, '!htaccess' => '<br />'. nl2br(check_plain($htaccess_lines)));
      form_set_error($form_item, t($message, $repl));
      watchdog('security', $message, $repl, WATCHDOG_ERROR);
    }
  }
  return TRUE;
}

/**
 * Remove a possible leading file directory path from the given path.
 */
function field_file_strip_path($path) {
  $dirpath = file_directory_path();
  $dirlen = strlen($dirpath);
  if (substr($path, 0, $dirlen + 1) == $dirpath .'/') {
    $path = substr($path, $dirlen + 1);
  }
  return $path;
}


