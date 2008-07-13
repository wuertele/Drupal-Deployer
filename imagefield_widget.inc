<?php

/**
 * @file
 * ImageField widget hooks and callbacks.
 */

/**
 * Implementation of hook_widget_info().
 */
function imagefield_widget_info() {
  return array(
    'imagefield_widget' => array(
      'label' => t('Image'),
      'field types' => array('image'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'callbacks' => array('default value' => CONTENT_CALLBACK_CUSTOM),
    ),
  );
}

/**
 * Implementation of hook_widget_settings().
 */
function imagefield_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      $form = array();
      $form['max_resolution'] = array(
        '#type' => 'textfield',
        '#title' => t('Maximum resolution for Images'),
        '#default_value' => $widget['max_resolution'] ? $widget['max_resolution'] : 0,
        '#size' => 15,
        '#maxlength' => 10,
        '#description' =>
        t('The maximum allowed image size expressed as WIDTHxHEIGHT (e.g. 640x480). Set to 0 for no restriction. If a larger image is uploaded, it will be resized to reflect the given width and height.'),
      );
      $form['max_filesize'] = array(
        '#type' => 'textfield',
        '#title' => t('Maximum filesize for Images'),
        '#default_value' => $widget['max_filesize'] ? $widget['max_filesize'] : 0,
        '#size' => 6,
        '#description' => t('The maximum allowed image file size expressed in kilobytes. Set to 0 for no restriction.')
      );
      $form['file_path'] = array(
        '#type' => 'textfield',
        '#title' => t('Image path'),
        '#default_value' => $widget['file_path'] ? $widget['file_path'] : '',
        '#description' => t('Optional subdirectory within the "%dir" directory where images will be stored. Do not include trailing slash. You can use the following tokens in the image path.', array('%dir' => variable_get('file_directory_path', 'files'))),
        '#element_validate' => array('_imagefield_widget_settings_file_path_validate'), 
        '#suffix' => theme('token_help', 'user'),
      );

      $form['file_extensions'] = array(
        '#type' => 'textfield',
        '#title' => t('Permitted upload file extensions.'),
        '#default_value' => $widget['file_extensions'] ? $widget['file_extensions'] : 'jpg jpeg png gif',
        '#size' => 64,
        '#maxlength' => 64,
        '#description' => t('Extensions a user can upload to this field. Seperate extensions with a space and do not include the leading dot.')
      );

      $form['custom_alt'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable custom alternate text'),
        '#default_value' =>  $widget['custom_alt'] ? $widget['custom_alt'] : 0,
        '#description' => t('Enable custom alternate text for images. Filename will be used if not checked.'),
      );
      $form['custom_title'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable custom title text'),
        '#default_value' =>  $widget['custom_title'] ? $widget['custom_title'] : 0,
        '#description' => t('Enable custom title text for images. Filename will be used if not checked.'),
      );
      return $form;

    case 'save':
      return array('max_resolution', 'max_filesize', 'file_path', 'file_extensions', 'custom_alt', 'custom_title');
  }
}

function _imagefield_widget_settings_file_path_validate($element, &$form_state) {
  // Strip slashes from the beginning and end of $widget['file_path']
  $form_state['values']['file_path'] =  trim($form_state['values']['file_path'], '\\/');
}

/**
 * Implementation of hook_form_alter(). Set the appropriate
 * attibutes to allow file uploads on the field settings form.
 */
function _imagefield_content_admin_field_form_alter(&$form, $form_state, $form_id) {
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
}

/**
 * Create the image directory relative to the 'files' dir recursively for every
 * directory in the path.
 *
 * @param $directory
 *   The directory path under files to check, such as 'photo/path/here'
 * @param $form_element
 *   A form element to throw an error on if the directory is not writable
 */
function imagefield_check_directory($directory, $fieldname = NULL) {
  $directory = field_file_strip_path($directory);
  foreach (explode('/', $directory) as $dir) {
    $dirs[] = $dir;
    $path = file_create_path(implode($dirs, '/'));
    if (!field_file_check_directory($path, FILE_CREATE_DIRECTORY, $fieldname)) {
      watchdog('imagefield', t('Imagefield failed to create directory(%d) at (%p).', array('%d' => $directory, '%p' => $path)), WATCHDOG_ERROR);
      return false;
    }
  }
  return true;
}

function _imagefield_scale_image($file, $resolution = 0) {
  $info = image_get_info($file['filepath']);
  if ($info) {
    list($width, $height) = explode('x', $resolution);
    if ($width && $height) {
      $result = image_scale($file['filepath'], $file['filepath'], $width, $height);
      if ($result) {
        $file['filesize'] = filesize($file['filepath']);
        drupal_set_message(t('The image %filename was resized to fit within the maximum allowed resolution of %resolution pixels', array('%resolution' => $resolution, '%filename' => $file['filename'])));
      }
    }
  }
  return $file;
}


/**
 * Implementation of hook_widget().
 */
function imagefield_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  if (empty($items[$delta])) { 
    $items[$delta] = array('fid' => 0, 'alt' => '', 'title' => '', 'data' => '');
  }

  $element['#attributes'] = array('enctype' => 'multipart/form-data');
  $element = array(
    '#type' => 'imagefield_widget',
    '#default_value' => $items[$delta],
  );
  // store the original item to revert form if necessary.
  $form_state['storage'][$field['field_name']][$delta]['orig'] = $items[$delta];
  
  return $element;
}


function imagefield_widget_value($element, $edit = FALSE) {
  dsm('imagefield_widget_value');
  dsm($edit);
  if (!$edit) { 
    return $element['#default_value'];
  }
  else {
    return array(
      'fid' => $edit['fid'],
      'alt' => $edit['alt'],
      'title' => $edit['title'],
      'data' => '',
    );
  }
  return $value;

}


function imagefield_widget_process($element, $edit, &$form_state, $form) {
  //dsm('imagefield_widget_process');
  //dsm($element);
  //dsm($form_state);

  $item = $element['#default_value'];
  $file = $item += field_file_load($item['fid']);
  $delta = $element['#delta'];
  $field_name = $element['#field_name'];

  
  $element['fid'] = array('#type' => 'hidden', '#value' =>  $file['fid']);
  if ($file['fid'] != 0) {
    $element['preview'] = array('#type' => 'markup', '#value' => theme('imagefield_item', $file));  
  }
  $element['alt'] = array(
    '#type' => 'textfield', 
    '#title' => t('Alternate Text'),
    '#default_value' => $file['alt'],
  );
  $element['title'] = array(
    '#type' => 'textfield', 
    '#title' => t('Title'),
    '#default_value' => $file['title'],
  );
  $element['data'] = array(
    '#type' => 'hidden', 
    '#value' => '',
  );

  if ($file['fid'] != 0) {
    $element['remove_btn'] = array(
      '#type' => 'submit', 
      '#value' => t('Remove'),
      '#submit' => array('imagefield_widget_submit_remove_btn'),
      '#field_name' => $element['#field_name'],
      '#delta' => $element['#delta'],
    );
    return $element;
  }

  $element['upload'] = array(
    '#name' => 'file['. $field_name .'_'. $delta .'_upload' .']',
    '#type' => 'file',
    '#title' => t('New Upload'),
  );
  $element['upload_btn'] = array(
    '#type' => 'submit', 
    '#value' => t('Upload'),
    '#submit' => array('imagefield_widget_submit_upload_btn'),
    '#field_name' => $element['#field_name'],
    '#delta' => $element['#delta'],
  );

  // @todo: jquery file browser.
  $element['reuse'] = array(
    '#type' => 'textfield',
    '#title' => t('Reuse Existing File'),
    '#description' => t('Enter a Drupal filepath or file id to use an existing file.'),
  );
  $element['reuse_btn'] = array(
    '#type' => 'submit', 
    '#value' => t('ReUse'),
    '#submit' => array('imagefield_widget_submit_reuse_btn'),
    '#field_name' => $element['#field_name'],
    '#delta' => $element['#delta'],
  );

  return $element;
}

function imagefield_widget_submit_remove_btn($form, &$form_state) {
  //dsm ('imagefield_widget_submit_remove_btn');
  //dsm($form);
  //dsm($form_state);
  $field_name = $form_state['clicked_button']['#field_name'];
  $delta = $form_state['clicked_button']['#delta'];
  
  $form_state['values'][$field_name][$delta] = array(
    'fid' => 0,
    'alt' => '',
    'filepath' => '',
    'filename' => '',
    'title' => '',
    'data' => '',
  ); 
}


function imagefield_widget_submit_reuse_btn($form, &$form_state) {
  //dsm('imagefield_widget_submit_reuse_btn');
  //dsm($form);
  //dsm($form_state);

  $field_name = $form_state['clicked_button']['#field_name'];
  $delta = $form_state['clicked_button']['#delta'];

  $reuse =  trim($form_state['values'][$field_name][$delta]['reuse']);
  if (empty($reuse))  return;

  // if numeric assume we got an fid and try to load it.
  if (is_numeric($reuse) && $file = field_file_load($reuse)) {
    $form_state['values'][$field_name][$delta]['fid'] = $file['fid'];
  } 
  else {
    // load fid from path..
    $result = db_query("SELECT fid FROM {files} WHERE filepath='%s'", $reuse);
    if ($fid = db_result($result)) {
      $form_state['values'][$field_name][$delta]['fid'] = $fid;
    }
  }
}

function imagefield_widget_after_build($element) {
  dsm('imagefield_widget_after_build');
  //dsm($element);
 
  $element['#value'] = array(
    'fid' => $element['fid']['#value'],
    'alt' => $element['alt']['#value'],
    'title' => $element['title']['#value'],
    'data' => $element['data']['#value'],
  );
  return $element;
}

function _imagefield_widget_validate($element, &$form_state) {
  dsm('imagefield_validate');
  //dsm($element);
}



/**
 * FormAPI theme function. Theme the output of an image field.
 */
function theme_imagefield_widget(&$element) {
  return $element['#children'];
}



