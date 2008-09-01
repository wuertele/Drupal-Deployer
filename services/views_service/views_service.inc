<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  Link general views functionalities to services module.
 */

/**
 * Get a view from the database.
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
function views_service_get($view_name, $fields = array(), $args = array(), $offset = 0, $limit = 0) {
  $view = views_get_view($view_name);

  // Put all arguments and then execute
  $view->set_arguments($args, FALSE);
  $view->set_offset($offset);
  $view->set_items_per_page($limit);
  $view->execute();

  return $view->result;
}

/**
 * Check the access permission to a given views.
 *
 * @param view_name
 *   String. The views name.
 * @return
 *   Boolean. TRUE if the user is allowed to load the given view.
 */
function views_service_get_access($view_name) {
  $view = views_get_view($view_name);
  if (empty($view)) {
    return FALSE;
  }

  global $user;
  return views_access($view);
}

/**
 * Export a view.
 *
 * @param view_name
 *   String. The views name.
 * @return
 *   Array. The view object.
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
 *
 * @param $view_export
 *   Array. The view object.
 * @return
 *   Number. The view ID.
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
