<?php

function amazon_settings_form($form_state) {
  $locales = amazon_locale_cache();
  $options = array();
  foreach ($locales as $locale => $data) {
    $options[$locale] = $data['name'];
  }
  $form['amazon_locale'] = array(
    '#type' => 'select',
    '#title' => t('Amazon locale'),
    '#default_value' => variable_get('amazon_locale', 'US'),
    '#options' => $options,
    '#description' => t('Amazon.com uses separate product databases and Ecommerce features in different locales; pricing and availability information, as well as product categorization, differs from one locale to the next. Be sure to select the locale your site will be running in.'),
  );

  $form['amazon_associate_setting'] = array(
    '#type' => 'select',
    '#title' => t('Amazon referral settings'),
    '#default_value' => variable_get('amazon_associate_setting', 'association'),
    '#options' => array(
      'association' => t('Use the Drupal Association\'s associate ID'),
      'author' => t('Use the module developer\'s associate ID'),
      'custom' => t('Use your own associate ID'),
      'none' => t('Do not use any associate ID')
    ),
    '#description' => t('Outgoing links to Amazon.com can include an associate ID code. When shoppers purchase Amazon products via one of those links, the associate ID is used to determine who should receive a referral bonus from Amazon.com.'),
  );

  $form['amazon_custom_associate_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Your associate ID'),
    '#description' => t('Enter your own associate ID to receive referral bonuses when shoppers purchase Amazon products via your site.'),
    '#default_value' => variable_get('amazon_custom_associate_id', ''),
    '#prefix' => '<div id="amazon-associate-id-wrapper">',
    '#suffix' => '</div>',
  );
  if (variable_get('amazon_associate_setting', 'author') != 'custom') {
    $form['amazon_custom_associate_id']['#prefix'] = '<div id="amazon-associate-id-wrapper" class="js-hide">';
  }

  $form['amazon_aws_access_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Amazon API key'),
    '#description' => t('Applications must use a unique key to access Amazon services. One is supplied with the Drupal Amazon API module by default, but developers building high-traffic custom modules on the Amazon API may want to enter a custom key here.'),
    '#default_value' => variable_get('amazon_aws_access_key', '0CD9RCQYM0TBVH55NB82'),
  );

  // Now add the Javascript that does the fancy hide/show effect on
  // The associate ID field.
  $js = <<<JSCRIPT
jQuery(
  function(){ jQuery("#edit-amazon-associate-setting").bind("change", function() {
  if (this.value == 'custom') {
    $("#amazon-associate-id-wrapper").show('fast');
  }
  else {
    $("#amazon-associate-id-wrapper").hide('fast');
  }
  return false;
})
});
JSCRIPT;

  drupal_add_js($js, 'inline');

  return system_settings_form($form);
}

function amazon_storage_settings_form() {
  $period = array(0 => t('Never'));
  $period += drupal_map_assoc(array(43200, 86400, 604800), 'format_interval');
  $form['amazon_refresh_schedule'] = array(
    '#type' => 'select',
    '#title' => t('Amazon refresh schedule'),
    '#description' => t('Locally cached information on Amazon items must be refreshed regularly to keep pricing and stock information up to date. Cron must be enabled for this function to work properly.'),
    '#default_value' => variable_get('amazon_refresh_schedule', 86400),
    '#options' => $period
  );

  $form['amazon_extra_data'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Store extra data'),
    '#default_value' => amazon_get_cacheable_chunk_list(),
    '#options' => array(
      'Author' => t('Book authors'),
      'Artist' => t('Musical artists'),
      'Actor' => t('Film actors'),
      'Director' => t('Film directors'),
      'ImageSets' => t('Product images'),
      'Feature' => t('Product features'),
      'EditorialReviews' => t('Product editorial reviews and descriptions'),
    ),
    '#description' => t('In addition to basic pricing, title, and availability information, Amazon stores detailed data about each product. To store some of that information locally for use on your site, select it from the list above. Keep in mind that information not selected above will be unavailable for use in product listings built using the Views module.'),
  );

  return system_settings_form($form);
}

function _amazon_upgrade() {
  // Stub function intended to update from AmazonTools module for Drupal 5
}
