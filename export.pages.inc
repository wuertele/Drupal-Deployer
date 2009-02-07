<?php
// $Id$
// $Name$

/**
 * Menu callback to configure module settings.
 */
function export_settings() {

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );
  $form['basic']['export_method'] = array(
    '#type' => 'radios',
    '#title' => t('Method to use when importing a node'),
    '#options' => array('prepopulate' => t('Pre-populate the node form fields'), 'save-edit' => t('Save as a new node then edit')),
    '#default_value' => variable_get('export_method', 'prepopulate'),
  );
  
  $form['publishing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Should the publishing options ( e.g. published, promoted, etc) be reset to the defaults?'),
  );
  $types = node_get_types('names');

  foreach ($types as $type => $name) {
    $form['publishing']['export_reset_'. $type] = array(
      '#type' => 'checkbox',
      '#title' => t('@s: reset publishing options when exported', array('@s' => $name)),
      '#default_value' => variable_get('export_reset_'. $type, FALSE),
    );
  }
  // Need the variable default key to be something that's never a valid node type.
  $form['omit'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content types that are not to be exported - omitted due to incompatibility'),
  );
  $form['omit']['export_omitted'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Omitted content types'),
    '#default_value' => variable_get('export_omitted', array()),
    '#options' => $types,
    '#description' => t('Select any node types which should <em>never</em> be exported. Typically you should will want to select here all node types for which importing fails (e.g. image nodes).'),
  );

  return system_settings_form($form);
}

/**
 *  exports a node - prepopulate a node editing form
 *  set $return_code to TRUE to not return form but the code instead.
 */
function export_node_export($original_node, $return_code = FALSE) {
  if (isset($original_node->nid)) {
    global $user;

    if (export_is_permitted($original_node->type)) {
      $node = drupal_clone($original_node);

      $node->nid = NULL;
      $node->vid = NULL;
      $node->name = $user->name;
      $node->uid = $user->uid;
      $node->created = NULL;
      $node->menu = NULL;
      if (isset($node->book['mlid'])) {
        $node->book['mlid'] = NULL;
      }
      $node->path = NULL;
      $node->files = array();
      drupal_set_title(t('Export of !title', array('!title' => check_plain($node->title))));

      if (variable_get('export_reset_'. $node->type, FALSE)) {
        $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
        // Fill in the default values.
        foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
          $node->$key = in_array($key, $node_options);
        }
      }
      // Let other modules do special fixing up.
      // The function signature is: hook_export_node_alter(&$node, $original_node, $method)
      // Where $method is 'export'
      drupal_alter("export_node", $node, $original_node, "export");
      $node_code = var_export($node, true);
      $node_code = str_replace('stdClass::__set_state(array(', 'node(code(', $node_code);
      return ($return_code ? $node_code : drupal_get_form('export_form', $node_code));
    }
  }
}

/**
 *  Export Form
 */
function export_form($form_state, $code) {
  $form = array();
  $form['export'] = array(
    '#type' => 'textarea',
    '#title' => t('Node Code'),
    '#default_value' => $code,
    '#rows' => 30,
    '#description' => t('Copy this code and then on the site you want to import to, go to the Import Node link under Content Management, and paste it in there.'),
  );
  return $form;
}

/**
 *  Import Node UI
 */
function export_node_import() {
  global $user;
  $output;
  if ($_POST['import_code']) {
    $node_code = str_replace('node(code(', 'array(array(', trim($_POST['import_code']));
    $imported_nodes = array();
    if (substr($node_code, 0, 5) == 'bulk(') {
      $node_code = str_replace('bulk(', 'array(', $node_code);
      $bulk_import = eval("return ".$node_code.";");
      foreach ($bulk_import as $imported_node) {
        $new_node = (object)current($imported_node);
        $new_nid = export_node_save($new_node);
        drupal_set_message(t("Imported !node", array('!node' => l($new_node->title, 'node/'. $new_nid))));
      }
      drupal_goto('admin/content/node');
    }
    else {
      $imported_nodes = eval("return ".$node_code.";");
      return export_node_check((object)current($imported_nodes));
    }
  } else {
    return drupal_get_form('export_import_form', $form);
  }
}

/**
 *  Import Form
 */
function export_import_form($form_state) {
  $form['#prefix'] = t('<p>You may import a node by copy-and-pasting the results of an export node. </p>');
  $method = variable_get('export_method', 'prepopulate');
  if ($method == 'prepopulate') {
    $form['#prefix'] .= t('<p>You will be given a prepopulated node add form which you must save to create the node.</p>');
  }
  elseif ($method == 'save-edit') {
    $form['#prefix'] .= t('<p>The node will be created and you will then be taken to the node edit page.</p>');
  }
  $form['#prefix'] .= t('<p>To change this behaviour, <a href="/admin/settings/export">configure the settings</a>.</p>');
  $form['import_code'] = array(
    '#type' => 'textarea',
    '#title' => t('Import Code'),
    '#default_value' => '',
    '#required' => TRUE,
    '#rows' => 30,
    '#description' => t('Cut and paste the results of an <em>Export Node</em> here.'),
  );
  $form['#redirect'] = FALSE;
  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Import'),
  );
  return $form;
}

/**
 *  Menu callback: prompt the user to confirm the operation
 */
function export_node_check($node) {

  $method = variable_get('export_method', 'prepopulate');
  
  switch ($method) {
    case 'save-edit':
      $new_nid = export_node_save($node);
      drupal_goto('node/'. $new_nid .'/edit');
      break;
    case 'prepopulate':
    default:
      include_once(drupal_get_path('module', 'node') .'/node.pages.inc');
      return export_node_prepopulate($node);
      break;
  }
}

/**
 *  exports a node - prepopulate a node editing form
 */
function export_node_prepopulate($original_node) {
  global $user;

  if (export_is_permitted($original_node->type)) {
    $node = drupal_clone($original_node);

    $node->nid = NULL;
    $node->vid = NULL;
    $node->name = $user->name;
    $node->uid = $user->uid;
    $node->created = NULL;
    $node->menu = NULL;
    if (isset($node->book['mlid'])) {
      $node->book['mlid'] = NULL;
    }
    $node->path = NULL;
    $node->files = array();
    drupal_set_title(check_plain($node->title));

    if (variable_get('export_reset_'. $node->type, FALSE)) {
      $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
      // Fill in the default values.
      foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
        $node->$key = in_array($key, $node_options);
      }
    }
    // Let other modules do special fixing up.
    // The function signature is: hook_export_node_alter(&$node, $original_node, $method)
    // Where $method is either 'prepopulate' or 'save-edit'.
    drupal_alter("export_node", $node, $original_node, "prepopulate");
    return str_replace('admin/content/import', 'node/add/' . $node->type, drupal_get_form($node->type . '_node_form', $node));
  }
}

/**
 *  exports a node by directly saving it.
 */
function export_node_save($node) {
  global $user;

  if (export_is_permitted($node->type)) {

    $original_node = drupal_clone($node);

    $node->nid = NULL;
    $node->vid = NULL;
    $node->name = $user->name;
    $node->uid = $user->uid;
    $node->created = NULL;
    $node->menu = NULL;
    $node->book['mlid'] = NULL;
    $node->path = NULL;
    $node->files = array();

    if (variable_get('export_reset_'. $node->type, FALSE)) {
      $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
      // Fill in the default values.
      foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
        $node->$key = in_array($key, $node_options);
      }
    }
    // Let other modules do special fixing up.
    // The function signature is: hook_export_node_alter(&$node, $original_node, $method)
    // Where $method is either 'prepopulate' or 'save-edit'.
    drupal_alter("export_node", $node, $original_node, "save-edit");

    node_save($node);

    return $node->nid;
  }

}


/**
 * Output page for 'export' node operation.

function export_node_export_bulk_page() {
  $node_code = $_SESSION['export_bulk'];
  unset($_SESSION['export_bulk']);
  return drupal_get_form('export_form', $node_code);
} */