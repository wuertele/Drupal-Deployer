// $Id$

/**
 * @file node_export_file_admin.js
 *  Admin DHTML for node_export_file module.
 **/

$(function () {
  var assets_state = false; // Start hidden
  var assets_div = $('div#edit-node-export-file-assets-path-wrapper');

  // On load, hide or show the assets path
  $('input[@name=node_export_file_mode]').each(function () {
    if (this.checked) {
      assets_state = _node_export_file_get_state($(this).val())

      return; // Break the loop, checked radio found.
    }
  });

  _node_export_file_assets_toggle(assets_state, assets_div, 'hide');

  $('input[@name=node_export_file_mode]').change(function () {
    assets_state = _node_export_file_get_state($(this).val())
    _node_export_file_assets_toggle(assets_state, assets_div, 'slide');
  });
});

/**
 * Get state based on input value.
 **/
function _node_export_file_get_state(value) {
  switch (value) {
    default:
    case 'remote':
      return false;

    case 'local':
      return true;
  }
}

/**
 * Basic show/hide controller.
 **/
function _node_export_file_assets_toggle(state, div, effect) {
  if (state) {
    if (effect == 'hide') {
      div.show();
    } else {
      div.slideDown();
    }
  } else {
    if (effect == 'hide') {
      div.hide();
    } else {
      div.slideUp();
    }
  }
}