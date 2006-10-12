$Id$

Overview
--------
This module allows Drupal to replace textarea fields with the FCKeditor.
This HTML text editor brings many of the powerful functionalities of
known desktop editors like Word to the web. It's relatively lightweight
and doesn't require any kind of installation on the client computer.

Compatibility
-------------
At the moment the CVS version will work in Drupal 4.7 But the integrated
file management needs a bit of manual configuration, more information
about this is in the included readme.txt file


Required components
-------------------
To use FCKeditor in Drupal, you will need to download the FCKeditor
http://www.fckeditor.net/


More information and licence
----------------------------
FCKeditor - The text editor for internet
Copyright (C) 2003-2006 Frederico Caldeira Knabben

Licensed under the terms of the GNU Lesser General Public License:
    http://www.opensource.org/licenses/lgpl-license.php

For further information visit:
    http://www.fckeditor.net/

Requirements
------------
  - Drupal 4.7
  - PHP 4.3.0 or greater
  - FCKeditor 2.3.x (http://www.fckeditor.net/)

Installation
------------
  1. Copy the module fckeditor folder to modules/.
  2. Download FCKeditor 2.x from http://www.fckeditor.net and copy the
     distribution files (the contents of the "fckeditor" directory from
     the FCKeditor distribution file) to modules/fckeditor/fckeditor.
  3. See 'How to enable the imagebrowser' for additional instructions.

Configuration
-------------
  1. Modify the fckeditor.config.js file to custom your needs (optional).
  2. Enable the module as usual from Drupal's admin pages.
  3. Under settings, configure the fckeditor settings.
  4. Grant permissions for use of FCKeditor in Administer > Access Control

Security
--------
Note that enabling file uploads is a security risk. That's why there is a
separate permission in Administer > Access Control for enabling the file
browser to certain groups.

How to enable the imagebrowser (in FCKeditor 2.3.x)
---------------------------------------------------
The editor gives the end user the flexibility to create a custom file browser
that can be integrated on it. The included file browser allows users to view
the content of a specific directory on the server and add new content to
that directory (create folders and upload files).

To enable file browsing you need to edit the connector configuration file in
your fckeditor module directory, the file should be in:
/fckeditor/editor/filemanager/browser/default/connectors/php/config.php

In this file you will need to enable the file browser:
  $Config['Enabled'] = true ;

To use the drupal files directory you also need to comment out the following
line in the connector configuration:
  //$Config['UserFilesPath'] = '/UserFiles/' ;

If your file browser does not work you might need to create an "Image" and
a "Flash" subdirectory in your drupal files directory. These directories must
have the same privileges as the drupal files directory. In some cases these
directories must be world writable (chmod 0777).

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
