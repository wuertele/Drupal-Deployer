// $Id$

var NVS = 
{
	modalId: 'nodereference_views_select_modal'
} 

/**
 * Removes an item from the teaser list
 */
NVS.removeItem = function(elem, fieldName)
{
	// find the node this remove link is rendered for
	var nid = $(elem).parent().find('div.node').attr('id').substring(5);
	
	// remove the nid from the hidden input
	var inputId = '#' + fieldName + '-nids';
	var nids = $(inputId).val().split(',');
	var newNids = [];
	
	var count = 0;
	for (i in nids)
	{
		if (nids[i] != nid)
		{
			newNids[count++] = nids[i];
		}
	}

	$(inputId).val(newNids.join(','));
	
	// remove the teaser from the screen
	$('#node-' + nid).parents('div.nodereference-teaser-current-list-item').fadeOut();
	
	return false;	
}

/**
 * Displays the modal panel
 */
NVS.showModal = function(fieldName)
{	
	
	var field = Drupal.settings.NVS.fields[fieldName];
	
	var url = 	Drupal.settings.jstools.basePath + 
				Drupal.settings.NVS.modalPath + '/' +
				field.viewName;
	
	var params = [];
	
	// add default filters to the GET request. This is because Views will
	// not render the filters properly if their valeus are not present in
	// the query string
	if (field.defaultFilters)
	{
		var filters = field.defaultFilters.split(',');
		
		for (var i = 0; i < filters.length; i++)
		{
			params[i] = 'filter' + i + '=' + filters[i];
		}
	}
	
	url += '?' + params.join('&');
				
	var modal = $('<div id="' + this.modalId + '" class="nodereference_views_select_modal ' + fieldName + '_modal"></div>');
		
	NVS.loadModal(modal, field, url);

	$('body').append('<div id="nodereference_views_select_overlay"></div>');
	$('body').append(modal);
	
	$('select').addClass('select_z_index_fix');
	
	return false;
}

/**
 * Removes the modal and the overlay
 */
NVS.closeModal = function(modal, field)
{
	NVS.modalChooseNodes(field);
	modal.remove();
	$('#nodereference_views_select_overlay').remove();
	$('select').removeClass('select_z_index_fix');
}

/**
 * Loads the modal content from the specified url
 */
NVS.loadModal = function(modal, field, url)
{
	modal.load(url, field, function()
	{
		NVS.modalOnLoad(modal, field);
	});			
}

/**
 * Called after modal content has been loaded and ajaxifies the view inside
 */
NVS.modalOnLoad = function(modal, field)
{
	// close button behavior
	modal.find('a.nodereference_views_select_modal_close').click(function()
	{
		NVS.closeModal(modal, field);		
		return false;
	});	
	
	// ajaxify pager
	modal.find('div.pager a').click(function()
	{
		NVS.loadModal(modal, field, $(this).attr('href'));
		
		return false;
	});
	
	// sort action
	modal.find('select.nodereference_views_select-sort-select').change(function()
	{
		NVS.loadModal(modal, field, $(this).val());	
	});
	
	// filter action
	modal.find('#views-filters select').change(function()
	{
		var url = 	Drupal.settings.jstools.basePath + 
					Drupal.settings.NVS.modalPath + '/' +
					field.viewName + '?' + $(this).parents('form').serialize();
					
		NVS.loadModal(modal, field, url);	
	});
	
	// turn off the appropriate selectors in the modal according to the
	// nids selected in the widget
	if (!field.isMultiReference || field.isMultiReference == 0)
	{
		var nids = $('#' + field.fieldName + '-nids').val().split(',');
		
		modal.find('a.add_item_link').each(function()
		{
			for (i in nids)
			{
				if (nids[i] == $(this).attr('nid'))
				{
					$(this).addClass('disabled');
				}
			}		
		});
	}
	
	// set selector action
	modal.find('a.add_item_link').click(function()
	{
		if (!$(this).hasClass('disabled'))
		{
			NVS.modalItemChosen(modal, field, this);
		}
		
		return false;
	});
}

/**
 * Called when choosing an item
 */
NVS.modalItemChosen = function(modal, field, element)
{
	// find the node this checkbox is rendered for
	var nid = $(element).attr('nid');
	var inputId = '#' + field.fieldName + '-nids';
	
	if (field.isMultiSelect == 1) // true
	{
		// add / remove the nid from the hidden input
		var nids = $(inputId).val().split(',');
		
		nids.push(nid);
			
		$(inputId).val(nids.join(','));
		
		// remove any old notification
		modal.find('div.nodereference-modal-item .messages').hide();
		
		// add notification
		$(element).parents('div.nodereference-modal-item').find('.messages')
			.hide().html(field.addedMessage).fadeIn();
		
		// disable link if field does not allow multiple references
		if (!field.isMultiReference)
		{
			$(element).addClass('disabled');
		}
	}
	else
	{
		// replace the hidden input content
		$(inputId).val(nid);
		
		// close modal
		NVS.closeModal(modal, field);		
	}
}

/**
 * Refreshes the teaser list in the widget
 */
NVS.modalChooseNodes = function(field)
{
	field.nids = $('#' + field.fieldName + '-nids').val();

	
	NVS.loadTeaserList(field, []);
}

/**
 * (re)loads the teaser list for the specified field 
 */
NVS.loadTeaserList = function(field, queryParams)
{	
	var url = 	Drupal.settings.jstools.basePath + 
				Drupal.settings.NVS.widgetPath + 
				'?' + queryParams.join('&');
	
	$('#' + field.widgetId + ' div.nodereference_views_select_teaser_wrapper').load(url, field, function()
	{
		NVS.teaserListLoaded(field.fieldName);
	});	
}

/**
 * Called after the teaser list has been loaded, set action for removal / sorting 
 */
NVS.teaserListLoaded = function(fieldName)
{
	var field = Drupal.settings.NVS.fields[fieldName];
	
	// add remove behavior
	$('#' + field.widgetId + ' a.remove-button').click(function()
	{
		return NVS.removeItem(this, fieldName);
	});
	
	// add sorting behavior
	if (field.isSortable)
	{
		var teaserList = $('#' + field.widgetId + ' ul.nodereference_views_select_teaser_list');
	
		teaserList.sortable
		({
			stop: function(event, ui)
			{
				// modify the hidden input order 
	    		var nids = [];
	    		
	    		$('#' + field.widgetId + ' div.nodereference-teaser-current-list-item').each(function(index, elem)
				{
	    			nids[index] = $(elem).attr('nid');
				});	   
	    		
	    		$('#' + field.fieldName + '-nids').val(nids.join(','));
			}
		});
	}
	
	// ajaxify pager
	$('#' + field.widgetId + ' div.pager a').click(function()
	{		
		var url = $(this).attr('href');
		
		var params = url.substring(url.indexOf('?') + 1, url.length).split('&');		
		
		NVS.loadTeaserList(field, params);
		
		return false;
	});	
}
