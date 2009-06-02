<?php
// $Id$

/**
 * Fetch a short blurb string about module maintainership and sponsors.
 *
 * This message will be FALSE in 'official' releases.
 */
function _xmlsitemap_get_blurb($check_version = TRUE) {
  static $blurb;

  if (!isset($blurb)) {
    $blurb = FALSE;
    $info = drupal_parse_info_file(drupal_get_path('module', 'xmlsitemap') . '/xmlsitemap.info');
    if (!$check_version || !isset($info['version']) || preg_match('/dev|alpha|beta|rc/i', $info['version'])) {
      $sponsors = array(
        l('Symantec', 'http://www.symantec.com/'),
        l('WebWise Solutions', 'http://www.webwiseone.com/'),
        l('Volacci', 'http://www.volacci.com/'),
      );
      // Don't extract the following string for translation.
      $blurb = strtr('<p>Thank you for helping test the XML sitemap module rewrite. Please consider helping offset developer free time by <a href="@link-donate">donating</a> or if your company is interested in sponsoring the rewrite or a specific feature, please <a href="@link-contact">contact the developer</a>. Thank you to the following current sponsors: !sponsors.</p>', array('@link-review' => 'http://drupalmodules.com/module/xml-sitemap', '@link-donate' => 'http://davereid.chipin.com/', '@link-contact' => 'http://davereid.net/contact', '!sponsors' => implode(', ', $sponsors)));
    }
  }

  return $blurb;
}

/**
 * Check the status of all hook_requirements() from xmlsitemap modules.
 *
 * @param $return_only
 *   If TRUE, will return the result, otherwise it will show a message.
 * @return
 *   TRUE if there is a warning or error requirement, or FALSE otherwise.
 */
function xmlsitemap_check_status($return_only = FALSE) {
  // Load .install files
  include_once './includes/install.inc';
  drupal_load_updates();
  $warnings = FALSE;

  foreach (module_implements('requirements') as $module) {
    if (strpos($module, 'xmlsitemap') !== FALSE) {
      $requirements = module_invoke($module, 'requirements', 'runtime');
      if (drupal_requirements_severity($requirements) >= REQUIREMENT_WARNING) {
        $warnings = TRUE;
        break;
      }
    }
  }

  if ($warnings && !$return_only && user_access('administer site configuration')) {
    drupal_set_message(t('One or more problems were detected with your sitemap configuration. Please check the <a href="@status-report">status report</a> for more information.', array('@status-report' => url('admin/reports/status'))), 'warning', FALSE);
  }

  return $warnings;
}
