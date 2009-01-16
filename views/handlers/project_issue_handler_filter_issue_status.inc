<?php
// $Id$


/**
 * Filter issues based on their status.
 */
class project_issue_handler_filter_issue_status extends views_handler_filter_in_operator {
  function get_value_options() {
    require_once drupal_get_path('module', 'project_issue') .'/issue.inc';
    $this->value_options = project_issue_state();
  }
}

