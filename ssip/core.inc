<?php
/**
 * FCKeditor for Drupal
 * 
 * Core functions for Server Side Intregation Pack
 * 
 * 
 * @version 1.0
 * @author  LatPro Inc (George)
 * 
 */

function _fck_connector_execute() {
	
	$properties = _fck_connector_properties();

	// Get the main request informaiton.
	//$server_path    = $_GET['ServerPath'];
	$command        = $_GET['Command'] ;
	$resource_type  = $_GET['Type'] ;
	$current_folder = $_GET['CurrentFolder'] ;
	
	if ( empty($command) || empty($resource_type) || empty($current_folder) ) {
		return;
	}

	// test if is a valid resource type
	if ( !in_array($resource_type, $properties['resource_list']) ) {
		return;
	}
	
	// Check the current folder syntax (must begin and start with a slash).
	if ( ! ereg('/$', $current_folder ) ) {
		$current_folder .= '/' ;
	}
	if ( strpos($current_folder, '/') !== 0 ) {
		$current_folder = '/' . $current_folder ;
	}

	// File Upload doesn't have to Return XML, so it must be intercepted before anything.
	if ( $command == 'FileUpload' ) {
		_fck_connector_file_upload($resource_type, $current_folder);
		return ;
	}

	// Prevent the browser from caching the result.
	// Date in the past
	header('Expires: Mon, 26 Jul 1997 05:00:00 GMT') ;
	// always modified
	header('Last-Modified: ' . gmdate('D, d M Y H:i:s') . ' GMT') ;
	// HTTP/1.1
	header('Cache-Control: no-store, no-cache, must-revalidate') ;
	header('Cache-Control: post-check=0, pre-check=0', false) ;
	// HTTP/1.0
	header('Pragma: no-cache') ;

	// Set the response format.
	header( 'Content-Type:text/xml; charset=utf-8' ) ;

	_fck_connector_xml_header($command, $resource_type, $current_folder );

	// Execute the required command.
	switch ( $command )
	{
		case 'GetFolders' :
			_fck_connector_get_folders($resource_type, $current_folder);
			break ;
		case 'GetFoldersAndFiles' :
			_fck_connector_get_folders_and_files($resource_type, $current_folder);
			break ;
		case 'CreateFolder' :
			_fck_connector_create_folder($resource_type, $current_folder);
			break ;
	}

	_fck_connector_xml_footer() ;
	exit;
}

/**
 * Print a xml node(s) representing a folder in the file system
 */
function _fck_connector_get_folders($resuorce_type, $current_folder) {
	
	// gets the ral path for the current folder
	$url = _fck_connector_get_url_from_path($resuorce_type, $current_folder);
	$path = _fck_connector_get_real_path($url);

	// Open the "Folders" node.
	print "<Folders>" ;

	$folder = opendir( $path ) ;
	while ( $file = readdir( $folder ) ) {
		if ( $file != '.' && $file != '..' && is_dir($path . $file) )
			print '<Folder name="' . _fck_connector_to_xml_attribute($file) . '" />';
	}
	closedir( $folder ) ;
	
	// Close the "Folders" node.
	print "</Folders>" ;
}

/**
 * Print a xml node(s) representing a file or directory
 */
function _fck_connector_get_folders_and_files($resuorce_type, $current_folder) {

	// gets the ral path for the current folder
	$url = _fck_connector_get_url_from_path($resuorce_type, $current_folder);
	$path = _fck_connector_get_real_path($url);

	// Initialize the output buffers for "Folders" and "Files".
	$sFolders	= '<Folders>' ;
	$sFiles		= '<Files>' ;

	$folder = opendir( $path ) ;
	while ( $file = readdir( $folder ) ) {
		if ( $file != '.' && $file != '..' ) {
			if ( is_dir( $path . $file ) ) {
				$sFolders .= '<Folder name="' . _fck_connector_to_xml_attribute($file) . '" />';
			} else {
				$size = filesize( $path . $file );
				if ( $size > 0 ) {
					$size = round( $size / 1024 );
					if ( $size < 1 ) $size = 1;
				}

				$sFiles	.= '<File name="' . _fck_connector_to_xml_attribute($file) . '" size="' . $size . '" />' ;
			}
		}
	}

	print $sFolders ;
	// Close the "Folders" node.
	print '</Folders>' ;
	
	print $sFiles ;
	// Close the "Files" node.
	print '</Files>' ;
}

/**
 * tries to create a new folder in the current file system
 */
function _fck_connector_create_folder($resuorce_type, $current_folder) {
	$error_number	= '0' ;
	$error_msg		= '' ;

	if ( isset( $_GET['NewFolderName'] ) ) {
		$folder_name = $_GET['NewFolderName'] ;

		$url = _fck_connector_get_url_from_path($resuorce_type, $current_folder);
		$path = _fck_connector_get_real_path($url);
		
		if ( is_writable( $path ) ) {
			$path .= $folder_name;

			$error_msg = _fck_connector_create_server_folder($path);

			switch ( $error_msg ) {
				case '' :
					$error_number = '0' ;
					break ;
				case 'Invalid argument' :
				case 'No such file or directory' :
					$error_number = '102' ;		// Path too long.
					break ;
				default :
					$error_number = '110' ;
					break ;
			}
		} else {
			$sErrorNumber = '103' ;
		}
	} else {
		$error_number = '102' ;
	}

	// Create the "Error" node.
	print '<Error number="' . $error_number . '" originalDescription="' . _fck_connector_to_xml_attribute($error_msg) . '" />' ;
}

/**
 * Tries to upload a file to the current file system
 */
function _fck_connector_file_upload($resuorce_type, $current_folder) {
	$sErrorNumber = '0' ;
	$sFileName = '' ;

	if ( isset( $_FILES['NewFile'] ) && !is_null( $_FILES['NewFile']['tmp_name'] ) ) {
		$oFile = $_FILES['NewFile'] ;

		// Map the virtual path to the local server path.
		$url = _fck_connector_get_url_from_path($resuorce_type, $current_folder);
		$sServerDir = _fck_connector_get_real_path($url);

		// Get the uploaded file name.
		$sFileName = $oFile['name'] ;
		$sOriginalFileName = $sFileName ;

		$iCounter = 0 ;

		while ( true ) {
			$sFilePath = $sServerDir . $sFileName ;

			if ( is_file( $sFilePath ) ) {
				$iCounter++ ;
				$oPathInfo = pathinfo( $sFilePath ) ;
				$sFileName = RemoveExtension( $sOriginalFileName ) . '(' . $iCounter . ').' . $oPathInfo['extension'] ;
				$sErrorNumber = '201' ;
			} else {
				move_uploaded_file( $oFile['tmp_name'], $sFilePath ) ;
				break ;
			}
		}
	} else {
		$sErrorNumber = '202' ;
	}

	print '<script type="text/javascript">' ;
	print 'window.parent.frames["frmUpload"].OnUploadCompleted(' . $sErrorNumber . ',"' . str_replace( '"', '\\"', $sFileName ) . '") ;' ;
	print '</script>' ;

	exit ;
}

/**
 * Creates a new folder
 */
function _fck_connector_create_server_folder($path) {
	if ( !file_exists( $path ) )	{
		// Turn off all error reporting.
		error_reporting( 0 ) ;
		// Enable error tracking to catch the error.
		ini_set( 'track_errors', '1' ) ;

		mkdir( $path, 0777 );

		$error_msg = $php_errormsg ;

		// Restore the configurations.
		ini_restore('track_errors');
		ini_restore('error_reporting');

		return $error_msg ;
	}
	return '';
}



/**
 * Write the XML header for connector response
 */
function _fck_connector_xml_header($command, $resource_type, $current_folder) {
	// Create the XML document header.
	print '<?xml version="1.0" encoding="utf-8" ?>' ;

	// Create the main "Connector" node.
	print '<Connector command="' . $command . '" resourceType="' . $resource_type . '">' ;
	
	// Add the current folder node.
	$path = _fck_connector_to_xml_attribute($current_folder);
	$url = _fck_connector_to_xml_attribute( _fck_connector_get_url_from_path($resource_type, $current_folder ) );
	print '<CurrentFolder path="' . $path . '" url="' . $url . '" />' ;
}

/**
 * Writes the XML footer  fro connector response
 */
function _fck_connector_xml_footer() {
	print '</Connector>' ;
}

/**
 * Formats a value as xml attribute
 */
function _fck_connector_to_xml_attribute( $value ) {
	return utf8_encode( htmlspecialchars( $value ) ) ;
}

/**
 * Gets the url fro a specified folder
 */
function _fck_connector_get_url_from_path($resource_type, $current_folder) {
	$properties = _fck_connector_properties();
	$path = $properties['context_path'] . $properties[$resource_type];
	return _fck_connector_remove_from_end( $path, '/' ) . $current_folder;
}

/**
 * Gets the real path in the current file system for the specified resource
 */
function _fck_connector_get_real_path($path) {
	$properties = _fck_connector_properties();
	$separator = $properties['path_separator'];
	if ($separator == '\\') {
		$separator .= '\\';
	}
	
	$sp = $_SERVER["PATH_TRANSLATED"];
	$sp = substr($sp, 0, strpos($sp, 'modules'));
	$sp = _fck_connector_remove_from_end($sp, $separator);

	// rewrite path to replace context path
	if ( !empty($properties['context_path']) && strstr($path, $properties['context_path'])) {
		$path = substr($path, strlen($properties['context_path']) + strpos($path, $properties['context_path']));
	}
	if ( empty($path) ) {
		$path = '/';
	}
	if ( !ereg('/$', $path) ) {
		$path .= '/';
	}
	$sp .= str_replace('/', $separator, $path);
	return $sp;
}

/**
 * Removes from the end of $source the specified $char
 */
function _fck_connector_remove_from_end($source, $char) {
	$pattern = '|' . $char . '+$|' ;
	return preg_replace( $pattern, '', $source) ;
}

?>