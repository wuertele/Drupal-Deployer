<?php

function ifeeldirty_theme() {
  return array(
    'node_submitted' => array(
      'arguments' => array('node' => NULL),
    ),
    'comment_form' => array(
      'arguments' => array('form' => NULL),
    ),
  );
}

function ifeeldirty_node_submitted($node) {
  return t('This entry was posted on @datetime', array('@datetime' => format_date($node->created)));
}

function ifeeldirty_preprocess_node(&$variables) {
  // Build the custom metadata
  $meta = $variables['submitted'];
  if ($variables['terms']) {
    if ($meta) {
      $meta .= ' '. t('and is filed under !terms.', array('!terms' => $terms));
    }
    else {
      $meta .= ' '. t('This entry is filed under !terms.', array('!terms' => $terms));
    }
  }
  if (module_exists('comment_rss')) {
    $meta .= ' '. t('You can follow any responses to this entry through the !comment_feed  feed', array('!comment_feed' => l('', '')));
  }
  $variables['metadata'] = $meta;
}

function ifeeldirty_preprocess_comment(&$variables) {
  static $comment_counter;
  if (!isset($comment_counter)) {
    $comment_counter = 1;
  }
  $variables['first'] = $comment_counter == 1;
  $variables['last'] = $comment_counter == $variables['node']->comment_count;

  $variables['comment_zebra'] = ($comment_counter % 2) ? 'odd' : 'even';
  $comment_counter++;

  $variables['moderation_message'] = ($variables['status'] == 'comment-published') ? NULL : t('Your comment is awaiting moderation.');
}

function ifeeldirty_preprocess_comment_wrapper(&$variables) {
  if ($variables['node']->comment_count) {
    $variables['comment_responses'] = format_plural($variables['node']->comment_count, 'One response', '@count responses');
  }
  else {
    $variables['comment_responses'] = t('No responses');
  }
}

function ifeeldirty_comment_form(&$form) {
  $preview = empty($form['comment_preview']) ? '' : drupal_render($form['comment_preview']);
  $form['comment_preview_below']['#access'] = FALSE;

  $output .= '<div class="cfbox2">';
  $output .= drupal_render($form['comment_filter']['filter']);
  $output .= drupal_render($form['comment_filter']);
  $output .= '</div>';

  $output .= '<div class="cfbox3">';
  if (!empty($form['submit'])) {
    $output .= drupal_render($form['submit']);
  }
  if (!empty($form['preview'])) {
    $output .= drupal_render($form['preview']);
  }
  $output .= '</div>';

  $output = '<div class="cfbox1">'. drupal_render($form) .'</div>'. $output;

  return '<div class="commentform">'. $preview . $output .'</div>';
}
