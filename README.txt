Overview
--------
This module allows Drupal to replace textarea fields with FCKEditor. This HTML 
text editor brings to the web many of the powerful functionalities of known 
desktop editors like Word. It's really  lightweight and doesn't require any 
kind of installation on the client computer.



FCKeditor - The text editor for internet
Copyright (C) 2003-2004 Frederico Caldeira Knabben
 
Licensed under the terms of the GNU Lesser General Public License:
		http://www.opensource.org/licenses/lgpl-license.php

For further information visit:
		http://www.fckeditor.net/



Requirements
------------
- Drupal 4.5.x
- PHP 4.3.0 or greater
- FCKEditor 2.0 (http://www.fckeditor.net/)



Features
--------
 - Administrative features:
   o Can customize block title 
   o Can define the type of directory (public/private) 

 - User features:
   o Can add/edit/delete own directory if have 'manage contacts' permission



Installation
------------
  1. Copy fckeditor folder to modules/.
  2. Download FCKeditor 2.0 from http://www.fckeditor.net and copy the
     distribution files to {DRUPAL_HOME}/modules/fckeditor/lib.



Configuration
-------------
  1. Modify the fckconfig.js file to custom your needs (not required)
  2. Enable the module as usual from Drupal's admin pages.
  3. Under settings, configure the fckeditor settings.
  4. Grant permissions to the groups you want use fckeditor.


   
Credits
-------
LatPor Inc.

 - Core functionality originally written by:
    Jorge Tite
   