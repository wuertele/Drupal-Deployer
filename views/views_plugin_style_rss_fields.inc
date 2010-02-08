<?php
// $Id$ 

/**
 * @file
 * Extending the view_plugin_style class to provide a kml view style.
 */

class views_plugin_style_rss_fields extends views_plugin_style {

  /**
   * Initialize plugin.
   *
   * Set feed image for shared rendering later.
   */
  function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options = NULL);
    $this->feed_image = drupal_get_path('module', 'views_kml') . '/images/kml.png';
  }
  
  /**
   * Attach this view to another display as a feed.
   *
   * Provide basic functionality for all export style views like attaching a
   * feed image link.
   */
  function attach_to($display_id, $path, $title) {
    $url_options = array('html' => true);
    $image = theme('image', $this->feed_image);
    $this->view->feed_icon .= l($image, $path, $url_options);
  }
  
  /**
   * Provide a form for setting options.
   *
   * @param array $form
   * @param array $form_state
   */
  function options_form(&$form, &$form_state) { 
    parent::options_form($form, $form_state);                  
    $options = parent::option_definition();
                           
    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#value' => t('You need at least one field before you can configure your field settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>',
      );
    }
    else {
      // Feed Description
      $form['description'] = array(
        '#type' => 'fieldset',
        '#title' => t('Feed Description'), 
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#weight' => -5,
      );
      $form['description']['feed_description'] = array(
        '#type' => 'textarea',
        '#default_value' => $this->options['description']['feed_description'],
        '#description' => t('Description for this feed.  If left blank, the default site mission will be used'),
      );
      // Field Chooser
      $field_names[$field] = array('' => '--');
      foreach ($handlers as $field => $handler) {
        if ($label = $handler->label()) {
          $field_names[$field] = $label;
        }
        else {
          $field_names[$field] = $handler->ui_name();
        }
      }
      $field_options = array(
        'title' => t('Title'),
        'description' => t('Description'),
        'link' => t('Link'),
        'author' => t('Author'),
        'category' => t('category'),
        'guid' => t('guid'),
        'pubDate' => t('pubDate'),
      );
      $form['fields'] = array(
        '#type' => 'fieldset',
        '#title' => 'Field usage',
        '#description' => t('Select the fields that relevant data for each element of the feed'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#weight' => 0,
      );
      foreach ($field_options as $k => $v) {
        $form['fields'][$k] = array(
          '#type' => 'select',
          '#title' => $v,
          '#options' => $field_names,
          '#default_value' => $this->options['fields'][$k],
        );
      }
      // GeoRSS
      $form['georss'] = array(
        '#type' => 'fieldset',
        '#title' => t('GeoRSS'), 
        '#description' => t('Select fields that provide the latitude and longitude values'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#weight' => 5,
      );
      $form['georss']['lat'] = array(
        '#type' => 'select',
        '#title' => t('Latitude'),
        '#options' => $field_names,
        '#default_value' => $this->options['georss']['lat'],
      ); 
      $form['georss']['lon'] = array(
        '#type' => 'select',
        '#title' => t('Longitude'),
        '#options' => $field_names,
        '#default_value' => $this->options['georss']['lon'],
      );
    }
  }
  
  function map_rows($rows) {
    // Fields must be rendered in order as of Views 2.3, so we will pre-render everything.
    $renders = array();
    $keys = array_keys($this->view->field);

    foreach ($rows as $count => $row) {
      foreach ($keys as $id) {
        $renders[$count][$id] = $this->view->field[$id]->theme($row);
      }
    }  

    $items = array();
    foreach ($renders as $id => $row) {
      $item = array();
      foreach (array_merge($this->options['fields'], $this->options['georss']) as $key => $val) {
        if (!isset($this->view->field[$val])) continue;
        $item[$key] = $row[$val];
      }
      $items[] = $item;
    }
    return $items;
  }
}
