<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for Mollom module.
 */

/**
 * Menu callback; Displays a list of forms configured for Mollom.
 */
function mollom_admin_form_list() {
  $modes = array(
    MOLLOM_MODE_DISABLED => t('No protection'),
    MOLLOM_MODE_CAPTCHA => t('CAPTCHA only'),
    MOLLOM_MODE_ANALYSIS => t('Text analysis and CAPTCHA backup'),
  );

  $header = array(
    t('Form'),
    t('Protection mode'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();
  $result = db_query("SELECT form_id FROM {mollom_form}");
  while ($form_id = db_result($result)) {
    $mollom_form = mollom_form_load($form_id);
    $rows[] = array(
      $mollom_form['title'],
      $modes[$mollom_form['mode']],
      l(t('Configure'), 'admin/settings/mollom/manage/' . $form_id),
      l(t('Unprotect'), 'admin/settings/mollom/unprotect/' . $form_id),
    );
  }

  // Add a row to add a form.
  if (empty($rows)) {
    $rows[] = array(array('data' => l(t('Add form'), 'admin/settings/mollom/add'), 'colspan' => 4));
  }

  return theme('table', $header, $rows);
}

/**
 * Return registered forms as an array suitable for a 'checkboxes' form element #options property.
 */
function mollom_admin_form_options() {
  // Retrieve all registered forms.
  $form_info = mollom_get_form_info();

  // Remove already configured form ids.
  $result = db_query("SELECT form_id FROM {mollom_form}");
  while ($form_id = db_result($result)) {
    unset($form_info[$form_id]);
  }
  // If all registered forms are configured already, output a message, and
  // redirect the user back to overview.
  if (empty($form_info)) {
    drupal_set_message(t('All available forms are protected already.'));
    drupal_goto('admin/settings/mollom');
  }

  // Load module information.
  $modules = module_implements('mollom_form_info');
  $placeholders = db_placeholders($modules, 'varchar');
  $result = db_query("SELECT name, info FROM {system} WHERE type = 'module' AND name IN ($placeholders)", $modules);
  $modules = array();
  while ($row = db_fetch_object($result)) {
    $module_info = unserialize($row->info);
    $modules[$row->name] = t($module_info['name']);
  }

  // Transform form information into an associative array suitable for #options.
  foreach ($form_info as $form_id => $info) {
    $form_info[$form_id] = $modules[$info['module']] . ': ' . $info['title'];
  }
  // Sort form options by title.
  asort($form_info);

  return $form_info;
}

/**
 * Form builder; Configure Mollom protection for a form.
 */
function mollom_admin_configure_form(&$form_state, $mollom_form = NULL) {
  // If no $mollom_form was passed, then we are adding a new form configuration.
  if (!isset($mollom_form)) {
    if (!isset($form_state['storage']['mollom_form'])) {
      $form_state['storage']['step'] = 'select';
    }
    else {
      $form_state['storage']['step'] = 'configure';
      $mollom_form = $form_state['storage']['mollom_form'];
    }
  }
  // Otherwise, we are editing an existing form configuration.
  else {
    $form_state['storage']['step'] = 'configure';
    $form_state['storage']['mollom_form'] = $mollom_form;
  }

  $form['#tree'] = TRUE;

  switch ($form_state['storage']['step']) {
    case 'select':
      drupal_add_js(drupal_get_path('module', 'mollom') . '/mollom.js');

      $form['mollom']['form_id'] = array(
        '#type' => 'select',
        '#title' => t('Form'),
        '#options' => mollom_admin_form_options(),
        '#required' => TRUE,
      );
      $form['actions']['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
        '#submit' => array('mollom_admin_configure_form_next_submit'),
      );
      break;

    case 'configure':
      // Display a list of fields for textual analysis (last step).
      $form['mollom']['form_id'] = array(
        '#type' => 'value',
        '#value' => $mollom_form['form_id'],
      );
      $form['mollom']['form_title'] = array(
        '#type' => 'item',
        '#title' => t('Form'),
        '#value' => $mollom_form['title'],
      );
      // Form elements defined by hook_mollom_form_info() use the
      // 'parent][child' syntax, which Form API also uses internally for
      // form_set_error(), and which allows us to recurse into nested fields
      // during processing of submitted form values. However, since we are using
      // those keys also as internal values to configure the fields to use for
      // textual analysis, we need to encode them. Otherwise, a nested field key
      // would result in the following checkbox attribute:
      //   '#name' => 'mollom[fields][parent][child]'
      // This would lead to a form validation error, because it is a valid key.
      // By encoding them, we prevent this from happening:
      //   '#name' => 'mollom[fields][parent%5D%5Bchild]'
      // @todo Use PHP5 functions in D7 (e.g. array_combine()).
      $elements = array();
      foreach ($mollom_form['elements'] as $key => $value) {
        $elements[rawurlencode($key)] = $value;
      }
      $fields = array();
      foreach ($mollom_form['fields'] as $value) {
        $fields[] = rawurlencode($value);
      }
      $form['mollom']['fields'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Fields to analyze'),
        '#options' => $elements,
        '#default_value' => $fields,
        '#description' => t('If no fields are selected, the form will be protected by a CAPTCHA.'),
      );
      if (empty($form['mollom']['fields']['#options'])) {
        $form['mollom']['fields']['#description'] = t('No fields are available.');
      }
      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
      );
      break;
  }

  $form['actions']['cancel'] = array(
    '#value' => l(t('Cancel'), 'admin/settings/mollom'),
  );

  return $form;
}

/**
 * Form submit handler for 'Next' button on Mollom form configuration form.
 */
function mollom_admin_configure_form_next_submit($form, &$form_state) {
  $form_id = $form_state['values']['mollom']['form_id'];

  // Load form information into $form_state for configuration.
  $mollom_form = mollom_get_form_info($form_id);
  // Enable all fields for textual analysis by default.
  if (!empty($mollom_form['mode']) && !empty($mollom_form['elements'])) {
    $mollom_form['fields'] = array_keys($mollom_form['elements']);
  }
  else {
    $mollom_form['fields'] = array();
  }
  $form_state['storage']['mollom_form'] = $mollom_form;

  $form_state['storage']['step'] = 'configure';
}

/**
 * Form submit handler for Mollom form configuration form.
 */
function mollom_admin_configure_form_submit($form, &$form_state) {
  $mollom_form = $form_state['values']['mollom'];
  // Merge in form information from $form_state.
  $mollom_form += $form_state['storage']['mollom_form'];
  // Update form information in $form_state for potential rebuilds.
  $form_state['storage']['mollom_form'] = $mollom_form;

  // Prepare selected fields for storage.
  $fields = array();
  foreach (array_keys(array_filter($mollom_form['fields'])) as $field) {
    $fields[] = rawurldecode($field);
  }
  $mollom_form['fields'] = $fields;
  // Determine form protection to use; use CAPTCHA-only protection if no fields
  // were selected, otherwise text analysis.
  $mollom_form['mode'] = (!empty($mollom_form['fields']) ? MOLLOM_MODE_ANALYSIS : MOLLOM_MODE_CAPTCHA);

  $status = mollom_form_save($mollom_form);
  if ($status === SAVED_NEW) {
    drupal_set_message('The form protection has been added.');
  }
  else {
    drupal_set_message('The form protection has been updated.');
  }

  unset($form_state['storage']);
  $form_state['redirect'] = 'admin/settings/mollom';
}

/**
 * Form builder; Remove Mollom protection from a form.
 */
function mollom_admin_unprotect_form(&$form_state, $mollom_form) {
  $form['#tree'] = TRUE;
  $form['form'] = array(
    '#type' => 'item',
    '#title' => t('Form'),
    '#value' => $mollom_form['title'],
  );
  $form['mollom']['form_id'] = array(
    '#type' => 'value',
    '#value' => $mollom_form['form_id'],
  );

  return confirm_form($form,
    t('Are you sure you want to unprotect this form?'),
    'admin/settings/mollom',
    t('Mollom will no longer protect this form from spam.')
  );
}

/**
 * Form submit handler for mollom_admin_unprotect_form().
 */
function mollom_admin_unprotect_form_submit($form, &$form_state) {
  db_query("DELETE FROM {mollom_form} WHERE form_id = '%s'", $form_state['values']['mollom']['form_id']);

  $form_state['redirect'] = 'admin/settings/mollom';
}

/**
 * Form builder; Global Mollom settings form.
 */
function mollom_admin_settings(&$form_state) {
  // When a user visits the Mollom administration page, automatically verify the
  // keys and output any error messages.
  if (empty($form_state['post'])) {
    $status = _mollom_status(TRUE);
    if ($status === TRUE) {
      // Everything is in order.
    }
    elseif ($status['keys valid'] === NETWORK_ERROR) {
      drupal_set_message(t('We tried to contact the Mollom servers but we encountered a network error. Please make sure that your web server can make outgoing HTTP requests.'), 'error');
    }
    elseif ($status['keys valid'] === MOLLOM_ERROR) {
      drupal_set_message(t('We contacted the Mollom servers to verify your keys: your keys do not exist or are no longer valid. Please visit the <em>Manage sites</em> page on the Mollom website again: <a href="@mollom-user">@mollom-user</a>.', array('@mollom-user' => 'http://mollom.com/user')), 'error');
    }
  }

  $form['server'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fallback strategy'),
    '#description' => t('When the Mollom servers are down or otherwise unreachable, no text analysis is performed and no CAPTCHAs are generated. If this occurs, your site will use the configured fallback strategy. Subscribers to <a href="@pricing">Mollom Plus</a> receive access to <a href="@sla">Mollom\'s high-availability backend infrastructure</a>, not available to free users, reducing potential downtime.', array(
      '@pricing' => 'http://mollom.com/pricing',
      '@sla' => 'http://mollom.com/standard-service-level-agreement',
    )),
  );
  $form['server']['mollom_fallback'] = array(
    '#type' => 'radios',
    // Default to treating everything as inappropriate.
    '#default_value' => variable_get('mollom_fallback', MOLLOM_FALLBACK_BLOCK),
    '#options' => array(
      MOLLOM_FALLBACK_BLOCK => t('Block all submissions of protected forms until the server problems are resolved'),
      MOLLOM_FALLBACK_ACCEPT => t('Leave all forms unprotected and accept all submissions'),
    ),
  );

  $form['access-keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mollom access keys'),
    '#description' => t('To use Mollom, you need a public and private key. To obtain your keys, <a href="@mollom-login">register and login on mollom.com</a>, and <a href="@mollom-manager-add">create a subscription</a> for your site. Once you created a subscription, copy your private and public access keys from the <a href="@mollom-manager">site manager</a> into the form fields below, and you are ready to go.', array(
      '@mollom-register' => 'http://mollom.com/user/register',
      '@mollom-login' => 'http://mollom.com/user',
      '@mollom-manager-add' => 'http://mollom.com/site-manager/add',
      '@mollom-manager' => 'http://mollom.com/site-manager',
    )),
  );
  $form['access-keys']['mollom_public_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Public key'),
    '#default_value' => variable_get('mollom_public_key', ''),
    '#description' => t('The public key is used to uniquely identify you.'),
  );
  $form['access-keys']['mollom_private_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Private key'),
    '#default_value' => variable_get('mollom_private_key', ''),
    '#description' => t('The private key is used to prevent someone from hijacking your requests. Similar to a password, it should never be shared with anyone.'),
  );

  $form['mollom_privacy_link'] = array(
    '#type' => 'checkbox',
    '#title' => t("Link to Mollom's privacy policy on forms protected by textual analysis"),
    '#return_value' => 1,
    '#default_value' => variable_get('mollom_privacy_link', 1),
    '#description' => t('Displays a link to the recommended <a href="@privacy-policy-url">privacy policy on mollom.com</a> on all forms that are protected via <a href="@help-url">textual analysis</a>. When disabling this option, you are required to inform visitors about data privacy through other means, as stated in the <a href="@terms-of-service-url">terms of service</a> applying to your subscription.', array(
      '@privacy-policy-url' => 'http://mollom.com/web-service-privacy-policy',
      '@help-url' => url('admin/help/mollom'),
      '@terms-of-service-url' => 'http://mollom.com/terms-of-service',
    )),
  );

  return system_settings_form($form);
}

/**
 * Form submit handler to mass-report and unpublish or delete comments.
 */
function mollom_comment_admin_overview_submit($form, &$form_state) {
  if (strpos($form_state['values']['operation'], 'mollom-') === 0) {
    list($id, $operation) = explode('-', $form_state['values']['operation']);
    foreach ($form_state['values']['comments'] as $cid => $value) {
      if ($value) {
        // First, report the comments as spam to Mollom.com.
        if ($data = mollom_data_load('comment', $cid)) {
          _mollom_send_feedback($data->session);
        }

        // Second, perform the proper operation on the comments:
        if ($comment = _comment_load($cid)) {
          if ($operation == 'unpublish') {
            db_query("UPDATE {comments} SET status = %d WHERE cid = %d", COMMENT_NOT_PUBLISHED, $cid);
            _comment_update_node_statistics($comment->nid);
          }
          elseif ($operation == 'delete') {
            _comment_delete_thread($comment);
            _comment_update_node_statistics($comment->nid);
          }
        }
      }
    }

    // Flush caches so the content changes are visible for anonymous users.
    cache_clear_all();

    if ($operation == 'delete') {
      drupal_set_message(t('The selected comments have been reported as inappropriate and are deleted.'));
    }
    else {
      drupal_set_message(t('The selected comments have been reported as inappropriate and are unpublished.'));
    }
  }
}

/**
 * Form submit handler to mass-report and unpublish or delete nodes.
 */
function mollom_node_admin_overview_submit($form, &$form_state) {
  if (strpos($form_state['values']['operation'], 'mollom-') === 0) {
    list($id, $operation) = explode('-', $form_state['values']['operation']);
    foreach ($form_state['values']['nodes'] as $nid => $value) {
      if ($value) {
        // First, report the nodes as spam to Mollom.com.
        if ($data = mollom_data_load('node', $nid)) {
           _mollom_send_feedback($data->session);
        }

        if ($node = node_load($nid)) {
          if ($operation == 'unpublish') {
            db_query("UPDATE {node} SET status = 0 WHERE nid = %d", $nid);
          }
          elseif ($operation == 'delete') {
            node_delete($nid);
          }
        }
      }
    }

    // Flush caches so the content changes are visible for anonymous users.
    cache_clear_all();

    if ($operation == 'delete') {
      drupal_set_message(t('The selected posts have been reported as inappropriate and are deleted.'));
    }
    else {
      drupal_set_message(t('The selected posts have been reported as inappropriate and are unpublished.'));
    }
  }
}

/**
 * Menu callback; Displays the administrative reports page.
 */
function mollom_reports_page() {
  $embed_attributes = array(
    'src' => 'http://mollom.com/statistics.swf?key=' . check_plain(variable_get('mollom_public_key', '')),
    'quality' => 'high',
    'width' => '100%',
    'height' => '430',
    'name' => 'Mollom',
    'align' => 'middle',
    'play' => 'true',
    'loop' => 'false',
    'allowScriptAccess' => 'sameDomain',
    'type' => 'application/x-shockwave-flash',
    'pluginspage' => 'http://www.adobe.com/go/getflashplayer',
  );
  $form['chart'] = array(
    '#type' => 'item',
    '#title' => t('Statistics'),
    '#value' => '<embed' . drupal_attributes($embed_attributes) . '></embed>',
  );
  if (module_exists('dblog')) {
    $logs = array();
    $query = db_query_range("SELECT message, variables FROM {watchdog} WHERE type = 'mollom' AND severity <= %d ORDER BY wid DESC", WATCHDOG_WARNING, 0, 10);
    while ($log = db_fetch_object($query)) {
      $t_args = unserialize($log->variables);
      $logs[] = t($log->message, ($t_args ? $t_args : array()));
    }
    $form['watchdog'] = array(
      '#type' => 'item',
      '#title' => t('Recent Mollom messages'),
      '#value' => $logs ? theme('item_list', $logs) : t('None'),
    );
  }
  return $form;
}

