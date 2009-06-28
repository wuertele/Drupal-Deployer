// $Id$

Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.xmlsitemap = function() {
  var vals = [];
  vals.push(Drupal.t('Included in sitemap'));
  var priority = $('#edit-xmlsitemap-priority-override').val();
  vals.push(Drupal.t('@priority priority', { '@priority': priority }));
  return vals.join(', ');
}
