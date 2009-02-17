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
	
	return false;
}

/**
 * Removes the modal and the overlay
 */
NVS.closeModal = function(modal)
{
	modal.remove();
	$('#nodereference_views_select_overlay').remove();
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
		NVS.closeModal(modal);		
		return false;
	});	
	
	// choose button behavior
	modal.find('a.nodereference_views_select_modal_choose').click(function()
	{
		NVS.modalChooseNodes(field);
		NVS.closeModal(modal);
		return false;
	});
	
	// divert pager to ajax calls
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
	
	// turn on the appropriate selectors in the modal according to the
	// nids selected in the widget
	var nids = $('#' + field.fieldName + '-nids').val().split(',');
	
	$('.nodereference-teaser-checkbox input').each(function()
	{
		for (i in nids)
		{
			if (nids[i] == $(this).attr('nid'))
			{
				$(this).attr('checked', 'checked');
			}
		}		
	});
	
	// set selector change action
	$('.nodereference-teaser-checkbox input').change(function()
	{
		NVS.modalSelectorChange(modal, field, this);
	});
}

/**
 * Called when changing a checkbox / radio button
 */
NVS.modalSelectorChange = function(modal, field, element)
{
	// find the node this checkbox is rendered for
	var nid = $(element).attr('nid');
	var inputId = '#' + field.fieldName + '-nids';
	
	if (field.isMultiSelect == 1) // true
	{
		// add / remove the nid from the hidden input
		var nids = $(inputId).val().split(',');
		
		// add
		if ($(element).attr('checked'))
		{
			nids.push(nid);
			
			$(inputId).val(nids.join(','));
		}
		else // remove
		{
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
		}	
	}
	else
	{
		// replace the hidden input content
		$(inputId).val(nid);
	}
}

/**
 * Refreshes the teaser list in the widget
 */
NVS.modalChooseNodes = function(field)
{
	field.nids = $('#' + field.fieldName + '-nids').val();
	
	var url = 	Drupal.settings.jstools.basePath + 
				Drupal.settings.NVS.widgetPath;
	
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
	
	$('#' + field.widgetId + ' a.remove-button').click(function()
	{
		return NVS.removeItem(this, fieldName);
	});
	
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
}
