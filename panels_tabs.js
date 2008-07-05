// $Id$

if (Drupal.jsEnabled) {
  $(document).ready(function(){ 

    // This function can be used to update the links of a pager to contain
    // the currently selected fragment. This allows you to click a link of a
    // pager in a panel, and when the page reloads, the correct tab will
    // be opened.
    updateLinks = function(id, fragment) {
      $('#tabs-'+ id +' .pager a').each(function() {
        var oldURI = $(this).attr('href');
        if (oldURI.lastIndexOf('#') > 0) {
          oldURI = oldURI.substr(0, oldURI.lastIndexOf('#'));
        }
        var newURI = oldURI + fragment;
        $(this).attr('href', newURI);
      });
    };    

    // Resize tabs as configured.
    for (var id in Drupal.settings.panelsTabs) {
      var $tabs = $('#tabs-'+ id +' > ul > li');

      switch (Drupal.settings.panelsTabs[id].fillingTabs) {
        case 'equal':
          $tabs.each(function() {
            $(this)
            .css('width', (99 / $tabs.length) +'%')
            .css('overflow', 'hidden');
          });
          break;

        case 'smart':
          var tabTextLengths = new Array();
          var totalTabTextLength = 0;

          $tabs
          // Collect the length of the text on each tab and the total length.
          .each(function(i) {
            totalTabTextLength += tabTextLengths[i] = $(this).text().length;
          })
          // Use the collected text lenghts to do smart tab width scaling.
          .each(function(i) {
            $(this)
            .css('width', (tabTextLengths[i] / totalTabTextLength * 100) +'%')
            .css('overflow', 'hidden');
          });
          break;

        case 'disabled':
        default:
          // Nothing should happen if we 
          break;
      }

      // Update links when a tab is clicked.
      $('#tabs-'+ id +' > ul > li > a').each(function() {
        $(this).click(function(_id, fragment) {
          return function() { updateLinks(_id, fragment); };
        }(id, $(this).attr('href')));
      });
    }

    // Update all links on load (automatically find the name of the tabset!).
    var currentURI = "" + window.location;
    var fragment = currentURI.substr(currentURI.indexOf('#'));
    var id = $('a[@href*="'+ fragment +'"]')
    .parent() // the parent li
    .parent() // the parent ul
    .parent() // the parent div (the actual tabset div!)
    .attr('id')
    .substr("tabs-".length);
    updateLinks(id, fragment);
  });
}
