<?php
// $Id$


/**
 * Field handler to display the current priority of an issue.
 */
class project_issue_handler_field_issue_priority extends views_handler_field {
  function render($values) {
    require_once drupal_get_path('module', 'project_issue') .'/issue.inc';
    return check_plain(project_issue_priority($values->{$this->field_alias}));
  }
}

