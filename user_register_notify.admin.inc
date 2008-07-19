<?php
// $Id$

/**
 * @file
 * Settings form.
 */

define('USER_REGISTER_NOTIFY_SUBJECT', t('Account details for !user_name at !site'));
define('USER_REGISTER_NOTIFY_BODY', t("!user_name (!user_view) has created account.\n\nEdit user: !user_edit\n\nDelete User: !user_delete\n\n!profile"));

/**
 * Module settings page.
 */
function user_register_notify_settings() {
  $form['user_register_notify_mailto'] = array(
    '#type' => 'textfield',
    '#title' => t('Send notifications to this email address'),
    '#description' => t('If you leave this blank, the site email address, %mailto, will be used.', array('%mailto' => variable_get('site_mail', ini_get('sendmail_from')))),
    '#default_value' => variable_get('user_register_notify_mailto', variable_get('site_mail', ini_get('sendmail_from'))),
  );
  $form['user_register_notify_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Subject'),
    '#default_value' => variable_get('user_register_notify_subject', USER_REGISTER_NOTIFY_SUBJECT),
    '#required' => TRUE,
    '#description' => t('Subject of user registration messages')
  );
  $form['user_register_notify_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Email Body'),
    '#default_value' => variable_get('user_register_notify_body', USER_REGISTER_NOTIFY_BODY),
    '#rows' => 10,
    '#required' => TRUE,
    '#description' => t('Customize the body of the user registration notification email.') .' '. t('Available variables are:') .' !user_name, !user_mail, !user_view, !user_edit, !user_delete, !site, !uri, !uri_brief, !date, !profile.'
  );
  $alert_options = array(
    'create' => 'Receive Email upon user creation.',
    'update' => 'Receive Email upon user creation and update.',
    );
  $form['user_register_notify_alert'] = array(
    '#type' => 'radios',
    '#options' => $alert_options,
    '#title' => t('Page Sort order'),
    '#default_value' => variable_get('user_register_notify_alert', 'create'),
    '#description' => t('When to send an Email'),
    '#required' => false,
  );
  return system_settings_form($form);
}
