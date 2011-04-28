<?php
// $Id$

function pingback_settings_form() {
  $form = array();
  $formats = array();
  foreach (filter_formats() as $f) {
    $formats[$f->format] = $f->name;
  }
  $form['pingback_input_format'] = array(
    '#type' => 'select',
    '#title' => t('Pingback input format'),
    '#options' => $formats,
    '#default_value' => variable_get('pingback_input_format', FILTER_FORMAT_DEFAULT),
    '#description' => t('The input format used to display pingbacks. All comments in this input format will be considered as pingbacks, so you must create <a href="@create-format">an exclusive input format</a> only for pingbacks (and let anonymous users post comments with that format). If you change this option, any existing pingbacks may become comments.', array('@create-format' => url('admin/settings/filters/add'))),
  );
  $form['pingback_hide_format_for_anon'] = array(
    '#type' => 'radios',
    '#title' => t('Hide pingback input format for anonymous users'),
    '#options' => array(0 => t('Disabled'), 1 => t('Enabled')),
    '#default_value' => variable_get('pingback_hide_format_for_anon', 0),
    '#description' => t('Hide the input format chosen from the above option when anonymous users post comments. Enable this, or else people can post comments as pingbacks!'), //to devs: every node update will invalidate the variable cache, so it is not appropriate for sites that has many concurrent node insert / update, e.g. 50 node updates each second.
  );
  $form['pingback_receive'] = array(
    '#type' => 'radios',
    '#title' => t('Receive pingbacks'),
    '#options' => array(1 => t('Yes'), 0 => t('No')),
    '#default_value' => variable_get('pingback_receive', 1),
    '#description' => t('Receive pingbacks sent from other sites linking to you.'),
  );
  $form['pingback_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Pingback autoping'),
    '#options' => array('off' => t('Never'), 'submit' => t('When creating and updating nodes'), 'cron' => t('On cron run')),
    '#default_value' => variable_get('pingback_mode', 'off'),
    '#description' => t('When to automatically ping linked sites.  Performance of cron mode is still poor if post creation and update traffic is very high.'), //to devs: every node update will invalidate the variable cache, so it is not appropriate for sites that has many concurrent node insert / update, e.g. 50 node updates each second.
  );
  $form['pingback_check_per_cron'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of checks per cron run'),
    '#default_value' => variable_get('pingback_check_per_cron', 30),
    '#description' => t('How many pingbacks one cron run must check.  Because pingbacks connect to external servers the process can be time consuming, depending on the amount of pingbacks. This option is effective only if pingbacks are sent at cron run.'),
  );
  $form['pingback_notify_successful_pings'] = array(
    '#type' => 'radios',
    '#title' => t('Notify for successful pingbacks'),
    '#options' => array(1 => t('Yes'), 0 => t('No')),
    '#default_value' => variable_get('pingback_notify_successful_pings', 1),
    '#description' => t('Notify users about successful pingbacks. This option is efective only if pingbacks are sent when creating and updating nodes.'),
  );
  /*
  How to implement this, I wonder.  comment_save() doesn't let modules insert comments with arbitrary status
  $form['pingback_moderation'] = array(
    '#type' => 'radios',
    '#title' => t('Pingback moderation'),
    '#options' => array(0 => t('Disabled'), 1 => t('Enabled')),
    '#default_value' => variable_get('pingback_moderation', 0),
    '#description' => t('If pingback moderation is enabled, incoming pingbacks will not be published automatically.  A user with "administer comments" permission can publish those pingbacks.'), //to devs: every node update will invalidate the variable cache, so it is not appropriate for sites that has many concurrent node insert / update, e.g. 50 node updates each second.
  );
  */
  return system_settings_form($form);
}

function pingback_settings_form_validate($form, &$form_state) {
  $formats = filter_formats();
  $f = $formats[$form_state['values']['pingback_input_format']];
  //dpm($f);
  //note: this role-matching is dependent of current implementation of filter.module
  if (strpos($f->roles, ','. DRUPAL_ANONYMOUS_RID .',') === FALSE) {
    form_set_error('pingback_input_format', t('The input format has to be usable by anonymous users.'));
  }
}