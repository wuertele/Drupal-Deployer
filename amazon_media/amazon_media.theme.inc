<?php

function theme_amazon_item_inline__dvd($item) {
  $output = '<span class="'. _amazon_item_classes($item) .' amazon-item-inline">';
  $output .= l('<em>'. check_plain($item['title']) .'</em>', $item['detailpageurl'], array('html' => TRUE));
  $date = split('-', $item['theatricalreleasedate']);
  $output .= ' ('. check_plain($date[0]) .')</span>';
  return $output;
}

function theme_amazon_item__dvd($item) {
  $output = '<div class="'. _amazon_item_classes($item) .'">';
  $output .= theme('image', $item['imagesets']['smallimage']['url'], t('Cover image'), check_plain($item['title']), NULL, FALSE);
  $date = split('-', $item['theatricalreleasedate']);
  $output .= '<h3>'. l(check_plain($item['title']), $item['detailpageurl']) .' ('. check_plain($date[0]) .')</h3>';
  $output .= '<div><strong>'. t('Director') .':</strong> '. implode(', ', $item['director']) .'</div>';
  $output .= '<div><strong>'. t('Rating') .':</strong> '. $item['audiencerating'] .'</div>';
  $output .= '<div><strong>'. t('Running time') .':</strong> '. $item['runningtime'] .' '. t('minutes') .'</div>';
  $output .= '</div>';
  return $output;
}
