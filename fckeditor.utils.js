// $Id$
// map of instancename -> boolean indicating if the instance has loaded
var fckIsRunning = {};
// map of instancename -> boolean indicating if the instance has launched
var fckIsLaunching = [];
// list of id's of converted textareas
var fckLaunchedTextareaId = [];
// list of instance names
var fckLaunchedJsId = [];
// map of instancename -> boolean indicating if the instance is running for the first time (false indicates it has been toggled)
var fckFirstrun = {};
// map of instancename -> FCKeditor object
var fckInstances = {};

var fckIsIE = ( /*@cc_on!@*/false ) ? true : false ;

function Toggle(textareaID, TextTextarea, TextRTE)
{
  var swtch = $('#switch_'+textareaID);
  
  // check if this FCKeditor was initially disabled
  if (fckInstances[textareaID].defaultState == 0) {
    fckInstances[textareaID].defaultState = 1;
    fckInstances[textareaID].ReplaceTextarea();
    swtch.text(TextTextarea);
    // simply return: ReplaceTextarea will take the contents of the textarea for us
    return;
  }

  var textArea = $('#'+textareaID);
  var textAreaContainer = $('#'+textareaID).parents('.resizable-textarea');
  var editorFrame = $('#'+textareaID+'___Frame');  
  var editorInstance = FCKeditorAPI.GetInstance(textareaID);
  var text;
  
  // execute the switch
  if (textArea.is(':hidden')) {
    // switch from fck to textarea
    // TODO reactivate teaser.js stuff
    swtch.text(TextRTE);
    
    text = editorInstance.GetData(true);
    
    // check if we have to take care of teasers
    var teaser = FCKeditor_TeaserInfo(textareaID);
    
    if(teaser) {
      var t = text.indexOf('<!--break-->');
      if (t != -1) {
        $('#'+teaser.textarea).val(FCKeditor_trim(text.slice(0,t)));
        text = FCKeditor_trim(text.slice(t+12));
        
        $('#'+teaser.textarea).parent().show();
        $('#'+teaser.textarea).attr('disabled', '');
        if ($('input.teaser-button').attr('value') != Drupal.t('Join summary')) {
          try {$('input.teaser-button').click();} catch(e) {$('input.teaser-button').val(Drupal.t('Join summary'));}
        }
      } else {
        $('#'+teaser.textarea).attr('disabled', 'disabled');
        if ($('input.teaser-button').attr('value') != Drupal.t('Split summary at cursor')) {
          try {$('input.teaser-button').click();} catch(e) {$('input.teaser-button').val(Drupal.t('Split summary at cursor'));}
        }
      }
      
      $('div.teaser-button-wrapper').show();
    } else {
      text = textArea.val();
    }
    textArea.val(text);
    
    textArea.show();
    textAreaContainer.show();
    editorFrame.hide();
    
    // fix the grippie
    if(textAreaContainer.length > 0) { // if we're in a container, textarea resizing is enabled
      var grippie = $('div.grippie', textAreaContainer).get(0);
      grippie.style.marginRight = '0px';
      grippie.style.marginRight = (grippie.offsetWidth - textArea.get(0).offsetWidth) +'px';
    }
  } else {
    // switch from textarea to fck
    swtch.text(TextTextarea);
    
    // check if we have to take care of teasers
    var teaser = FCKeditor_TeaserInfo(textareaID);
    
    if(teaser) {
      if ($('#'+teaser.textarea).val().length > 0) {
        text = $('#'+teaser.textarea).val() + '\n<!--break-->\n' + textArea.val();
      } else {
        text = textArea.val();
      }
      $('#'+teaser.textarea).attr('disabled', '');
      $('div.teaser-button-wrapper').hide();
      $('#'+teaser.textarea).parent().hide();
      $('#'+teaser.checkbox).parent().show();
    } else {
      text = textArea.val();
    }
    
    editorInstance.SetData(text, true);
    
    // Switch the DIVs display.
    textArea.hide();
    textAreaContainer.hide();
    editorFrame.show();
  }
}

function CreateToggle(elId, jsId, fckeditorOn)
{
  var ta = $('#'+elId);
  var ta2 = $('fck_' + jsId);

  ta2.value = ta.value;
  ta.parentNode.insertBefore(ta2, ta);
  if (fckeditorOn)
  ta.style.display = 'none';
  else
  ta2.style.display = 'none';
}

// The FCKeditor_OnComplete function is a special function called everytime an
// editor instance is completely loaded and available for API interactions.
function FCKeditor_OnComplete( editorInstance )
{
  fckIsRunning[editorInstance.Name] = true ;
  fckLaunchedTextareaId.push(editorInstance.Config['TextareaID']) ;
  fckLaunchedJsId.push(editorInstance.Name) ;
  fckFirstrun[editorInstance.Name] = true;

  // Enable the switch button. It is disabled at startup, waiting the editor to be loaded.
  $('#switch_' + editorInstance.Name).show();

  editorInstance.Events.AttachEvent('OnAfterLinkedFieldUpdate', FCKeditor_OnAfterLinkedFieldUpdate);
  
  
  var teaser = FCKeditor_TeaserInfo(editorInstance.Name);
  
  if(teaser) {
    if ($('#'+teaser.textarea).val().length > 0) {
      var text = $('#'+teaser.textarea).val() + '\n<!--break-->\n' + editorInstance.GetData();
      editorInstance.SetData(text);
    }
    
    $('#'+teaser.textarea).attr('disabled', '');
    $('div.teaser-button-wrapper').hide();
    $('#'+teaser.textarea).parent().hide();
    $('#'+teaser.checkbox).parent().show();
  }
  
  // cope with resizable
  var container = $(editorInstance.LinkedField).parents('div.resizable-textarea');
  if(container.length) {
    container.after($('iframe', container));
    container.hide();
  }
  
  //Img_Assist integration
  IntegrateWithImgAssist();	  
}

function FCKeditor_TeaserInfo(taid) {
  // TODO add caching
  var teasers = {};
  for(x in Drupal.settings.teaser) {
    teasers[Drupal.settings.teaser[x]] = x;
  }
  
  if (teasers[taid]) {
    return {
      textarea : teasers[taid],
      checkbox : Drupal.settings.teaserCheckbox[teasers[taid]],
    };
  } else {
    return null;
  }
}

function FCKeditor_OnAfterLinkedFieldUpdate(editorInstance) {
  // make a list of all elements that are associated with teaser-js's
  var teasers = {};
  for(x in Drupal.settings.teaser) {
    teasers[Drupal.settings.teaser[x]] = x;
  }
  
  var textArea = editorInstance.LinkedField;
  var taid = textArea.id;
  if($(textArea).is(':hidden')) {
    var text = editorInstance.GetData();
    textArea.value = text;
    // only update the teaser field if this field is associated with a teaser field
    if(teasers[taid]) {
      var t = text.indexOf('<!--break-->');
      var teaser = $('#'+teasers[taid]);
      if(t != -1) {
        teaser.val(FCKeditor_trim(text.slice(0,t)));
        textArea.value = FCKeditor_trim(text.slice(t+12));
      } else {
        teaser.val('');
        teaser.attr('disabled', 'disabled');
        
        var teaserbutton = $('input.teaser-button');
        var teaserbuttontxt = Drupal.t('Join summary');
        
        if (teaserbutton.attr('value') == teaserbuttontxt) {
          try {
            teaserbutton.click();
          } catch(e) {
            teaserbutton.val(teaserbuttontxt);
          }
        }
      }
    }
  }
}

function IntegrateWithImgAssist()
{
	var link = document.getElementsByTagName("a");
	for (var i = 0; i < link.length; i++) {
		cl = link[i].className;
		if ( cl == "img_assist-link") {
			link[i].href = link[i].href.replace("/load/textarea", "/load/fckeditor");
		}
	}
}

Drupal.behaviors.fckeditor = function(context) {
  $('textarea.fckeditor:not(.fckeditor-processed)', context).each(function() {
    var textarea = $(this).addClass('fckeditor-processed');
    
    var taid = textarea.attr('id');
    if (fckInstances[taid]) {
      var editorInstance = fckInstances[taid];
      
      if(editorInstance.defaultState == 1) {
        editorInstance.ReplaceTextarea();
      }
    }
  });
}

function FCKeditor_trim(text) {
  return text.replace(/^\s+/g, '').replace(/\s+$/g, '');
}