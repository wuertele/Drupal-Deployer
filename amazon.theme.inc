<?php
// $Id$

function theme_amazon_item($item, $style = 'default') {
  $functions[] = 'amazon_item_'. $style .'__'. _amazon_clean_type($item['producttypename']);
  $functions[] = 'amazon_item__'. _amazon_clean_type($item['producttypename']);
  $functions[] = 'amazon_item_'. $style;
  $functions[] = 'amazon_item_default';
  drupal_add_css(drupal_get_path('module', 'amazon') .'/amazon.css');
  return theme($functions, $item);
}

function theme_amazon_item_default($item) {
  $output = '<div class="'. _amazon_item_classes($item) .'">';
  $output .= theme('image', $item['imagesets']['smallimage']['url'], t('Cover image'), check_plain($item['title']), NULL, FALSE);
  $output .= '<h3>'. l(check_plain($item['title']), $item['detailpageurl']) .'</h3>';
  $output .= '</div>';
  return $output;
}

function theme_amazon_item_full($item) {
  $output = '<div class="'. _amazon_item_classes($item) .' amazon-item-full">';
  $output .= theme('image', $item['imagesets']['mediumimage']['url'], t('Cover image'), check_plain($item['title']), NULL, FALSE);
  $output .= '<h3>'. l(check_plain($item['title']), $item['detailpageurl']) .'</h3>';
  $output .= '</div>';
  return $output;
}

function theme_amazon_item_inline($item, $style = 'default') {
  $output = '<span class="'. _amazon_item_classes($item) .' amazon-item-inline">';
  $output .= l(check_plain($item['title']), $item['detailpageurl'], array('html' => TRUE));
  $output .= '</span>';
  return $output;
}

function theme_amazon_item_thumbnail($item, $style = 'default') {
  $output = '<div class="'. _amazon_item_classes($item) .' amazon-item-thumbnail">';
  $output .= theme('image', $item['imagesets']['mediumimage']['url'], t('Cover image'), check_plain($item['title']), NULL, FALSE);
  $output .= '</div>';
  return $output;
}
