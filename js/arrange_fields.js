// $Id$ 

// Some global variables we will need...
var arrangeFieldsStartupHeight;
var arrangeFieldsGreatestHeight;
var arrangeFieldsDragging;
var arrangeFieldsDialogConfigField;
var arrangeFieldsDialogConfigFieldId;
var arrangeFieldsDialogConfigObj = new Object();  // we will use this later like a 2d assoc array, for keeping up with dialog settings for fields.


Drupal.behaviors.arrangeFieldsStartup = function() {
 
  // We we have any dialog config settings saved from a previous session,
  // let's load them.
  if (Drupal.settings.arrangeFieldsDialogConfigObj != null) {
    arrangeFieldsDialogConfigObj = Drupal.settings.arrangeFieldsDialogConfigObj;
  }
  
  // This section of code makes the "handle" appear for draggable items, which users
  // may use to drag the item, or for important links to appear there.
  $(".arrange-fields-container .draggable-form-item").bind("mouseenter", function(event) {
    var hand = $(this).find(".arrange-fields-control-handle");
    if (arrangeFieldsDragging != true) {
      $(hand).show();
    }
  });

  $(".arrange-fields-container .draggable-form-item").bind("mouseleave", function(event) {
    var hand = $(this).find(".arrange-fields-control-handle");
    if (arrangeFieldsDragging != true) {
      $(hand).hide();
    }
  });
  

  // This actually makes the draggable items draggable.
  $(".arrange-fields-container .draggable-form-item").draggable({
    stop: function(event, ui) { arrangeFieldsRepositionToGrid(false); },
    containment: ".arrange-fields-container", 
    scroll: true,
    grid : [10,10],
    start: function(event, ui) {arrangeFieldsDragging = true;},
    stop:  function(event, ui) {arrangeFieldsDragging = false;}
  });

  
  arrangeFieldsStartupHeight = 0;
  arrangeFieldsGreatestHeight = 0; 

  $(".arrange-fields-container .draggable-form-item:not(.draggable-form-item-fieldset) textarea").resizable();
  $(".arrange-fields-container .draggable-form-item:not(.draggable-form-item-fieldset) .form-text").resizable({
        handles: 'e'
  });  
  
  // We do the "true" if this is a totally fresh new form, with no
  // position data already saved.  
  var startup = true;
  
  try {
    if (Drupal.settings.arrangeFieldsNotNewForm == true) {
      startup = false;
    }
  }
  catch (exception) {}


  //Set up the config dialog....
  $("#arrange-fields-config-dialog").dialog({
    autoOpen: false,
    height: 200,
    width: 300,
    buttons: {
      "Apply" : function() {
        arrangeFieldsApplyDialogChanges();
      },
      "Cancel" : function() { $(this).dialog("close"); }
    }  
  });
  
  
  // Make sure everything starts off on a grid line.
  arrangeFieldsRepositionToGrid(startup); 
  
}


/**
  * Repositions all the draggable elements to the grid lines.
  */
function arrangeFieldsRepositionToGrid(startup) {


  var gridWidth = 10;
  $(".arrange-fields-container .draggable-form-item").each(function (index, element) {
    var postop = $(element).css("top");
    var posleft = $(element).css("left");

    postop = $(element).css("top").replace("px", "");
    posleft = $(element).css("left").replace("px", "");
    
    if (postop == "auto") postop = 0; 
    if (posleft == "auto") posleft = 0;
    
    if (startup == true && postop == 0) {
      // Since this is a new form, with values not set yet,
      // let's assign the postop based on the running startupHeight
      // value.
      postop = arrangeFieldsStartupHeight;
      arrangeFieldsStartupHeight += $(element).height() + 20; 
    }
      
    if (parseInt(postop) > parseInt(arrangeFieldsGreatestHeight)) {
      arrangeFieldsGreatestHeight = parseInt(postop);
    }
    
    // We want to round the top and left positions to the nearest X (gridWidth)
    var newTop = Math.round(postop/gridWidth) * gridWidth;
    var newLeft = Math.round(posleft/gridWidth) * gridWidth;
    
    var diffLeft = "-" + (posleft - newLeft);
    var diffTop = "-" + (postop - newTop);
    
    if (posleft < newLeft) { diffLeft = newLeft - posleft; }
    if (postop < newTop) { diffTop = newTop - postop; }

    if (newTop < 0) newTop = 0;
    if (newLeft < 0) newLeft = 0;
    
   $(element).css("top", newTop + "px");
   $(element).css("left", newLeft + "px");

   // We want to resize the container as we go to make sure we don't run out of
   // room, and to make sure the user can always drag things below the rest of
   // the items on the form.
   $(".arrange-fields-container").css("height", (parseInt(arrangeFieldsGreatestHeight) + 500) + "px");
 
  });
  
}

/**
  * This method will save the position, width, and height, and other important
  * information of the draggable items on the page.
  *
  */
function arrangeFieldsSavePositions() {
  
  var dataString = "";
  var maxBottom = 0;
  
  $(".arrange-fields-container .draggable-form-item").each(function (index, element) {
   var id = $(element)[0].id;
   var top = $(element).css("top");
   var left = $(element).css("left");
   var element_type = "";
   // Now, we want to find the element inside...
   
   //var width = $(element).width();
   //var height = $(element).height();
   var width = 0;
   var height = 0;
   
   // Attempt to find either a text area or a textfield...
   // But, only do this if we are NOT within a fieldset!
   if ($(element).hasClass("draggable-form-item-fieldset") == false) {
     var test = $(element).find("textarea");
     width = $(test).width();
     height = $(test).height();
     if (width != null) element_type = "textarea";
     
     if (width == null) {
       test = $(element).find("input:text");
       width = $(test).width();
       height = $(test).height();
       if (width != null) element_type = "input";
     }
   }

   if (width == null) {
     width = height = 0;
   }   
   
   dataString += id + "," + top + "," + left + "," + element_type + "," + width + "px," + height + "px,";
   
   // Do we have any extra data for this element?  Perhaps data from the config dialog?
   if (arrangeFieldsDialogConfigObj[id] != null) {
     dataString += arrangeFieldsDialogConfigObj[id]["wrapperWidth"] + ",";
     dataString += arrangeFieldsDialogConfigObj[id]["wrapperHeight"] + ",";
     dataString += arrangeFieldsDialogConfigObj[id]["labelDisplay"] + ",";
     dataString += arrangeFieldsDialogConfigObj[id]["labelVerticalAlign"] + ",";
   }
   
   dataString += ";";
   
   var bottom = parseInt(top) + $(element).height();
   if (bottom > maxBottom) {
     maxBottom = bottom;
   }
   
  });
   
  // This maxBottom value tells us how tall the container needs to be on the node/edit page
  // for this form.
  dataString += "~~maxBottom~~," + maxBottom + "px";
  
  $("#edit-arrange-fields-position-data").val(dataString);

}

function arrangeFieldsConfirmReset() {
  var x = confirm("Are you sure you want to reset the position data for these fields?  This action cannot be undone.");
  return x;
}

function arrangeFieldsPopupEditField(type, field) {
  var popup_url = Drupal.settings.basePath + "arrange-fields/popup-edit-field&type_name=" + type + "&field=" + field;
  var win_title = "myPopupWin";
  var win_options = "height=700,width=700,scrollbars=yes";
  
  var myWin = window.open(popup_url, win_title, win_options);
  myWin.focus();

}

function arrangeFieldsClosePopup() {
  // Closes the popup and saves the form in the opener window.
  opener.arrangeFieldsSavePositions();
  opener.document.getElementById("arrange-fields-position-form").submit();
  window.close();
}

/**
 * We are opening the config dialog.  Let's reset the values
 *
 **/ 
function arrangeFieldsPopupConfigField(field, field_type) {
  var dia = $("#arrange-fields-config-dialog");
  dia.dialog("option", "title", "Configure " + field);
  
  arrangeFieldsDialogConfigField = field;
  var fieldId = "edit-" + field + "-draggable-wrapper";
  // if this is a fieldset, the fieldId is slightly different.
  if (field_type == "fieldset") {
    fieldId = "edit-" + field + "-fieldset-draggable-wrapper";
  }
  arrangeFieldsDialogConfigFieldId = fieldId;
  
  
  // Is this field in the dialog config obj yet?
  if (arrangeFieldsDialogConfigObj[fieldId] == null) {
    arrangeFieldsDialogConfigObj[fieldId] = new Object();
  }

  // Make sure the properties have initial, non-null values.
  if (arrangeFieldsDialogConfigObj[fieldId]["wrapperHeight"] == null) {
    arrangeFieldsDialogConfigObj[fieldId]["wrapperHeight"] = "";
  }
  if (arrangeFieldsDialogConfigObj[fieldId]["wrapperWidth"] == null) {
    arrangeFieldsDialogConfigObj[fieldId]["wrapperWidth"] = "";
  }
  if (arrangeFieldsDialogConfigObj[fieldId]["labelDisplay"] == null) {
    arrangeFieldsDialogConfigObj[fieldId]["labelDisplay"] = "";
  }
  if (arrangeFieldsDialogConfigObj[fieldId]["labelVerticalAlign"] == null) {
    arrangeFieldsDialogConfigObj[fieldId]["labelVerticalAlign"] = "";
  }
  

  // Let's reset the inputs in the dialog to use whatever is in the
  // config obj.
  dia.find("input[name=af-dialog-width]").val(arrangeFieldsDialogConfigObj[fieldId]["wrapperWidth"]);
  dia.find("input[name=af-dialog-height]").val(arrangeFieldsDialogConfigObj[fieldId]["wrapperHeight"]);
  dia.find("input[name=af-dialog-label-display]").each(function() {
    if ($(this).val() == arrangeFieldsDialogConfigObj[fieldId]["labelDisplay"]
        || $(this).val() == arrangeFieldsDialogConfigObj[fieldId]["labelDisplay"] + "-block") {
      $(this).attr("checked", "checked");
    }
  });
  
  
  
  dia.dialog('open');
}

/**
 * Apply the changes the user has entered into the dialog.
 **/
function arrangeFieldsApplyDialogChanges() {
  var dia = $("#arrange-fields-config-dialog");
  
  var wrapperWidth = dia.find("input[name=af-dialog-width]").val();
  var wrapperHeight = dia.find("input[name=af-dialog-height]").val();
  var labelDisplay = dia.find("input[name=af-dialog-label-display]:checked").val();
  
  // Remove trouble characters, if they exist.
  wrapperWidth = wrapperWidth.replace(";", "");
  wrapperHeight = wrapperHeight.replace(";", "");
  
  // Save these values to the config obj
  var field = arrangeFieldsDialogConfigField;
  var fieldId = arrangeFieldsDialogConfigFieldId;
  
  arrangeFieldsDialogConfigObj[fieldId]["wrapperWidth"] = wrapperWidth;
  arrangeFieldsDialogConfigObj[fieldId]["wrapperHeight"] = wrapperHeight;
  arrangeFieldsDialogConfigObj[fieldId]["labelDisplay"] = labelDisplay;
    
  if (wrapperWidth == "") { wrapperWidth = "auto"; }
  if (wrapperHeight == "") { wrapperHeight = "auto"; }
  if (labelDisplay == "") { labelDisplay = "block"; }  
  
  // Let's actually affect these changes on the page.
  $("#" + fieldId).css("width", wrapperWidth);
  $("#" + fieldId).css("height", wrapperHeight);
  
  
  var valign = "top";
  // Because of the way IE handles inline-block displays with radio buttons,
  // we have to change inline-block to simply inline for radios and checkboxes.
  // Isn't IE great?
  var radioDisplay = labelDisplay;
  if (labelDisplay == "inline-block") { radioDisplay = "inline"; }
  var boolRadio = false;
  
  // Grab all the sibling elements under the wrapper and make them
  // have this display property.
  $("#" + fieldId + " .form-item").children(":not(.description)").each(function() {
    
    // If these are radios/checkboxes, then also apply this style to the div's there.
    $("#" + fieldId + " .form-item .form-radios, #" + fieldId + " .form-item .form-checkboxes").children().each(function () {
      $(this).css("display", radioDisplay);
      valign = "middle";
      boolRadio = true;
    });

    if (!boolRadio) {
      $(this).css("display", labelDisplay);
    }
    else { // we are dealing with radio buttons or checkboxes.
      $(this).css("display", radioDisplay);
      arrangeFieldsDialogConfigObj[fieldId]["labelDisplay"] = radioDisplay;
    }
       
  });
    
  // Make the label look right...  
  $("#" + fieldId + " .form-item label").css("vertical-align", valign);

  
  arrangeFieldsDialogConfigObj[fieldId]["labelVerticalAlign"] = valign;

  dia.dialog('close');
}