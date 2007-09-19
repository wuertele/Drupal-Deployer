// $Id$

if (Drupal.jsEnabled) {
	$(document).ready(function(){
		var $e;
		var id;
		var settings = new Array;
		
		for (id in Drupal.settings.panels_tabs['filling_tabs']) {
			settings['filling_tabs'] = Drupal.settings.panels_tabs['filling_tabs'][id];

			$e = $("div."+ id +" ul.anchors li");
			$e.each(function() {
				if (settings['filling_tabs'] == 1) {
		   		$(this).css('width', (99 / $e.length) +'%').css('overflow', 'hidden');
				}
				else {
					$(this).css('min-width', (99 / $e.length) +'%');
				}
		 	});
		}
	});
}
