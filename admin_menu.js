/* $Id$ */

$(document).ready(function() {
  // Apply margin-top if enabled.
  if (Drupal.settings.admin_menu_margin_top == 1) {
  	$('body').css('marginTop', '20px');
  }

  // Hover emulation for IE 6.
  if ($.browser.msie && parseInt(jQuery.browser.version) == 6) {
    $('#admin-menu li').hover(function() {
      $(this).addClass('iehover');
    }, function() {
      $(this).removeClass('iehover');
    });
  }
  
  // Delayed mouseout.
  $('#admin-menu li').hover(function() {
    // Stop the timer.
    clearTimeout(this.sfTimer);
    // Display child lists.
    $('> ul', this).css('left', 'auto')
      // Immediately hide nephew lists.
      .parent().siblings('li').children('ul').css('left', '-999em');
  }, function() {
    // Start the timer.
    var uls = $('> ul', this);
    this.sfTimer = setTimeout(function() {
      uls.css('left', '-999em');
    }, 400);
  });
});
