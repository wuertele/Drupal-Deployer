<?php

/**
 * @file
 * ImageField formatter hooks and callbacks.
 */

/**
 * Implementation of hook_field_formatter_info().
 */
function imagefield_field_formatter_info() {
  $formatters = array(
    'image_plain' => array(
      'label' => t('Default'),
      'field types' => array('image'),
    ),
    'image_nodelink' => array(
      'label' => t('link to node'),
      'field types' => array('image'),
    ),
    'image_imagelink' => array(
      'label' => t('link to image'),
      'field types' => array('image'),
    ),
    'path_plain' => array(
       'label' => t('path to image'),
       'field types' => array('image'),
    ),   
    'url_plain' => array(
       'label' => t('url to image'),
       'field types' => array('image'),
    ),   
  );
  return $formatters;
}

function theme_imagefield_formatter_image_plain($element) {
  $field = content_fields($element['#field_name']);
  $item = $element['#item'];

  if (empty($item['fid']) && $field['use_default_image']) $item = $field['default_image'];
  if (empty($item['filepath']))  $item = array_merge($item, field_file_load($item['fid']));
  
  $class = 'imagefield imagefield-'. $field['field_name'];
  return  theme('imagefield_image', $item, $item['alt'], $item['title'], array('class' => $class));
}

function theme_imagefield_formatter_image_nodelink($element) {
  $node = $element['#node'];
  $imagetag = theme('imagefield_formatter_image_plain', $element);
  $class = 'imagefield imagefield-nodelink imagefield-'. $element['#field_name'];
  return l($imagetag, 'node/'. $node->nid, array('class' => $class), null, null, false, true);
}

function theme_imagefield_formatter_image_imagelink($element) {
  $item = $element['#item'];
  $imagetag = theme('imagefield_formatter_image_plain', $element);
  $original_image_url = file_create_url($item['filepath']);
  $class = 'imagefield imagefield-imagelink imagefield-'. $element['#field_name'];
  return l($imagetag, $original_image_url, array('class' => $class), null, null, false, true);
}

function theme_imagefield_formatter_path_plain($element) {

  $field = content_fields($element['#field_name']);
  $item = $element['#item'];
  if (empty($item['fid']) && $field['use_default_image'])  $item = $field['default_image'];
  // If there is no image on the database, use default.
  if (empty($item['filepath']))  $item = array_merge($item, field_file_load($item['fid']));

  $attributes['class'] .= ' imagefield-formatter-path';
  return '<span '. drupal_attributes($attributes) .'>'. file_create_path($item['filepath']) .'</span>';
}

function theme_imagefield_formatter_url_plain($element) {
  $field = content_fields($element['#field_name']);
  $item = $element['#item'];
  if (empty($item['fid']) && $field['use_default_image'])  $item = $field['default_image'];
  // If there is no image on the database, use default.
  if (empty($item['filepath']))  $item = array_merge($item, field_file_load($item['fid']));

  $attributes['class'] .= ' imagefield-formatter-url';
  return '<span '. drupal_attributes($attributes) .'>'. file_create_url($item['filepath']) .'</span>';
}



