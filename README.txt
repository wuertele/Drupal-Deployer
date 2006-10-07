Overview
--------
This module allows Drupal to replace textarea fields with FCKeditor or have
the option to use FCKeditor in its own popup window thus allowing a user the
choice of when to load and use it.
This HTML text editor brings to the web many of the powerful functionalities
of known desktop editors like Word. It's relatively lightweight and doesn't
require any kind of installation on the client computer.

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
  2. Enable the imagebrowser (optional)
  3. Enable the module as usual from Drupal's admin pages.
  4. Under settings, configure the fckeditor settings.
  5. Grant permissions to the groups you want use fckeditor.

How to enable the imagebrowser (in FCKeditor 2.3.x)
---------------------------------------------------
The editor gives the end user the flexibility to create a custom file browser
that can be integrated on it. The file browser allows users to view the
content of a specific directory on the server and add new content to that
directory (create foldes and upload files).

To enable file browsing you need to edit the connector configuration file in
your fckeditor module directory, the file should be in:
/fckeditor/editor/filemanager/browser/default/connectors/php/config.php

In this file you will need to enable the file browser:
  $Config['Enabled'] = true ;

To use the drupal files directory you also need to comment out the following
line in the connector configuration:
  //$Config['UserFilesPath'] = '/UserFiles/' ;

If your file browser does not work you might need to create an "Image" and
a "Flash" subdirectory in your drupal files directory. These directories
probably must be world writable (chmod 0777).

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
