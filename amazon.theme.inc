<?php
// $Id$

function theme_amazon_inline_item($item) {
  $alt_theme = 'amazon_inline_item_' . _amazon_clean_type($item['producttypename']);
  if ($output = theme($alt_theme, $item)) {
    return $output;
  }
  else {
    $output = '<span class="'. _amazon_item_classes($item) .' amazon-item-inline">';
    $output .= l($item['title'], $item['detailpageurl']);
    $output .= '</span>';
    return $output;
  }
}
