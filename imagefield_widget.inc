<?php
// $Id$

/**
 * @file
 * ImageField widget hooks and callbacks.
 *
 * This file is loaded by fileifeld.module and the callbacks it contains are
 * invoked by filefield.module. This happens because imagefield delegates it's
 * CCK hooks and callbacks to filefield in imagefield.module.
 */

/**
 * @defgroup "FileField widget settings callbacks"
 * @{
 *
 * These callbacks are invoked by filefield_widget_settings. Their
 * return values are merged with and will override filefields' widget
 * settings callback's return values.
 */
function imagefield_widget_settings_form($widget) {
  $form = module_invoke('filefield', 'widget_settings', 'form', $widget);

  $form['max_resolution'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum resolution for Images'),
    '#default_value' => !empty($widget['max_resolution']) ? $widget['max_resolution'] : 0,
    '#size' => 15,
    '#maxlength' => 10,
    '#description' => t('The maximum allowed image size expressed as WIDTHxHEIGHT (e.g. 640x480). Set to 0 for no restriction. If a larger image is uploaded, it will be resized to reflect the given width and height.'),
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
    '#title' => t('Permitted upload file extensions'),
    '#default_value' => !empty($widget['file_extensions']) ? $widget['file_extensions'] : 'jpg jpeg png gif',
    '#size' => 64,
    '#maxlength' => 64,
    '#description' => t('Extensions a user can upload to this field. Separate extensions with a space and do not include the leading dot. Only jpg, png, and gif images are supported with this widget.'),
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

/**
 * Validate callback for the CCK widget form.
 */
function imagefield_widget_settings_validate($widget) {
  // Check that only web images are specified in the callback.
  $extensions = array_filter(explode(' ', $widget['file_extensions']));
  $web_extensions = array('jpg', 'jpeg', 'gif', 'png');
  if (count(array_diff($extensions, $web_extensions))) {
    form_set_error('file_extensions', t('Only web-standard images (jpg, gif, and png) are supported through the image widget. If needing to upload other types of images, change the widget to use a standard file upload.'));
  }
}

/**
 * Save callback for the CCK widget form.
 */
function imagefield_widget_settings_save($widget) {
  // TODO: Rename custom_alt and custom_title to alt_custom and title_custom.
  $filefield_settings = module_invoke('filefield', 'widget_settings', 'save', $widget);
  return array_merge($filefield_settings, array('max_resolution', 'min_resolution', 'alt',  'custom_alt', 'title', 'custom_title'));
}

/**
 * Element #value_callback function.
 */
function imagefield_widget_value($element, $edit = FALSE) {
  $item = filefield_widget_value($element, $edit);
  if ($edit) {
    $item['alt'] = isset($edit['alt']) ? $edit['alt'] : '';
    $item['title'] = isset($edit['title']) ? $edit['title'] : '';
  }
  else {
    $item['alt'] = '';
    $item['title'] = '';
  }
  return $item;
}

/**
 * Element #process callback function.
 */
function imagefield_widget_process($element, $edit, &$form_state, $form) {
  $file = $element['#value'];
  $field = content_fields($element['#field_name'], $element['#type_name']);

  $element['#theme'] = 'imagefield_widget_item';

  $element['data']['alt'] = array(
    '#title' => t('Alternate Text'),
    '#type' => 'value',
    '#default_value' => $field['widget']['alt'],
  );
  if ($field['widget']['custom_alt']) {
    $element['data']['alt']['#type'] = $file['fid'] ? 'textfield' : 'hidden';
    if (!empty($file['data']['alt'])) $element['data']['alt']['#default_value'] =  $file['data']['alt'];
  }

  $element['data']['title'] = array(
    '#type' => 'value',
    '#title' => t('Title'),
    '#default_value' => $field['widget']['title'],
  );
  if ($field['widget']['custom_title']) {
    $element['data']['title']['#type'] = $file['fid'] ? 'textfield' : 'hidden';
    if (!empty($file['data']['title'])) $element['data']['title']['#value'] =  $file['data']['title'];
  }
  return $element;
}

/**
 * FormAPI theme function. Theme the output of an image field.
 */
function theme_imagefield_widget(&$element) {
  return theme('form_element', $element, $element['#children']);
}
