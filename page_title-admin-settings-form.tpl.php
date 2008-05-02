<?php
$rows = array();

foreach (element_children($form['title']) as $key) {
  $row = array();
  $row[] = drupal_render($form['title'][$key]);
  $row[] = drupal_render($form['pattern'][$key]);
  
  $rows[] = $row;
}

print theme('table', array('Page Type', 'Pattern'), $rows);

print drupal_render($form);
