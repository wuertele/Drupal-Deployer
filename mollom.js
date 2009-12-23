// $Id$

(function ($) {

/**
 * Attach click event handlers for CAPTCHA links.
 */
Drupal.behaviors.mollom = function(context) {
  $('a.mollom-audio-captcha', context).click(getAudioCaptcha);
  $('a.mollom-image-captcha', context).click(getImageCaptcha);
}

function getAudioCaptcha() {
  var context = $(this).parents('.form-item').parent();

  // Extract the Mollom session ID from the form:
  var mollomSessionId = $('input.mollom-session-id', context).val();

  // Retrieve an audio CAPTCHA:
  var data = $.get(Drupal.settings.basePath + 'mollom/captcha/audio/' + mollomSessionId,
    function(data) {
      // When data is successfully loaded, replace
      // contents of captcha-div with an audio CAPTCHA:
      $('a.mollom-captcha', context).parent().html(data);

      // Add an onclick-event handler for the new link:
      $('a.mollom-image-captcha', context).click(getImageCaptcha);
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
      $('a.mollom-captcha', context).parent().html(data);

      // Add an onclick-event handler for the new link:
      $('a.mollom-audio-captcha', context).click(getAudioCaptcha);
   });
   return false;
}

})(jQuery);
