<?php

/**
 * @file
 * Field Hooks and Callbacks for ImageField.
 */

/**
 * Implementation of hook_field_info().
 */
function imagefield_field_info() {
  return array(
    'image' => array(
      'label' => t('Image'),
      'description' => t('Store an image file and optionally text for alt and title tags.'),
    ),
  );
}

/**
 * Implementation of hook_field_settings().
 */
function imagefield_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      $form = array();
      $form['default'] = array(
        '#type' => 'fieldset',
        '#title' => t('Default image'),
        '#element_validate' => array('_imagefield_field_settings_default_validate'),
      );
      // Present a thumbnail of the current default image.
      $form['default']['use_default_image'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use default image'),
        '#default_value' =>  $field['use_default_image'],
        '#description' => t('Check here if you want to use a image as default.'),
      );
      if (!empty($field['default_image'])) {
        $form['default']['default_image_thumbnail'] = array(
          '#type' => 'markup',
          '#value' => theme('imagefield_image', $field['default_image'], '', '', array('width' => '150'), false),
        );
      }
      $form['default']['default_image_upload'] = array(
        '#type'  => 'file',
        '#title' => t('Upload image'),
        '#description' => t('Choose a image that will be used as default.'),
      );
      // We set this value on 'validate' so we can get cck to add it
      // as a standard field setting.
      $form['default_image'] = array(
        '#type' => 'value',
        '#value' => $field['default_image'],
      );
      return $form;

    case 'save':
      return array('default_image', 'use_default_image');

    case 'database columns':
      $columns = array(
        'fid' => array('type' => 'int', 'not null' => true, 'default' => '0'),
        'title' => array('type' => 'varchar', 'length' => 255, 'not null' => true, 'default' => '', 'sortable' => true),
        'alt' => array('type' => 'varchar', 'length' => 255, 'not null' => true, 'default' => '', 'sortable' => true),
        // widget specific data storage where widgets can stick some serialized foo per item.
        'data' => array('type' => 'text', 'serialize' => true),
      );
      return $columns;

    case 'filters':
      return array(
        'not null' => array(
          'operator' => array('=' => t('Has Image')),
          'list' => 'views_handler_operator_yesno',
          'list-type' => 'select',
          'handler' => 'imagefield_views_handler_filter_is_not_null',
        ),
      );
  }
}

/**
 * Element specific validation for imagefield default value.
 *
 * Validated in a separate function from imagefield_field() to get access
 * to the $form_state variable.
 */
function _imagefield_field_settings_default_validate($element, &$form_state) {
  // Verify the destination exists
  $dst = file_directory_path() .'/imagefield_default_images';
  if (!imagefield_check_directory($dst)) {
    form_set_error('default_image', t("The default image could not be uploaded. The destination(%d) does not exist or is not writable by the webserver.", array('%d' => dirname($dst))));
    return;
  }

  // We save the upload here because we can't know the correct path until the file is saved.
  if (!$file = file_save_upload('default_image_upload', array(), $dst)) {
    // no upload to save we hope... or file_save_upload reported an error on its own.
    return;
  }

  // set new value.
  $form_state['values']['default_image'] = (array)$file;

  // remove old image & clean up database.
  if (file_delete(file_create_path($field['default_image']['filepath']))) {
    db_query('DELETE FROM {files} WHERE fid=%d', $field['default_image']['fid']);
  }
}

/**
 * Custom filter for imagefield NOT null.
 */
function imagefield_views_handler_filter_is_not_null($op, $filter, $filterinfo, &$query) {
  if ($op == 'handler') {
    $query->ensure_table($filterinfo['table']);
    if ($filter['value']) {
      $qs = '%s.%s > 0';
      $query->add_where($qs, $filterinfo['table'], $filterinfo['field']);
    }
    else {
      $qs = '%s.%s = 0 OR %s.%s IS null';
      $query->add_where($qs, $filterinfo['table'], $filterinfo['field'], $filterinfo['table'], $filterinfo['field']);
    }
  }

}

/**
 * Implementation of hook_field().
 */
function imagefield_field($op, $node, $field, &$items, $teaser, $page) {
  return filefield_field($op, $node, $field, &$items, $teaser, $page);
}

/**
 * implementation of hook_content_is_empty
 *
 * the result of this determines whether content.module will save
 * the value of the field.
 */
function imagefield_content_is_empty($item, $field) {
  if (empty($item['fid'])) {
    return true;
  }
  else {
    return false;
  }  
}


