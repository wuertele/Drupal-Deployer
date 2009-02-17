<?php
define('NODEREFERENCE_VIEWS_SELECT_STYLE_PLUGIN_NAME', 'nvs_teaser_list');

/**
 * Implementation of hook_views_style_plugins()
 */
function nodereference_views_select_views_style_plugins() {
	$items = array();
	
	$items[NODEREFERENCE_VIEWS_SELECT_STYLE_PLUGIN_NAME] = array
	(
    	'name' => t('Sortable teaser list for Node Reference'),
        'theme' => 'nodereference_views_select_teaser_list',
        'needs_table_header' => true,
    	'needs_fields' => true
	);

	return $items;
}

/**
 * Helper function. Prepares sorting structure
 * 
 * @param object $view
 * @return array
 */
function _nodereference_views_select_views_sort($view)
{
	$sorts = array();
	
	$ts = tablesort_get_order($view->table_header);
	$ts['sort'] = tablesort_get_sort($view->table_header);
	$ts['query_string'] = drupal_query_string_encode($_GET, array_merge(array('q', 'sort', 'order'), array_keys($_COOKIE)));

	if (!empty($view->table_header))
	{
		foreach($view->table_header as $cell) 
		{
			$th = _nodereference_views_select_views_list_header($cell, $view->table_header, $ts, 'asc');
			if (!empty($th))
			{
				$sorts[] = $th;
			}
			
			$th = _nodereference_views_select_views_list_header($cell, $view->table_header, $ts, 'desc');
			if (!empty($th))
			{
				$sorts[] = $th;
			}
		}
	}
	
	return $sorts;
}

/**
 * Gather inforamtion for sortable field header
 * 
 * @param array $cell 
 * 	The cell to format formated as cell from $view->table_header built by view engine
 * @param array $header
 * 	An array of column headers in the format described in theme_table()
 * @param array $ts
 * 	The current table sort context as returned from tablesort_init()
 * @param string $direction
 *  Sort direction (asc/desc)
 * @return array
 */
function _nodereference_views_select_views_list_header($cell, $header, $ts, $direction)
{	
	$url = '';
	if (is_array($cell) && isset($cell['field']))
	{
		$url = url($_GET['q']);
		$sort['sort'] = $direction;
		$sort['active'] = (int)($cell['data'] == $ts['name'] && $ts['sort'] == $direction);

		if (!empty($ts['query_string'])) {
			$sort['query_string'] = '&'. $ts['query_string'];
		}
		$url .= '?sort='. $direction .'&order='. urlencode($cell['data']);
		
		if ($ts['query_string'])
		{
			$url .= '&' . $ts['query_string'];
		}

		$sort['url'] = $url;
		$sort['name'] = $cell['data'] . ' (' . t($direction) . ')';
	}
	else
	{
		$sort = null;
	}

	return $sort;
}


/**
 * Loads the nodes for this view
 * 
 * @param $view
 * @param $nodes
 * @return array of nodes
 */
function _nodereference_views_select_views_items($view, $nodes)
{
	foreach ($nodes as &$node)
	{
		$node = node_load($node->nid);	
	}
	
	return $nodes;
}
