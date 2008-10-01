// $Id$

/**
 * Attach client side error classes on tabs and select first tab in error
 */
Drupal.behaviors.cckFieldgroupTabsErrors = function (context) {
  $('.ui-tabs-nav:not(.cckFieldgroupTabsErrors-processed)').addClass('cckFieldgroupTabsErrors-processed').children('li').each( function() {
    if ($($(this).find('a').attr('href')).find('div.form-item .error').size()) {
      $(this).addClass('error').addClass('selected');
    }
    $('#tabs-fieldgroup_tabs > ul').tabs('select', $('.ui-tabs-nav').children('li.error:first').find('a').attr('href'));
  })
}
