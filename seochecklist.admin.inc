<?php
// $Id$

/**
 * Define the settings form.
 */
function seochecklist_admin_settings() {
  global $user;

  $form['save_above'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['check_existing'] = array(
    '#type' => 'submit',
    '#value' => t('Check for already installed modules'),
  );

  // Fetch modules and groups from database.
  $query = "SELECT id, name, description FROM {seo_group} ORDER BY id";

  $result = db_query($query);
  $group_id = 0;

  // Every group is a fieldset.
  while ($data = db_fetch_object($result)) {
    $group_id = intval($data->id);

    $form['group_' . $group_id] = array(
      '#type' => 'fieldset',
      '#title' => t($data->name),
      '#collapsible' => TRUE,
      '#description' => t($data->description),
    );

    $res = db_query("SELECT download, enable, configure, module, completed, id, name, uid FROM {seo_checklist} WHERE group_id = %d ORDER BY order_id", $group_id);
    while ($row = db_fetch_object($res)) {
      $row->links = array();

      if ($row->completed) {
        // Show when and who completed this task.
        $row->links[] = t('Completed on %date by !username', array('%date' => format_date($row->completed, 'small'), '!username' => theme('username', user_load($row->uid))));
      }
      else {
        // Show non-completed sub-tasks.
        if ($row->download) {
          $row->links[] = l(t('Download'), $row->download, array('attributes' => array('target' => '_blank')));
        }
        if ($row->enable) {
          $row->links[] = l(t('Enable'), $row->enable);
        }
      }

      if ($row->configure && (!$row->module || module_exists($row->module))) {
        // Show the link to configure if this isn't a module or module is enabled.
        $row->links[] = l(t('Configure'), $row->configure);
      }

      $form['group_' . $group_id]['seochecklist_nodetypes_' . intval($row->id)] = array(
        '#type' => 'checkbox',
        '#title' => t($row->name),
        '#default_value' => $row->completed || ($row->module && module_exists($row->module)),
        '#description' => join(' | ', $row->links),
      );
      if (strpos($row->name, 'Clean URLs') === 0) {
        $form['group_' . $group_id]['seochecklist_nodetypes_' . intval($row->id)]['#disabled'] = !variable_get('clean_url', 0);
        $form['group_' . $group_id]['seochecklist_nodetypes_' . intval($row->id)]['#default_value'] |= variable_get('clean_url', 0);
      }
    }
  }

  $form['bottom_code'] = array(
    '#type' => 'fieldset',
    '#title' => t('Extras'),
    '#collapsible' => TRUE,
  );
  $form['bottom_code']['seo_checklist_link'] = array(
    '#type' => 'checkbox',
    '#title' => t("Link to Volacci to thank them for this awesome module. <strong>Don't forget to click Save!</strong>"),
    '#description' => t("A small link will appear at the very bottom of your website. You can disable it at any time by un-checking this box. We really appreciate it!!!"),
    '#default_value' => variable_get('seo_checklist_link', 0),
  );
  $form['bottom_code']['seo_checklist_thanks'] = array(
    '#type' => 'checkbox',
    '#title' => t("Link to us then send us feedback on the Drupal 6 SEO Checklist module or just say \"Thanks!\" and we will link to you from our website. Send your feedback and link information to !email. (If you don't know why you should link with other websites, read this: !link.)", array('!email' => l('seochecklist@volacci.com', 'mailto:seochecklist@volacci.com'), '!link' => l('Why links help SEO', 'http://www.volacci.com/why-links-help-seo'))),
    '#default_value' => variable_get('seo_checklist_thanks', 0),
  );
  $form['bottom_code']['seo_checklist_podcast'] = array(
    '#type' => 'checkbox',
    '#title' => t("Listen to the Volacci Drupal SEO Podcast for more tips and tricks about Drupal SEO: !link", array('!link' => l('http://www.volacci.com/podcast', 'http://www.volacci.com/podcast'))),
    '#default_value' => variable_get('seo_checklist_podcast', 0),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * Submit callback for seochecklist_admin_settings().
 */
function seochecklist_admin_settings_submit($form, &$form_state) {
  global $user;

  // Saving checked modules.
  if ($form_state['values']['op'] == t('Save')) {
    foreach ($form_state['values'] as $key => $value) {
      if (preg_match('/seochecklist_nodetypes/', $key)) {
        $key = explode('_', $key);
        $key = $key[2];
        $current = (bool) db_result(db_query("SELECT completed FROM {seo_checklist} WHERE id = %d", $key));
        if ($current != $value) {
          // If the checkbox changed states, update the record.
          db_query("UPDATE {seo_checklist} SET completed = %d, uid = %d WHERE id = %d", ($value ? time() : 0), $user->uid, $key);
        }
      }
    }

    // Special values not in database.
    variable_set('seo_checklist_link', $form_state['values']['seo_checklist_link']);
    variable_set('seo_checklist_thanks', $form_state['values']['seo_checklist_thanks']);
    variable_set('seo_checklist_podcast', $form_state['values']['seo_checklist_podcast']);
  }

  // Check for already installed modules.
  if ($form_state['values']['op'] == t('Check for already installed modules')) {
    $query = db_query("SELECT id, module FROM {seo_checklist} WHERE module <> '' AND completed = 0");
    while ($task = db_fetch_object($query)) {
      if (module_exists($task->module)) {
        db_query("UPDATE {seo_checklist} SET completed = %d WHERE id = %d", time(), $task->id);
      }
    }
  }
}
