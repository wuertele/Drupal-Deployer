// $Id$

// The following line is necessary since our JavaScript will execute before the
// Vertical tabs code.
Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.xmlsitemap = function() {
  var vals = [];
  //vals.push(Drupal.t('Included in sitemap'));
  var priority = $('#edit-xmlsitemap-priority-override').val();
  vals.push(Drupal.t('Priority @priority', { '@priority': priority }));
  return vals.join('<br />');
}
