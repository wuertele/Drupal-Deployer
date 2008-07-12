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
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
 
  if (!empty($items[$delta]) && $file = field_file_load($items[$delta]['fid'])) {
    $file = array_merge($items[$delta], $file); 
  }
  else {
    $file = array(
      'fid' => 0, 
      'title' => '', 
      'alt' => '',
      'filename' => '', 
      'filepath' => '', 
      'filemime' => '',
      'filesize' => 0,
      'source' => 'initialization',
    );
  }

  $widget['fid'] = array(
    '#type' => 'imagefield_widget_file',
    '#default_value' => $file['fid'],
    '#field' => $field,
    '#field_name' => $field['field_name'],
    '#delta' => $delta,
    '#items' => $items,
    '#file' => (object)$file,
  );

  $widget['alt'] = array(
    '#type' => 'textfield',
    '#title' => t('Alternate Text'),
    '#default_value' => '',
  );  
  if (!$field['widget']['custom_alt']) {
    $widget['alt']['#type'] = 'hidden';
  }  

  $widget['title'] = array(
    '#type' => 'textfield',
    '#title' => t('title'),
    '#default_value' => '',
  );  
 
  if ($field['widget']['custom_title']) {
    $widget['title']['#type'] = 'hidden';
  }    

  $widget['alt'] = array('#type' => 'value', '#value' => '');
  $widget['title'] = array('#type' => 'value', '#value' => '');
  $widget['data'] = array('#type' => 'value', '#value' => '');

  return $widget;
}

function imagefield_widget_value($form, $edit = FALSE) {
  return $form['#default_value'];
}


function imagefield_widget_process($element, $edit, &$form_state, $form) {
  return $element;
}

function imagefield_widget_file_value($form, $edit = FALSE) {
  dsm('widget_file_value');
  
  if ($edit === FALSE) {
    return $form['#default_value'];
  }  
  $field = $form['#field'];
  $fieldname = $field['field_name'];
  $upload_name = $fieldname .'_'. $form['#delta'];
  

  global $user;
  $widget_file_path = token_replace($field['widget']['file_path'], 'user', $user);

  $validators = array(
    'file_validate_extensions' => array($field['widget']['file_extensions']),
    'file_validate_size' => array($field['widget']['max_filesize']),
    'file_validate_image_resolution' => array($field['widget']['max_resolution']),
  );

  $complete_file_path = file_directory_path() .'/'. $widget_file_path;

  if (!imagefield_check_directory($widget_file_path, $upload_name)) {
    // watchdog.
    return $form['#default_value'];
  }  
  
  if (!$file = field_file_save_upload($upload_name, $validators, $complete_file_path)) {
    // watchdog.
    return $form['#default_value'];
  }
  return $file['fid'];
}

/**
 * Process the image type element before displaying the field.
 *
 * this process call back should either display an input[@type=file] or 
 * an individual image edit form.
 */
function imagefield_widget_file_process($element, $edit, &$form_state, $form) {
  dsm('widget_file_process');

  $field_name = $element['#field_name'];
  $field = content_fields($field_name);
  $delta = $element['#delta'];

  $form_state['values'][$field['field_name']][$delta] = array(); 
  $field_name_css = str_replace('_', '-', $field['field_name']);
  $id = 'filefield-'. $field_name_css .'-'. $delta .'-form';

  $classes = array(
    'filefield-'. $field_name_css .'-form',
    'filefield-file-form',
  );
  $element['#prefix'] =  '<div id="'. $id .'" class="'. implode(' ', $classes) .'"><div class="filefield-file-edit">';
  $element['#suffix'] = '</div></div>';

  //drupal_add_js(drupal_get_path('module', 'imagefield') .'/imagefield.js');
  drupal_add_css(drupal_get_path('module', 'imagefield') .'/imagefield.css');
  if ($file = field_file_load($element['#value'])) {
    $element['preview'] = array(
      '#type' => 'markup',
      '#value' => '<img src="'. file_create_url(imagefield_file_admin_thumb_path($file)) .'" />',
    );  
    $element['delete'] = array(
      '#name' => $field_name .'_'. $delta .'_delete',
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('filefield_file_edit_delete_submit'), // without JavaScript
      '#ahah' => array( // with JavaScript
        'path' => 'filefield/js/delete/'. $element['#field_name'] .'/'. $field['type_name'] .'/'. $element['#delta'],
        'wrapper' => $id,
        'method' => 'replace',
        'effect' => 'fade',
      ),  
      '#field' => $field,
      '#delta' => $delta,
      '#file' => $file,
    ); 
  }  
  else {   
    $element[$field_name .'_'. $delta] = array(
      '#type' => 'file',
      '#title' => t('Upload Image'),
      '#description' => $element['#description'],
      '#weight' => 9,
      '#attributes' => array('class' => 'imagefield imagefield-'. $field_name, 'accept' => str_replace(' ', '|', trim($field['widget']['file_extensions']))),
      // emulate how FAPI normalizes the _FILES array since this won't go through form_builder.
      '#name' => 'files['. $field_name .'_'. $delta .']',
      '#tree' => FALSE,
    );

    $btn_name =  $field['field_name'] .'_'. $delta .'_upload';
    $element[$btn_name] = array( 
      '#name' => $btn_name, 
      '#type' => 'submit', 
      '#value' => t('Upload'), 
      '#submit' => array('filefield_file_upload_submit'), // without JavaScript 
      // set #parents since widget_value is different.
      '#parents' => array($btn_name),
      '#ahah' => array( // with JavaScript 
        'path' => 'filefield/js/upload/'. $field['field_name'] .'/'. $field['type_name'] .'/'. $delta, 
        'wrapper' => $id, 
        'method' => 'replace', 
        'effect' => 'fade', 
      ), 
      '#weight' => 10, 
      '#field' => $field, 
      '#delta' => $delta, 
    );
  }   
  return $element;
}


function _imagefield_widget_validate($element, &$form_state) {
}


function theme_imagefield_row($element) {
  $output = '<div class="imagefield-edit-preview">'. drupal_render($element['admin_preview']) .'</div>';
  $output .= '<div class="imagefield-edit-image-detail">';
  $output .= '<div class="imagefield-edit-image-flags">'. drupal_render($element['flags']) .'</div>';
  $output .= '<div class="imagefield-edit-image-description">'. drupal_render($element['description']);
  $output .= '</div>';
  $output .= drupal_render($element['alt']);
  $output .= drupal_render($element['title']);
  $output .= drupal_render($element['fid']);
  $output .= '</div>';
  if (isset($element['replace'])) {
    $output .= '<div class="imagefield-edit-image-replace">'. drupal_render($element['replace']) .'</div>';
  }
  return $output;
}

/**
 * FormAPI theme function. Theme the output of an image field.
 */
function theme_imagefield_widget(&$element) {
  return $element['#children'];
}

function theme_imagefield_widget_file(&$element) {
  return $element['#children'];
}


