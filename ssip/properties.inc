<?php
/**
 * FCKeditor for Drupal
 * 
 * Server Side Intregation Pack
 * 
 * This properties are used by connector to enable server side integration.
 * Edit this properties to custom your needs
 * 
 * 
 * @version 1.0
 * @author  LatPro Inc (George)
 *    
 */

function _fck_connector_properties() {
	$properties = array();
	
	// The file system path separator (\) windows, (/) unix
	$properties['path_separator'] = '\\';
	
	// The Context Path property represents a web application, which is run 
	// within a particular virtual host. If you specify a context path of an 
	// empty string (''), you are defining the default web application for 
	// this Host.
	// $properties['context_path'] = '';
	$properties['context_path'] = '/drupal';
	
	// The default resource types list
	$properties['resource_list'] = array('File', 'Image', 'Flash', 'Media');
	
	// resource type paths (absolute path under the context_path)
	// These directories must exists in the current file system and have
	// read and write permissions
	$properties['File']   = '/';
	$properties['Image']  = '/images';
	$properties['Flash']  = '/flash';
	$properties['Flash']  = '/media';
	
	return $properties;
}
?>