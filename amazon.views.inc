<?php
// $Id$
/**
 * @file
 * Provide views data and handlers for amazon.module
 */

/**
 * @defgroup views_amazon_module amazon.module handlers
 *
 * Includes the tables 'amazon_item', 'amazon_item_participant',
 * 'amazon_item_image', 'amazon_item_editorial_review', and 'amazon_item_node'.
 * @{
 */

/**
 * Implementation of hook_views_data()
 */
function amazon_views_data() {
  // Basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['amazon_item_node']['table']['group']  = t('Node');

  // For other base tables, explain how we join
  $data['amazon_item_node']['table']['join'] = array(
    'amazon_item' => array(
      'handler' => 'views_join', // this is actually optional
      'arguments' => array('amazon_item_node', 'amazon_item', 'asin', 'asin'),
     ),
    'node' => array(
      'handler' => 'views_join', // this is actually optional
      'arguments' => array('amazon_item_node', 'node', 'vid', 'vid'),
     ),
  );

  // ----------------------------------------------------------------
  // Fields

  // delta
  $data['amazon_item_node']['module'] = array(
    'title' => t('Amazon item source'), // The item it appears as on the UI,
    'help' => t('The module that associated a product with the node.'), // The help that appears on the UI,
    // Information for accepting a nid as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_amazon_node_module',
    ),
  );


  // ----------------------------------------------------------------
  // Core amazon_item table


  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['amazon_item']['table']['group']  = t('Amazon item');

  // Advertise this table as a possible base table
  $data['amazon_item']['table']['base'] = array(
    'field' => 'asin',
    'title' => t('Amazon item'),
    'help' => t('The Amazon item type allows views of Amazon products saved on your site.'),
    'weight' => 10,
  );

  // ----------------------------------------------------------------
  // Fields
  _amazon_make_simple_text_field($data, 'asin', 'ASIN', 'The product ID of the item.');

  // title
  $data['amazon_item']['title'] = array(
    'title' => t('Title'), // The item it appears as on the UI,
    'help' => t('The name of the product.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'title', // the real field
      'handler' => 'views_handler_field_amazon_title',
      'group' => t('Amazon item'), // The group it appears in on the UI,
      'click sortable' => TRUE,
     ),
  );

  // detailpageurl
  $data['amazon_item']['detailpageurl'] = array(
    'title' => t('Detail page URL'), // The item it appears as on the UI,
    'help' => t("The URL of the product's page on Amazon.com."), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'detailpageurl', // the real field
      'handler' => 'views_handler_field',
      'group' => t('Amazon item'), // The group it appears in on the UI,
      'click sortable' => FALSE,
     ),
  );

  _amazon_make_simple_number_field($data, 'salesrank', 'Sales rank', 'The current sales ranking of the product on Amazon.com.');
  _amazon_make_simple_text_field($data, 'publisher', 'Publisher', 'The publisher of the product.');
  _amazon_make_simple_text_field($data, 'manufacturer', 'Manufacturer', 'The manufacturer of the product.');
  _amazon_make_simple_text_field($data, 'studio', 'Studio', 'The name of the studio that published the product.');
  _amazon_make_simple_text_field($data, 'label', 'Label', 'The name of the label that published the product.');
  _amazon_make_simple_text_field($data, 'binding', 'Binding', 'The style of binding used for the product.');
  _amazon_make_simple_text_field($data, 'releasedate', 'Release date', 'The release date of the product.');
  _amazon_make_simple_text_field($data, 'productgroup', 'Product group', 'The Amazon grouping the product is categorized in.');
  _amazon_make_simple_text_field($data, 'producttypename', 'Product type name', 'The Amazon internal product-type code for the product.');

  return $data;
}


function _amazon_make_simple_text_field(&$data, $column, $title, $help = '') {
  $data['amazon_item'][$column] = array(
    'title' => t($title), // The item it appears as on the UI,
    'help' => t($help), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => $column, // the real field
      'handler' => 'views_handler_field',
      'group' => t('Amazon item'), // The group it appears in on the UI,
      'click sortable' => TRUE,
     ),
    'filter' => array('handler' => 'views_handler_filter_string'),
    'sort' => array('handler' => 'views_handler_sort'),
  );
}

function _amazon_make_simple_number_field(&$data, $column, $title, $help = '') {
  $data['amazon_item'][$column] = array(
    'title' => t($title), // The item it appears as on the UI,
    'help' => t($help), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => $column, // the real field
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
      'group' => t('Amazon item'), // The group it appears in on the UI,
     ),
    'filter' => array('handler' => 'views_handler_filter_numeric'),
    'sort' => array('handler' => 'views_handler_sort'),
  );
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
      '#title' => t('Display the title as'),
      '#type' => 'radios',
      '#options' => array(
        'plain' => t('Plain text'),
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

/**
 * @}
 */
