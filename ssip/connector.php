<?php
/**
 * FCKeditor for Drupal
 * 
 * Server Side Intregation Pack
 * 
 * This pack implements a Connector between Drupal and FCKeditor File browser.
 * The "Connector" is the responsible for handling requests made by the
 * File Browser. The following tasks are done by the Connector:
 *   - Receive the File Manager requests.
 *   - Execute operations in the File System, like folder and files creations 
 *     and listings.
 *   - Build the XML response in the right format and syntax.
 *   - Receive and handle file uploads from the File Browser.
 *   
 * All requests are simply made by the File Browser using the normal HTTP 
 * channel. The request info is always passed by QueryString in the URL that 
 * reflects the following format:
 * 
 * connector.php?Command=CommandName&Type=ResourceType&CurrentFolder=FolderPath&ServerPath=ServerPath
 * 
 * 
 * 
 * Configuration
 * -------------
 * 1. Edit the fckconfig.js file and set the FCKConfig.LinkBrowserURL and
 *    FCKConfig.ImageBrowserURL properties to:
 *    
 *       FCKConfig.LinkBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=[context_path]/modules/fckeditor/ssip/connector.php"
 *       FCKConfig.ImageBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=[context_path]/modules/fckeditor/ssip/connector.php"
 *    
 *    Note: If you are running Drupal under a directory replace [context_path]
 *    with yours directory's name (ie. /drupal ), otherwise simply remove it
 *    
 *       FCKConfig.LinkBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=/modules/fckeditor/ssip/connector.php"
 *       FCKConfig.ImageBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=/modules/fckeditor/ssip/connector.php"

 * 2. Modify the properties.inc file to fit your customs.
 *    Note: if you are running Drupal under a directory you need to set the
 *    'context_path' property to the ditectory's name
 *    
 *    
 *    
 *    
 * @version 1.0
 * @author  LatPro Inc (George)
 *    
 */

include_once 'core.inc';
include_once 'properties.inc';

_fck_connector_execute();

?>