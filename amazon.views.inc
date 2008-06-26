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
 * Implementation of hook_views_plugins
 */
function amazon_views_plugins() {
  return array(
    'module' => 'amazon', // This just tells our themes are elsewhere.
    'row' => array(
      'amazon_item' => array(
        'title' => t('Amazon item'),
        'help' => t('Display the item as a fully themed Amazon product.'),
        'handler' => 'amazon_views_plugin_row_amazon_view',
        'theme' => 'amazon_views_view_row_item',
        'base' => array('amazon_item'), // only works with 'node' as base.
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data()
 */

/**
 * Plugin which performs a node_view on the resulting object.
 *
 * Most of the code on this object is in the theme function.
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


function amazon_views_data() {
  // Basic table information.

  // ----------------------------------------------------------------
  // Core amazon_item table

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['amazon_item']['table']['group']  = t('Amazon');

  // Advertise this table as a possible base table
  $data['amazon_item']['table']['base'] = array(
    'field' => 'asin',
    'title' => t('Amazon item'),
    'help' => t('The Amazon item type allows views of Amazon products saved on your site.'),
    'weight' => 10,
  );

  // ----------------------------------------------------------------
  // Fields
  _amazon_make_simple_text_field($data, 'amazon_item', 'asin', 'ASIN', 'The product ID of the item.');

  // title
  $data['amazon_item']['title'] = array(
    'title' => t('Title'), // The item it appears as on the UI,
    'help' => t('The name of the product.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'title', // the real field
      'handler' => 'views_handler_field_amazon_title',
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
      'click sortable' => FALSE,
     ),
  );

  _amazon_make_simple_number_field($data, 'amazon_item', 'salesrank', 'Sales rank', 'The current sales ranking of the product on Amazon.com.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'publisher', 'Publisher', 'The publisher of the product.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'manufacturer', 'Manufacturer', 'The manufacturer of the product.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'mpn', "Part number", "The Manufacturer's own part number for the product.");
  _amazon_make_simple_text_field($data, 'amazon_item', 'studio', 'Studio', 'The name of the studio that published the product.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'label', 'Label', 'The name of the label that published the product.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'binding', 'Binding', 'The style of binding used for the product.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'productgroup', 'Product group', 'The Amazon grouping the product is categorized in.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'producttypename', 'Product type name', 'The Amazon internal product-type code for the product.');

  // We probably want to convert this to a real date field.
  _amazon_make_simple_text_field($data, 'amazon_item', 'releasedate', 'Release date', 'The release date of the product.');
  _amazon_make_simple_text_field($data, 'amazon_item', 'listpriceformattedprice', 'List price', 'The current list price of the item. (Unformatted)');
  unset($data['amazon_item']['listpriceformattedprice']['argument']);

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['amazon_item_participant']['table']['group']  = t('Amazon');

  $data['amazon_item_participant']['table']['join'] = array(
    // participant links to amazon_item directly via asin.
    'amazon_item' => array(
      'left_field' => 'asin',
      'field' => 'asin',
    ),
  );

  _amazon_make_simple_text_field($data, 'amazon_item_participant', 'participant', 'Participant name', 'The name of an individual who participated in the creation of a product.');
  _amazon_make_simple_text_field($data, 'amazon_item_participant', 'type', 'Participant role', 'The role the participant had in creating the product (author, artist, etc).');
  //$data['amazon_item_participant']['participant']['argument']['handler'] = 'views_handler_argument_many_to_one';
  unset($data['amazon_item_participant']['type']['argument']);

  // participants_all
  $data['amazon_item_participant']['participants_all'] = array(
    'title' => t('All participants'), // The item it appears as on the UI,
    'help' => t("The names of all individuals who participated in the creation of a product, grouped as a single field."), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'participant', // the real field
      'handler' => 'views_handler_field_amazon_participant',
      'click sortable' => FALSE,
     ),
  );


  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['amazon_item_image']['table']['group']  = t('Amazon');

  $data['amazon_item_image']['table']['join'] = array(
    // participant links to amazon_item directly via asin.
    'amazon_item' => array(
      'left_field' => 'asin',
      'field' => 'asin',
    ),
  );

  // image
  $data['amazon_item_image']['product_image'] = array(
    'title' => t('Product image'), // The item it appears as on the UI,
    'help' => t("A photo of the Amazon product in question."), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'url', // the real field
      'handler' => 'views_handler_field_amazon_image',
      'click sortable' => FALSE,
     ),
  );

  return $data;
}


function _amazon_make_simple_text_field(&$data, $table, $column, $title, $help = '') {
  $data[$table][$column] = array(
    'title' => t($title), // The item it appears as on the UI,
    'help' => t($help), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => $column, // the real field
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'argument' => array('handler' => 'views_handler_argument_string'),
    'filter' => array('handler' => 'views_handler_filter_string'),
    'sort' => array('handler' => 'views_handler_sort'),
  );
}

function _amazon_make_simple_number_field(&$data, $table, $column, $title, $help = '') {
  $data[$table][$column] = array(
    'title' => t($title), // The item it appears as on the UI,
    'help' => t($help), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => $column, // the real field
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
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

/**
 * @}
 */
