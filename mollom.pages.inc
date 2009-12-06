<?php
// $Id$

/**
 * @file
 * Various non-administration page callbacks for the mollom module.
 */

/**
 * AJAX callback to retrieve a CAPTCHA.
 *
 * @todo Add error handling.
 */
function mollom_captcha_js($type, $session_id) {
  $output = '';

  if ($type == 'audio') {
    $response = mollom('mollom.getAudioCaptcha', array('author_ip' => ip_address(), 'session_id' => $session_id));
    if ($response) {
      $output = '<a href="http://mollom.com" class="mollom-captcha"><embed src="' . check_plain($response['url']) . '" autostart="true" width="120" height="30" /></a>';
      $output .= ' (<a href="#" class="mollom-image-captcha">' . t('use image CAPTCHA') . '</a>)';
    }
  }
  elseif ($type == 'image') {
    $response = mollom('mollom.getImageCaptcha', array('author_ip' => ip_address(), 'session_id' => $session_id));
    if ($response) {
      $output = '<a href="http://mollom.com" class="mollom-captcha"><img src="' . check_plain(url($response['url'])) . '" alt="Mollom CAPTCHA" /></a>';
      $output .= ' (<a href="#" class="mollom-audio-captcha">' . t('play audio CAPTCHA') . '</a>)';
    }
  }

  print $output;
  exit();
}

/**
 * Form builder for report to Mollom form.
 *
 * @param $entity
 *   The entity type of the data to report, e.g. 'node' or 'comment'.
 * @param $id
 *   The entity id the data belongs to. If 'session' is passed as $entity, then
 *   $id is assumed to be a Mollom session_id, as returned by Mollom servers,
 *   which should only be used to report session data that was not stored for an
 *   entity in the database (such as contact form submissions).
 */
function mollom_report_form(&$form_state, $entity, $id) {
  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $form['feedback'] = _mollom_feedback_options();

  return confirm_form($form,
    t('Are you sure you want to delete and report the content as inappropriate?'),
    isset($_GET['destination']) ? $_GET['destination'] : '<front>',
    t('This action cannot be undone.'),
    t('Delete'), t('Cancel')
  );
}

/**
 * Form submit handler for mollom_report_form().
 */
function mollom_report_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $entity = $form_state['values']['entity'];
    $id = $form_state['values']['id'];

    // Load the Mollom session data.
    if ($entity == 'session') {
      $data = new stdClass;
      $data->session = $id;
    }
    else {
      $data = mollom_data_load($entity, $id);
    }

    // Send feedback to Mollom, if we have session data.
    if (isset($data) && isset($data->session) && isset($form_state['values']['feedback']) && $form_state['values']['feedback'] != 'none') {
      // @todo Check the actual reponse.
      mollom('mollom.sendFeedback', array(
        'session_id' => $data->session,
        'feedback' => $form_state['values']['feedback'],
      ));
      drupal_set_message(t('The content was successfully reported as inappropriate.'));
    }

    // Delete the content. The callback should take care of proper deletion and
    // cache clearing on its own.
    foreach (mollom_get_form_info() as $form_id => $info) {
      if (!isset($info['entity']) || $info['entity'] != $entity) {
        continue;
      }
      // If there is a 'report delete callback', invoke it.
      if (isset($info['report delete callback']) && function_exists($info['report delete callback'])) {
        $function = $info['report delete callback'];
        $function($entity, $id);
        break;
      }
    }
  
    $form_state['redirect'] = '<front>';
  }
}

/**
 * Return a list of the possible feedback options for content.
 */
function _mollom_feedback_options() {
  return array(
    '#type' => 'radios',
    '#title' => t('Optionally report this to Mollom'),
    '#options' => array(
      'none' => t("Don't send feedback to Mollom"),
      'spam' => t('Report as spam or unsolicited advertising'),
      'profanity' => t('Report as obscene, violent or profane content'),
      'low-quality' => t('Report as low-quality content or writing'),
      'unwanted' => t('Report as unwanted, taunting or off-topic content'),
    ),
    '#default_value' => 'none',
    '#description' => t("Mollom is a web service that helps you moderate your site's content: see <a href=\"http://mollom.com\">http://mollom.com</a> for more information. By sending feedback to Mollom, you teach Mollom about the content you like and dislike, allowing Mollom to do a better job helping you moderate your site's content. If you want to report multiple posts at once, you can use Mollom's bulk operations on the content and comment administration pages."),
  );
}

