<?php

/**
 * Suitability callback the image widget and formatter.
 * Handles the three web mimetypes: jpeg, png and gif.
 */
function imagefield_handles_file($file) {
  return in_array($file->filemime, array(
    'image/jpeg', 'image/png', 'image/gif'
  ));
}

/**
 * Implementation of filefield's hook_file_widget_info().
 */
function imagefield_file_widget_info() {
  $module_path = drupal_get_path('module', 'filefield_image');

  return array(
    'default' => array(
      'form element' => 'imagefield_filefield_edit',
      'suitability callback' => 'imagefield_handles_file',
      'css' => array($module_path .'/imagefield.css'),
      'title' => t('Images (png/jpeg/gif)'),
      'description' => t('An edit widget for image files, including a preview of the image.'),
    ),
  );
}

/**
 * The 'process' callback for 'filefield_image_edit' form elements.
 * Called after defining the form and while building it, transforms the
 * barebone element array into an preview and and a text field for editing
 * the image properties (.
 */
function imagefield_filefield_edit_process($element, $edit, &$form_state, $form) {
  $field = $element['#field'];
  $delta = $element['#delta'];
  $file = $element['#file'];
  $url = file_create_url($file->filepath);

  $prefix = isset($element['#prefix']) ? $element['#prefix'] : '';
  $suffix = isset($element['#suffix']) ? $element['#suffix'] : '';
  $element['#prefix'] = $prefix .'<div class="filefield-image-edit">';
  $element['#suffix'] = '</div>'. $suffix;

  $title = isset($file->title) ? $file->title : '';

  $element['preview'] = array(
    '#type' => 'markup',
    '#value' => theme('imagefield_filefield', $file, array('width' => '150'), FALSE),
  );
  $element['info'] = array(
    '#type' => 'item',
    '#description' => t('Size: !size. Filename: !link', array(
      '!size' => format_size($file->filesize),
      '!link' => l($file->filename, $url),
    )),
    '#prefix' => '<div class="imagefield-filefield-edit-info imagefield-filefield-edit-control">',
    '#suffix' => '</div>',
  );
  $element['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Alternate text to be displayed if the image cannot be displayed.'),
    '#default_value' => empty($file->description)
                        ? $file->filename
                        : $file->description,
    '#maxlength' => 256,
    '#required' => TRUE,
    '#prefix' => '<div class="imagefield-filefield-edit-description imagefield-filefield-edit-control">',
    '#suffix' => '</div>',
  );
  return $element;
}


/**
 * Implementation of filefield's hook_file_formatter_info().
 */
function imagefield_filefield_file_formatter_info() {
  $module_path = drupal_get_path('module', 'imagefield');

  return array(
    'default' => array(
      'suitability callback' => 'imagefield_handles_file',
      'css' => array($module_path .'/imagefield.css'),
      'title' => t('Images (png/jpeg/gif, original size)'),
      'description' => t('Displays image files in their original size.'),
    ),
  );
}

/**
 * Theme function for the 'filefield_image_edit' form element.
 */
function theme_imagefield_filefield_edit($element) {
  return theme('form_element', $element, $element['#children']);
}

/**
 * Theme function for the single image file formatter.
 */
function theme_imagefield_filefield_file_formatter_default($file, $field, $file_formatter_settings) {
  return theme('imagefield_filefield', $file);
}

/**
 * Theme function for displaying an file as \<img\> tag, optionally with
 * extra attributes for the tag and automatic size recognition.
 */
function theme_imagefield_filefield($file, $attributes = array(), $getsize = TRUE) {
  if ($getsize) {
    list($width, $height, $type, $image_attributes) = @getimagesize($file->filepath);
  }
  if (empty($image_attributes)) {
    if (!is_file($file->filepath)) {
      return '';
    }
    $image_attributes = '';
  }
  $description = empty($file->description) ? $file->filename : $file->description;

  $attributes = array_merge($attributes, array(
    'alt' => $description,
    'title' => empty($file->title) ? $description : $file->title,
  ));
  $attributes = $image_attributes .' '. drupal_attributes($attributes);
  $url = file_create_url($file->filepath);

  return '<div class="filefield-image">'.
    '<img src="'. check_url($url) .'" '. $attributes .' /></div>';
}

