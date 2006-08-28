Overview
--------
This module allows Drupal to replace textarea fields with FCKeditor or have 
the option to use FCKeditor in its own popup window thus allowing a user the 
choice of when to load and use it. This HTML text editor brings to the web 
many of the powerful functionalities of known desktop editors like Word. 
It's really lightweight and doesn't require any kind of installation on the 
client computer.

NOTE: FCKeditor for Drupal relays on an external library called FCKeditor. 
For further information please refer to:

-------------------------------------------------------------------------------

FCKeditor - The text editor for internet
Copyright (C) 2003-2006 Frederico Caldeira Knabben
 
Licensed under the terms of the GNU Lesser General Public License:
		http://www.opensource.org/licenses/lgpl-license.php

For further information visit:
		http://www.fckeditor.net/

-------------------------------------------------------------------------------

Requirements
------------
- Drupal 4.7
- PHP 4.3.0 or greater
- FCKEditor 2.x (http://www.fckeditor.net/)

Installation
------------
  1. Copy the module fckeditor folder to modules/.
  2. Download FCKeditor 2.x from http://www.fckeditor.net and copy the
     distribution files (the contents of the "fckeditor" directory from the
     FCKeditor distribution file) to modules/fckeditor/fckeditor.

Configuration
-------------
  1. Modify the fckeditor.config.js file to custom your needs (optional).
  2. Enable the module as usual from Drupal's admin pages.
  3. Under settings, configure the fckeditor settings.
  4. Grant permissions to the groups you want use fckeditor.

Server Side Integration
-----------------------
NOTE: Server Side Integration is not yet tested for drupal 4.7, the configuration 
instructions are for the previous version, and will probably not work
-----------------------
The editor gives the end user the flexibility to create a custom file browser 
that can be integrated on it. The file browser allows users to view the
content of a specific directory on the server and add new content to that 
directory (create foldes and upload files). To enable the File Browser 
(server side integration pack) follow these steps

  1. Edit the fckconfig.js file and set the FCKConfig.LinkBrowserURL and
     FCKConfig.ImageBrowserURL properties to:
         
       FCKConfig.LinkBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=[context_path]/modules/fckeditor/ssip/connector.php"
       FCKConfig.ImageBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=[context_path]/modules/fckeditor/ssip/connector.php"
       
     Note: If you are running Drupal under a directory replace '[context_path]'
     with yours directory's name (ie. /drupal ), otherwise simply remove it
    
       FCKConfig.LinkBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=/modules/fckeditor/ssip/connector.php"
       FCKConfig.ImageBrowserURL = FCKConfig.BasePath + "filemanager/browser/default/browser.html?Type=Image&Connector=/modules/fckeditor/ssip/connector.php"

  2. Modify the properties.inc file to fit your customs.
     Note: if you are running Drupal under a directory you need to set the
     'context_path' property to the ditectory's name

   
Credits
-------

 - FCKeditor for Drupal Core functionality originally written by:
     Frederico Caldeira Knabben
     Jorge Tite (LatPro Inc.)

 - FCKeditor for Drupal 4.7
     Ontwerpwerk (www.ontwerpwerk.nl)

 - FCKeditor - The text editor for internet
     Copyright (C) 2003-2006 Frederico Caldeira Knabben
     http://www.fckeditor.net/
