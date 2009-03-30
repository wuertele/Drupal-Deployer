<?php
// $Id$

/**
 * Renders a single node with or without the remove link
 * 
 * @ingroup themeable
 * @param $node
 * @param $show_remove_link
 * @return html
 */
function theme_nodereference_views_select_teaser($node, $show_remove_link)
{
	$teaser = node_view($node, true, false, false);
	$remove_link = $show_remove_link ? theme('nodereference_views_select_teaser_remove_link', $node->nid): '';
	
	$result = <<<EOD
<div class="nodereference-teaser-current-list-item" nid="{$node->nid}">
	{$teaser}
	{$remove_link}
</div>
EOD;

	return $result;
}

/**
 * @ingroup themeable
 * @param $nid
 * @return html
 */
function theme_nodereference_views_select_teaser_remove_link($nid)
{
	return sprintf('<a href="#" nid="%d" class="remove-button">%s</a>', $nid, t('Remove'));
}

/**
 * @ingroup themeable
 * @param $title
 * @param $attributes
 * @return html
 */
function theme_nodereference_views_select_widget_link($title, $attributes)
{	
	return sprintf('<a href="#" %s>%s</a>', drupal_attributes($attributes), $title);
}

/**
 * @ingroup themable
 * @param $view_content
 * @param $title
 * @param $has_pager
 * @return html
 */
function theme_nodereference_views_select_modal_content($view_content, $title, $has_pager)
{
	$scroll_pager_class = $has_pager ? 'nodereference_views_select_modal_pager' : 'nodereference_views_select_modal_scroll';
	
	$close_text = t('Close');
	
	$close_link = theme('nodereference_views_select_widget_link', $close_text, array('class' => 'nodereference_views_select_modal_close'));
	//$choose_link = theme('nodereference_views_select_widget_link', t('Choose'), array('class' => 'nodereference_views_select_modal_choose'));
	
	$output = <<<EOD
<div class="nodereference_views_select_modal_inner">
	<div class="nodereference_views_select_modal_header">
		<h4>$title</h4>
		<a href="#" class="nodereference_views_select_modal_close">$close_text</a>
	</div>
	<div class="$scroll_pager_class">
		$view_content
	</div>
	<div class="nodereference_views_select_modal_controls">
		$close_link
	</div>
</div>
EOD;
	
	return $output;
}

/**
 * theming function for the view
 *
 * @ingroup themable
 * @param object $view
 * @param array $nodes
 * @param string $type
 * @return html
 */
function theme_nodereference_views_select_teaser_list($view, $nodes, $type)
{
	$sorts = _nodereference_views_select_views_sort($view);
	$items = _nodereference_views_select_views_items($view, $nodes);

	$output = views_theme('nodereference_views_select_sort_header', $view->name, $sorts, tablesort_get_sort($view->table_header));
	$output .= views_theme('nodereference_views_select_modal_items', $view->name, $items, $view->field_data);

	return $output;
}

/**
 * Themes the sorting header of this view
 * 
 * @ingroup themable
 * @param $view_name
 * @param $items
 * @param $direction
 * @return html
 */
function theme_nodereference_views_select_sort_header($view_name, $items, $direction)
{
	$view_name_class = views_css_safe($view_name);

	$options = array();
	$value = false;
	for ($i = 0; $i < count($items); $i++)
	{
		$options[$items[$i]['url']] = $items[$i]['name'];
		if ($items[$i]['active'])
		{
			$value = $items[$i]['url'];
		}
	}

	$select = array
	(
		'#name' => "view_sort_links_{$view_name}_select",
		'#options' => $options,
		'#value' => $value,
		'#attributes' => array('class' => 'nodereference_views_select-sort-select'),
		'#title' => t('Sort by')
	);
	
	$select_html = !empty($items) ? theme('select', $select) : '';
	
	$output = <<<EOA
<div class="nodereference_views_select-sort" id ="nodereference_views_select-sort-$view_name_class">
	$select_html
</div>
EOA;

	return $output;
}

/**
 * @ingroup themable
 * @param $view_name
 * @param $nodes
 * @param $field_data array of field metadata
 * @return html
 */
function theme_nodereference_views_select_modal_items($view_name, $nodes, $field_data)
{
	$items = array();
	
	foreach ($nodes as $node)
	{
		$items[] = views_theme('nodereference_views_select_modal_item', $node, $field_data);
	}
	
	$list = theme('item_list', $items);
	
	$result = <<<EOD
<div class="nodereference_views_select_modal_items">
	$list
</div>	
EOD;

	return $result;
}

/**
 * Renders a single node with or without the remove link
 * 
 * @ingroup themeable
 * @param $node
 * @param $field_data array of field metadata
 * @return html
 */
function theme_nodereference_views_select_modal_item($node, $field_data)
{
	$teaser = node_view($node, true, false, false);
	
	$title = $field_data['is_multiple'] ? t('Add') : t('Choose');	
	
	$link = theme('nodereference_views_select_widget_link', $title, array('class' => 'add_item_link', 'nid' => $node->nid)); 
	
	$result = <<<EOD
<div class="nodereference-modal-item" id="nvs-{$node->nid}">
	{$teaser}
	{$link}
	<span class="messages">
	</span>
</div>
EOD;

	return $result;
}
