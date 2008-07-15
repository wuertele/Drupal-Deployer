<?php

/**
 * @file
 * ImageField widget hooks and callbacks.
 */

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
 * Imagefield widget callbacks, called by filefield.module.
 */
function imagefield_widget_widget_value($element, $edit = FALSE) {
  // handle additional 
  if ($edit) { 
    $item = array_merge( 
      $item, 
      array(
        'alt' => isset($edit['alt']) ? $edit['alt'] : '',
        'title' => isset($edit['title']) ? $edit['title'] : '',
      )
    );
  }
  return $item;
}

function imagefield_widget_widget_process($element, $edit, &$form_state, $form) {
  $file = $element['#value'];
  $field = content_fields($element['#field_name'], $element['#type_name']);

  $element['alt'] = array(
    '#type' => 'value',
    '#value' => $field['widget']['alt'],
  );
  $element['title'] = array(
    '#type' => 'value',
    '#value' => $field['widget']['title'],
  );

  if ($field['widget']['custom_alt']) {
    $element['alt'] = array(
      '#type' => $file['fid'] ? 'textfield' : 'hidden',
      '#title' => t('Alternate Text'),
      '#value' => empty($edit['alt']) ? $file['alt'] : $edit['alt'],
    );
  } 

  if ($field['widget']['custom_title']) {
    $element['title'] = array(
      '#type' => $file['fid'] ? 'textfield' : 'hidden',
      '#title' => t('Title'),
      '#value' => empty($edit['title']) ? $file['title'] : $edit['title'],
    );
  }
  return $element;
}


/**
 * FormAPI theme function. Theme the output of an image field.
 */
function theme_imagefield_widget(&$element) {
  return $element['#children'];
}



