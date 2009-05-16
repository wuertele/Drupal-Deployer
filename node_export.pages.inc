<?php
// $Id$

/**
 * Menu callback to configure module settings.
 */
function node_export_settings() {

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );
  $form['basic']['node_export_method'] = array(
    '#type' => 'radios',
    '#title' => t('Method to use when importing a node'),
    '#options' => array('prepopulate' => t('Pre-populate the node form fields'), 'save-edit' => t('Save as a new node then edit')),
    '#default_value' => variable_get('node_export_method', 'prepopulate'),
  );
  
  $form['publishing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Should the publishing options ( e.g. published, promoted, etc) be reset to the defaults?'),
  );
  $types = node_get_types('names');

  foreach ($types as $type => $name) {
    $form['publishing']['node_export_reset_'. $type] = array(
      '#type' => 'checkbox',
      '#title' => t('@s: reset publishing options when exported', array('@s' => $name)),
      '#default_value' => variable_get('node_export_reset_'. $type, FALSE),
    );
  }
  // Need the variable default key to be something that's never a valid node type.
  $form['omit'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content types that are not to be exported - omitted due to incompatibility'),
  );
  $form['omit']['node_export_omitted'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Omitted content types'),
    '#default_value' => variable_get('node_export_omitted', array()),
    '#options' => $types,
    '#description' => t('Select any node types which should <em>never</em> be exported. Typically you should will want to select here all node types for which importing fails (e.g. image nodes).'),
  );

  return system_settings_form($form);
}

/**
 *  exports a node - prepopulate a node editing form
 *  set $return_code to TRUE to not return form but the code instead.
 */
function node_export_node_export($original_node, $return_code = FALSE, $iteration = 0) {
  if (isset($original_node->nid)) {
    global $user;

    if (node_export_is_permitted($original_node->type)) {
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

      if (variable_get('node_export_reset_'. $node->type, FALSE)) {
        $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
        // Fill in the default values.
        foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
          $node->$key = in_array($key, $node_options);
        }
      }

      // Fix taxonomy array
      if (isset($node->taxonomy) && count($node->taxonomy)) {
        $vocabularies = taxonomy_get_vocabularies();
        $new_taxonomy = array();
        foreach ($node->taxonomy as $term) {
          // Free-tagging vocabularies need a different format
          if ($vocabularies[$term->vid]->tags) {
            $new_taxonomy['tags'][$term->vid][] = $term->name;
          }
          else {
            $new_taxonomy[$term->vid][$term->tid] = $term->tid;
          }
        }

        if (isset($new_taxonomy['tags']) && count($new_taxonomy['tags'])) {
          // Comma seperate the tags
          foreach ($new_taxonomy['tags'] as $vid => $tags) {
            $new_taxonomy['tags'][$vid] = implode(', ', $tags);
          }

        }
        $node->taxonomy = $new_taxonomy;
      }

      // Let other modules do special fixing up.
      // The function signature is: hook_export_node_alter(&$node, $original_node, $method)
      // Where $method is 'export'
      drupal_alter("node_export_node", $node, $original_node, "export");
      $node_code = node_export_node_encode($node, $iteration);
      return ($return_code ? $node_code : drupal_get_form('node_export_form', $node_code));
    }
  }
}

/**
 *  Export Form
 */
function node_export_form($form_state, $code) {
  $form = array();
  $form['export'] = array(
    '#type' => 'textarea',
    '#title' => t('Node Code'),
    '#default_value' => $code,
    '#rows' => 30,
    '#description' => t('Copy this code and then on the site you want to import to, go to the Import Node link under Content Management, and paste it in there.'),
    '#attributes' => array(
      'style' => 'width: 97%;',
    ),
  );
  return $form;
}

/**
 *  Import Node UI
 */
function node_export_node_import() {
  global $user;
  $output;
  if ($_POST['import_code']) {
    $node_code = trim($_POST['import_code']);
    $import = node_export_node_decode($node_code);
    if (is_array($import)) {
      foreach ($import as $new_node) {
        $new_nid = node_export_node_save($new_node);
        drupal_set_message(t("Imported !node", array('!node' => l($new_node->title, 'node/'. $new_nid))));
      }
      drupal_goto('admin/content/node');
    }
    else {
      return node_export_node_check($import);
    }
  } 
  else {
    return drupal_get_form('node_export_import_form', $form);
  }
}

/**
 *  Import Form
 */
function node_export_import_form($form_state) {
  $form['#prefix'] = t('<p>You may import a node by copy-and-pasting the results of an exported node. </p>');
  $method = variable_get('node_export_method', 'prepopulate');
  if ($method == 'prepopulate') {
    $form['#prefix'] .= t('<p>You will be given a prepopulated node add form which you must save to create the node.</p>');
  }
  elseif ($method == 'save-edit') {
    $form['#prefix'] .= t('<p>The node will be created and you will then be taken to the node edit page.</p>');
  }
  $form['#prefix'] .= t('<p>To change this behaviour, <a href="/admin/settings/node_export">configure the settings</a>.</p>');
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
function node_export_node_check($node) {

  $method = variable_get('node_export_method', 'prepopulate');
  
  switch ($method) {
    case 'save-edit':
      $new_nid = node_export_node_save($node);
      drupal_goto('node/'. $new_nid .'/edit');
      break;
    case 'prepopulate':
    default:
      include_once(drupal_get_path('module', 'node') .'/node.pages.inc');
      return node_export_node_prepopulate($node);
      break;
  }
}

/**
 *  exports a node - prepopulate a node editing form
 */
function node_export_node_prepopulate($original_node) {
  global $user;

  if (node_export_is_permitted($original_node->type)) {
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

    if (variable_get('node_export_reset_'. $node->type, FALSE)) {
      $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
      // Fill in the default values.
      foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
        $node->$key = in_array($key, $node_options);
      }
    }
    // Let other modules do special fixing up.
    // The function signature is: hook_export_node_alter(&$node, $original_node, $method)
    // Where $method is either 'prepopulate' or 'save-edit'.
    drupal_alter("node_export_node", $node, $original_node, "prepopulate");
    return str_replace('admin/content/import', 'node/add/' . $node->type, drupal_get_form($node->type . '_node_form', $node));
  }
}

/**
 *  exports a node by directly saving it.
 */
function node_export_node_save($node) {
  global $user;

  if (node_export_is_permitted($node->type)) {

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

    if (variable_get('node_export_reset_'. $node->type, FALSE)) {
      $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
      // Fill in the default values.
      foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
        $node->$key = in_array($key, $node_options);
      }
    }
    // Let other modules do special fixing up.
    // The function signature is: hook_export_node_alter(&$node, $original_node, $method)
    // Where $method is either 'prepopulate' or 'save-edit'.
    drupal_alter("node_export_node", $node, $original_node, "save-edit");

    node_save($node);

    return $node->nid;
  }

}


/**
 *  build node code string recursively
 */
function node_export_node_encode($var, $iteration = 0){
  $tab = '';
  for ($i = 0; $i < $iteration; $i++) {
    $tab = $tab ."  ";
  }
  $iteration++;
  if (is_object($var)) {
    $var = (array)$var;
    $var['#_export_node_encode_object'] = '1';
  }
  if (is_array($var)) {
    $empty = empty($var);
    $code = "array(". ($empty ? '' : "\n");
    foreach ($var as $key => $value) {
      $code .= $tab ."  '". $key ."' => ".node_export_node_encode($value, $iteration).",\n";
    }
    $code .= ($empty ? '' : $tab) .")";
    return $code;
  } else {
    if (is_string($var)) {
      return "'". addslashes($var) ."'";
    } 
    elseif (is_numeric($var)) {
      return $var;
    } 
    elseif (is_bool($var)) {
      return ($var ? 'TRUE' : 'FALSE');
    } 
    else {
      return 'NULL';
    }
  }
}

/**
 *  eval and return decoded string
 */
function node_export_node_decode($string) {
  $array = eval('return '.$string.';');
  $return = node_export_node_decode_objects($array);
  return $return;
}

/**
 *  recursively convert arrays back to objects
 */
function node_export_node_decode_objects($array) {
  foreach ($array as $k => $v) {
    if (is_array($v)) {
      $array[$k] = node_export_node_decode_objects($v);
    }
    if (is_string($v)) {
      if (get_magic_quotes_gpc()) {
          $array[$k] = stripslashes($v);
      }
      else {
          $array[$k] = $v;
      }
    }
  }
  if ($array['#_export_node_encode_object']) {
    unset($array['#_export_node_encode_object']);
    $array = (object)$array;
  }
  return $array;
}