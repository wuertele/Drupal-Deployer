<?php

/**
 * @file
 * ImageField widget hooks and callbacks.
 */

/**
 * Implementation of hook_widget_info().
 */
function imagefield_widget_info() {
  $module_path = drupal_get_path('module', 'imagefield');
  return array(
    'imagefield_widget' => array(
      'label' => t('Image'),
      'field types' => array('image', 'filefield'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'callbacks' => array('default value' => CONTENT_CALLBACK_CUSTOM),
      'suitability callback' => 'imagefield_handles_file',
      'css' => array($module_path .'/imagefield.css'),
      'description' => t('An edit widget for image files, including a preview of the image.'),
    ),
  );
}

/**
 * Implementation of hook_widget_settings().
 */
function imagefield_widget_settings($op, $widget) {
  // filefield wiget settings will call the filefield callbacks
  // then merge the wiget callback values,
  // widget callbacks are in the form of 
  // $widget['type'] .'_settings_'. $op.
  return filefield_widget_settings($op, $widget);
}

function imagefield_widget_widget_settings_form($widget) {
  $form = array();
  $form['max_resolution'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum resolution for Images'),
    '#default_value' => !empty($widget['max_resolution']) ? $widget['max_resolution'] : 0,
    '#size' => 15,
    '#maxlength' => 10,
    '#description' =>
    t('The maximum allowed image size expressed as WIDTHxHEIGHT (e.g. 640x480). Set to 0 for no restriction. If a larger image is uploaded, it will be resized to reflect the given width and height.'),
    '#weight' => 2,
  );
  $form['min_resolution'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum resolution for Images'),
    '#default_value' => !empty($widget['min_resolution']) ? $widget['min_resolution'] : 0,
    '#size' => 15,
    '#maxlength' => 10,
    '#description' =>
    t('The minimum allowed image size expressed as WIDTHxHEIGHT (e.g. 640x480). Set to 0 for no restriction. If an image that is smaller than these dimensions is uploaded it will be rejected.'),
    '#weight' => 2,
  );
 
  $form['file_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Permitted upload file extensions.'),
    '#default_value' => !empty($widget['file_extensions']) ? $widget['file_extensions'] : 'jpg jpeg png gif',
    '#size' => 64,
    '#maxlength' => 64,
    '#description' => t('Extensions a user can upload to this field. Seperate extensions with a space and do not include the leading dot.'),
    '#weight' => 2,
  );

  $form['alt_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('ALT text settings'),
    '#collapsible' => true,
    '#collapsed' => true,
    '#weight' => 8,
  );
  $form['alt_settings']['custom_alt'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable custom alternate text'),
    '#default_value' =>  !empty($widget['custom_alt']) ? $widget['custom_alt'] : 0,
    '#description' => t('Enable user input alternate text for images.'),
  );

  $form['alt_settings']['alt'] = array(
    '#type' => 'textfield',
    '#title' => t('Default ALT text'),
    '#default_value' => !empty($widget['alt']) ? $widget['alt'] : '',
    '#description' => t('This value will be used for alternate text by default.'),
    '#suffix' => theme('token_help', 'file'),
  );
  
  $form['title_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Title text settings'),
    '#collapsible' => true,
    '#collapsed' => true,
    '#weight' => 8,
  );
  $form['title_settings']['custom_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable custom title text'),
    '#default_value' =>  !empty($widget['custom_title']) ? $widget['custom_title'] : 0,
    '#description' => t('Enable user input title text for images.'),
  );

 
  $form['title_settings']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Title text'),
    '#default_value' => !empty($widget['title']) ? $widget['title'] : '',
    '#description' => t('This value will be used as the image title by default.'),
    '#suffix' =>  theme('token_help', 'file'),
  );
  return $form;
}

function imagefield_widget_widget_settings_save($widget) {
  //@todo: rename custom_alt and custom_title to alt_custom and title_custom to be OCD.
  return array('max_resolution', 'min_resolution', 'alt',  'custom_alt', 'title', 'custom_title');
}


/**
 * Implementation of hook_form_alter(). Set the appropriate
 * attibutes to allow file uploads on the field settings form.
 */
function _imagefield_content_admin_field_form_alter(&$form, $form_state, $form_id) {
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
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
  // add default values to items.
  if (empty($items[$delta])) { 
    $items[$delta] = array('alt' => '', 'title' => '');
  }
  return filefield_widget(&$form, &$form_state, $field, $items, $delta = 0);
}

function imagefield_widget_value($element, $edit = FALSE) {
  $item = filefield_widget_value($element, $edit);
  // handle additional 
  if ($edit) { 
    $item = array_merge( 
      $item, 
      array(
        'alt' => $edit['alt'],
        'title' => $edit['title'],
      )
    );
  }
  return $item;
}

function imagefield_widget_process($element, $edit, &$form_state, $form) {
  //dsm('imagefield_widget_process');
  //dsm($element);
  //dsm($form_state);

  $file = $element['#value'];
  $delta = $element['#delta'];
  $field_name = $element['#field_name'];

  
  $element['#theme'] = 'imagefield_row';
  $element['fid'] = array('#type' => 'hidden', '#value' =>  $file['fid']);

  if ($file['fid'] != 0) {
    $element['preview'] = array('#type' => 'markup', '#value' => theme('imagefield_admin_thumbnail', $file));  
    if (empty($file['filepath'])) {
      //dsm($file);
    }
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
      '#name' => $field_name .'_'. $delta .'_remove_btn',
      '#type' => 'submit', 
      '#value' => t('Remove'),
      '#submit' => array('imagefield_widget_submit_remove_btn'),
      '#field_name' => $field_name,
      '#delta' => $delta,
    );
  }

  else if ($file['fid'] == 0) {
    $element['alt']['#type'] = 'hidden';
    $element['title']['#type'] = 'hidden';

    $element['upload'] = array(
      '#name' => 'files['. $field_name .'_'. $delta .']',
      '#type' => 'file',
      '#title' => t('New Upload'),
    );

    $element['upload_btn'] = array(
      '#name' => $field_name .'_'. $delta .'_upload_btn',
      '#type' => 'submit', 
      '#value' => t('Upload'),
      '#submit' => array('imagefield_widget_submit_upload_btn'),
      '#field_name' => $field_name,
      '#delta' => $delta,
    );
  }
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
  $form_state['rebuild'] = true;
}

function imagefield_widget_submit_upload_btn($form, &$form_state) {
  //dsm('imagefield_widget_submit_upload_btn');
  //dsm($form);
  //dsm($form_state);
  $field_name = $form_state['clicked_button']['#field_name'];
  $delta = $form_state['clicked_button']['#delta'];

  $field = content_fields($field_name);

  $widget_file_path = $field['widget']['file_path'];
  if (module_exists('token')) {
    global $user;
    $widget_file_path = token_replace($widget_file_path, 'user', $user);
  }

  $complete_file_path = file_directory_path() .'/'. $widget_file_path;
  $upload_name = $field_name .'_'. $delta;
  if (!field_file_check_directory($complete_file_path, FILE_CREATE_DIRECTORY)) {
    watchdog('imagefield', 'The upload directory %directory for the file field %field (content type %type) could not be created or is not accessible. A newly uploaded file could not be saved in this directory as a consequence, and the upload was canceled.', array('%directory' => $widget_file_path, '%field' => $field_name, '%type' => $field['type_name']));
    form_set_error($upload_name, t('The file could not be uploaded.'));
    return $file;
  }
  
  $validators = array(
    'file_validate_extensions' => array(isset($field['widget']['file_extensions']) ? $field['widget']['file_extensions'] : 0),
    'file_validate_size' => array(isset($field['widget']['max_filesize']) ? $field['widget']['max_filesize'] : 0),
    'file_validate_image_resolution' => array(
      isset($field['widget']['max_resolution']) ? $field['widget']['max_resolution'] : 0,
      isset($field['widget']['min_resolution']) ? $field['widget']['min_resolution'] : 0,
    ),
  );

  if (!$file = field_file_save_upload($upload_name, $validators, $complete_file_path)) {
    watchdog('imagefield', 'The file upload failed. %upload', array('%upload' => $upload_name));
    form_set_error($upload_name, t('The Image upload failed.'));
    return;
  }
  //dsm($file);
  $form_state['values'][$field_name][$delta]['fid'] = $file['fid'];
  $form_state['rebuild'] = true;
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
    $form_state['rebuild'] = true;
  } 
  else {
    // load fid from path..
    $result = db_query("SELECT fid FROM {files} WHERE filepath='%s'", $reuse);
    if ($fid = db_result($result)) {
      $form_state['values'][$field_name][$delta]['fid'] = $fid;
      $form_state['rebuild'] = true;
    }
  }
}

function imagefield_widget_after_build($element) {
  //dsm('imagefield_widget_after_build');
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
  //dsm('imagefield_validate');
  //dsm($element);
}



/**
 * FormAPI theme function. Theme the output of an image field.
 */
function theme_imagefield_widget(&$element) {
  return $element['#children'];
}



