<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  Link general views functionalities to services module.
 */

/**
 * Get an actual view from the database.
 *
 * @param view_name
 *   String. The views name.
 * @param display_id
 *   String (optional).
 * @param args
 *   Array (optional). A list of params.
 * @return
 *   Array. The views return.
 */
function views_service_get_actualview($view_name, $display_id = NULL, $args = array()) {
  if ($display_id === NULL or trim($display_id) === '') {
    $display_id = 'default';
  }
  $view = views_get_view($view_name);
  if (is_null($view)) {
    return services_error('View does not exist.');
  }
  // Check access
  if (!views_access($view)) {
    return services_error('You do not have access to this view.');
  }
  $view->set_arguments($args);
  $view->execute();
  foreach ($view->result as $node) {
    $new_node = new stdClass;
    $view_fields = $view->display[$display_id]->handler->handlers['field'];
    foreach ($view_fields as $f_key => $f_val) {
      $new_node->{$f_key} = $f_val->render($node);
    }
    $nodes[] = $new_node;
  }
  return $nodes;
}

/**
 * Get a view from the database.
 */
function views_service_get_view($view_name, $fields = array(), $args = array()) {
  $view = views_get_view($view_name);
  if (is_NULL($view)) {
    return services_error('View does not exist.');
  }

  // Check access
  if (!views_access($view)) {
    return services_error('You do not have access to this view.');
  }

  $view->set_arguments($args);
  $view->execute();

  foreach ($view->result as $node) {
    $nodes[] = services_node_load(node_load($node->nid), $fields);
  }
  return $nodes;
}

/**
 * Export a view.
 */
function views_service_export_view($view_name) {
  $view = views_get_view($view_name);
  if (is_NULL($view)) {
    return services_error('View does not exist.');
  }

  return $view->export();
}

/**
 * Import a view.
 */
function views_service_import_view($view_export) {
  views_include('view');
  ob_start();
  eval($view_export);
  ob_end_clean();

  if (!is_object($view)) {
    return services_error(t('Unable to interpret view code.'));
  }

  if (empty($view->api_version) or $view->api_version < 2) {
    // Check for some value that would only exist on a Views 1 view.
    if (isset($view->url) or isset($view->page) or isset($view->block)) {
      views_include('convert');
      $view = views1_import($view);
      return services_error(t('You are importing a view created in Views version 1. You may need to adjust some parameters to work correctly in version 2.'));
    }
    else {
      return services_error(t('That view is not compatible with this version of Views.'));
    }
  }

  $existing_view = views_get_view($view->name);
  if ($existing_view and $existing_view->type != t('Default')) {
    $view->vid = $existing_view->vid;
  }

  $view->save();
  return $view->vid;
}
