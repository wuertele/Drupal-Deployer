// $Id$

(function ($) {

/**
 * Open Mollom privacy policy link in a new window.
 *
 * Required for valid XHTML Strict markup.
 */
Drupal.behaviors.mollomPrivacy = function (context) {
  $('.mollom-privacy a', context).click(function () {
    this.target = '_blank';
  });
};

/**
 * Attach click event handlers for CAPTCHA links.
 */
Drupal.behaviors.mollomCaptcha = function (context) {
  $('.mollom-audio-captcha', context).click(getAudioCaptcha);
  $('.mollom-image-captcha', context).click(getImageCaptcha);
};

function getAudioCaptcha() {
  var context = $(this).parents('.form-item').parent();

  // Extract the Mollom session ID from the form:
  var mollomSessionId = $('input.mollom-session-id', context).val();

  // Retrieve an audio CAPTCHA:
  var data = $.get(Drupal.settings.basePath + 'mollom/captcha/audio/' + mollomSessionId,
    function(data) {
      // When data is successfully loaded, replace
      // contents of captcha-div with an audio CAPTCHA:
      $('.mollom-captcha', context).parent().html(data);

      // Add an onclick-event handler for the new link:
      $('.mollom-image-captcha', context).click(getImageCaptcha);
   });
   return false;
}

function getImageCaptcha() {
  var context = $(this).parents('.form-item').parent();

  // Extract the Mollom session ID from the form:
  var mollomSessionId = $('input.mollom-session-id', context).val();

  // Retrieve an image CAPTCHA:
  var data = $.get(Drupal.settings.basePath + 'mollom/captcha/image/' + mollomSessionId,
    function(data) {
      // When data is successfully loaded, replace
      // contents of captcha-div with an image CAPTCHA:
      $('.mollom-captcha', context).parent().html(data);

      // Add an onclick-event handler for the new link:
      $('.mollom-audio-captcha', context).click(getAudioCaptcha);
   });
   return false;
}

})(jQuery);
