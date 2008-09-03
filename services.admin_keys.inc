<?php
// $Id$
/**
 * @author Services Dev Team
 * @file
 *  Manage security keys.
 */

/**
 * List all registered application keys.
 */
function services_admin_keys_list() {
  $header = array(
    array('data' => t('User'),    'field' => 'uid', 'sort' => 'asc'),
    array('data' => t('Title'),   'field' => 'application_title'),
    array('data' => t('Domain'),  'field' => 'application_url'),
    array('data' => t('Type'),    'field' => 'application_type'),
    array('data' => t('Operations'), 'colspan' => '1')
  );
  $result = pager_query('SELECT * FROM {oauth_cool_auth}'. tablesort_sql($header), 10);
  while ($key = db_fetch_array($result)) {
    $keys[$key['uid']] = $key;
  }
  $rows = array();

  foreach ($keys as $kid => $key) {
    if ($key['application_type'] == 0) {
      $option = t('Web');
    }
    elseif ($key['application_type'] == 1) {
      $option = t('Desktop');
    }
    else {
      $option = t('Mobile');
    }

    $row = array();
    $row[] = $kid;
    $row[] = $key['application_title'];
    $row[] = $key['application_url'];
    $row[] = $option;
    //$row[] = array('data' => l(t('edit'), 'admin/build/services/keys/edit'. $key->application_key));
    $row[] = array('data' => l(t('delete'), 'admin/build/services/registered/'. $key['application_key']));
    $rows[] = $row;
  }

  if (empty($rows)) {
    return t('No website registered to use cool auth on server.');
  }

  return theme('table', $header, $rows);
}
