// $Id$

Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.xmlsitemap = function() {
  var vals = [];

  // Inclusion select field
  var status = $('#edit-xmlsitemap-status').val();
  var status_text = $("#edit-xmlsitemap-status option[value='" + status + "']").text();
  vals.push(Drupal.t('Inclusion: @value', { '@value': status_text }));

  // Priority select field
  var priority = $('#edit-xmlsitemap-priority').val();
  var priority_text = $("#edit-xmlsitemap-priority option[value='" + priority + "']").text();
  vals.push(Drupal.t('Priority: @value', { '@value': priority_text }));

  return vals.join('<br />');
}
