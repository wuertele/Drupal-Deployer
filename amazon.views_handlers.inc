<?php
/* $Id$ */

/**
 * @file
 * Provide views handlers for amazon.module
 */


/**
 * Plugin which loads and themes an amazon item.
 *
 * @ingroup views_plugin_rows
 */
class amazon_views_plugin_row_amazon_view extends views_plugin_row {
  function option_definition() {
    $options = parent::option_definition();

    $options['display_format'] = array('default' => 'default');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['display_format'] = array(
      '#type' => 'select',
      '#title' => t('Display format'),
      '#options' => array(
        'default' => t('Default'),
        'details' => t('Details'),
        'thumbnail' => t('Thumbnail'),
        'inline' => t('Inline'),
      ),
      '#default_value' => $this->options['display_format'],
    );
  }
}

/**
 * Template helper for theme_amazon_views_view_row_item
 */
function template_preprocess_amazon_views_view_row_item(&$vars) {
  $options = $vars['options'];
  $vars['amazon_item'] = ''; // make sure var is defined.
  $asin = $vars['row']->asin;
  $items = amazon_item_lookup($asin);
  if (empty($items[$asin])) {
    return;
  }
  else {
    $item = $items[$asin];
    $item['view'] = $vars['view'];
    $vars['amazon_item'] = $item;
    if ($options['display_format'] == 'inline') {
      $vars['content'] = theme('amazon_item_inline', $item, $options['display_format']);
    }
    else {
      $vars['content'] = theme('amazon_item', $item, $options['display_format']);
    }
  }
}


/**
 Filter node-linked amazom products by source module
 */
class views_handler_filter_amazon_node_module extends views_handler_filter_in_operator {
  function construct() {
    parent::construct();
    $this->value_title = t('Source module');
    $options = array();
    $result = db_query("SELECT DISTINCT ain.module FROM {amazon_item_node} ain");
    while ($module = db_fetch_array($result)) {
      $options[$module['module']] = $module['module'];
    }
    $this->value_options = $options;
  }

  function options(&$options) {
    parent::options($options);
    $result = db_query("SELECT DISTINCT ain.module FROM {amazon_item_node} ain");
    $module = db_fetch_array($result);
    $options['value'] = array($module['module']);
  }
}

class views_handler_field_amazon_title extends views_handler_field {
  function options(&$options) {
    parent::options($options);
    $options['link_format'] = 'amazon';
  }

  /**
   * Override init function to provide generic option to link to node.
   */
  function init(&$view, &$data) {
    parent::init($view, $data);
    if (!empty($data['link_format']) && $data['link_format'] == 'amazon') {
      $this->additional_fields[] = 'detailpageurl';
    }
  }

  /**
   * Provide link to node option
   */
  function options_form(&$form, &$form_state) {
    $form['link_format'] = array(
      '#title' => t('Link behavior'),
      '#type' => 'radios',
      '#options' => array(
        'plain' => t('No link'),
        'amazon' => t("A link to the product's Amazon page"),
      ),
      '#default_value' => !empty($this->options['link_format']) ? $this->options['link_format'] : 'plain',
    );
    if ($this->view->base_table == 'node') {
      $form['link_format']['#options']['node'] = t('A link to the node the product is associated with');
    }
  }

  function render($values) {
    $title = check_plain($values->{$this->field_alias});
    switch($this->options['link_format']) {
      case 'plain':
        return $title;
        break;
      case 'node':
        return l($title, 'node/'. $values->nid, array('html' => TRUE));
        break;
      case 'amazon':
        return l($title, check_url($values->{$this->table_alias . '_detailpageurl'}), array('html' => TRUE));
        break;
    }
  }
}


class views_handler_field_amazon_image extends views_handler_field {
  function options(&$options) {
    parent::options($options);
    $options['image_size'] = 'smallimage';
    $options['link_format'] = 'amazon';
  }

  /**
   * Override init function to provide generic option to link to node.
   */
  function init(&$view, &$data) {
    parent::init($view, $data);
    if (!empty($data['image_size'])) {
      //$this->additional_fields[] = 'size';
      $this->additional_fields[] = 'height';
      $this->additional_fields[] = 'width';
    }
    if (!empty($data['link_format']) && $data['link_format'] == 'amazon') {
      $this->additional_fields['detailpageurl'] = array('table' => 'amazon_item', 'field' => 'detailpageurl');
    }
  }

  function ensure_my_table() {
    if (empty($this->table_alias)) {
      $join_extra = array();
      if (!empty($this->options['image_size'])) {
        $join_extra[] = array('field' => 'size', 'value' => $this->options['image_size'], 'numeric' => FALSE);
      }

      $join = new views_join();
      $join->construct($this->table, 'amazon_item', 'asin', 'asin', $join_extra);

      $this->table_alias = $this->query->ensure_table($this->table, $this->relationship, $join);
    }
    return $this->table_alias;
  }

  /**
   * Provide link to node option
   */
  function options_form(&$form, &$form_state) {
    $form['image_size'] = array(
      '#title' => t('Image size'),
      '#type' => 'select',
      '#options' => array(
        'smallimage' => t('Small'),
        'mediumimage' => t("Medium"),
        'largeimage' => t("Large"),
      ),
      '#default_value' => !empty($this->options['image_size']) ? $this->options['image_size'] : 'MedumImage',
    );

    $form['link_format'] = array(
      '#title' => t('Link behavior'),
      '#type' => 'radios',
      '#options' => array(
        'plain' => t('No link'),
        'amazon' => t("A link to the product's Amazon page"),
      ),
      '#default_value' => !empty($this->options['link_format']) ? $this->options['link_format'] : 'plain',
    );
    if ($this->view->base_table == 'node') {
      $form['link_format']['#options']['node'] = t('A link to the node the product is associated with');
    }
  }

  function render($values) {
    $attributes = array(
      'height' => $values->{$this->table_alias . '_height'},
      'width' => $values->{$this->table_alias . '_width'},
    );
    $image = theme('image', $values->{$this->table_alias . '_url'}, NULL, NULL, $attributes, FALSE);

    switch($this->options['link_format']) {
      case 'plain':
        return $image;
        break;
      case 'node':
        return l($image, 'node/'. $values->nid, array('html' => TRUE));
        break;
      case 'amazon':
        $urlfield = $this->additional_fields['detailpageurl']['field'];
        if (!empty($this->relationship)) {
          $urlfield = $this->relationship .'_'. $urlfield;
        }
        return l($image, $values->{$urlfield}, array('html' => TRUE));
        break;
    }
  }
}


class views_handler_field_amazon_participant extends views_handler_field_prerender_list {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['asin'] = array('table' => 'amazon_item', 'field' => 'asin');
  }

  /**
   * Provide meaningful defaults
   */
  function options(&$options) {
    parent::options($options);
    $options['participant_type'] = '**ALL**';
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['participant_type'] = array(
      '#type' => 'select',
      '#title' => t('Participant restriction'),
      '#options' => array(
        '**ALL**' => t('Show all particpants'),
        'author' => 'Author',
        'artist' => 'Artist',
        'actor' => 'Actor',
        'director' => 'Director',
        'creator' => 'Creator'
      ),
      '#default_value' => $this->options['participant_type'],
    );
  }

  function query() {
    $this->add_additional_fields();
  }

  function pre_render($values) {
    $this->field_alias = $this->aliases['asin'];
    $this->items = array();

    foreach ($values as $result) {
      if (!empty($result->{$this->aliases['asin']})) {
        $asins[] = $result->{$this->aliases['asin']};
      }
    }

    $sql = "SELECT aip.participant, aip.asin FROM {amazon_item_participant} aip WHERE aip.asin IN ('" . implode("', '", $asins) . "') ";
    if ($this->options['participant_type'] != '**ALL**') {
      $sql .= " AND aip.type = '%s'";
    }
    $results = db_query($sql, $this->options['participant_type']);

    while ($result = db_fetch_array($results)) {
      $this->items[$result['asin']][] = check_plain($result['participant']);
    }
  }
}
