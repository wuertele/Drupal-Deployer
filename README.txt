$Id$

Overview
--------
This module allows Drupal to replace textarea fields with the
FCKeditor.
This HTML text editor brings many of the powerful functions of known
desktop editors like Word to the web. It's relatively lightweight and
doesn't require any kind of installation on the client computer.

Compatibility
-------------
The integrated File Browser needs a bit of manual configuration,
more information about this in the included in this readme.txt file

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
  - Drupal 5.x
  - PHP 4.3.0 or greater
  - FCKeditor 2.3.x or greater (http://www.fckeditor.net/)

Installation
------------
  1. Copy the module fckeditor folder to modules/.
  2. Download FCKeditor from http://www.fckeditor.net and copy
     the distribution files (the contents of the "fckeditor" directory
     from the FCKeditor distribution file) to
     modules/fckeditor/fckeditor.
     
     A quick check to see if it is correct: the files 'COPY_HERE.txt',
     'fckconfig.js' and also the directory 'editor' must exist in the
     same directory,
     
     The correct directory structure is as follows:
     
      modules
      |--fckeditor
         |--fckeditor
            |--_samples
            |--editor
     
  3. See 'How to enable the File Browser' for additional instructions.

     Also read the security note in this readme

     Alternatives to the built-in file browser are the IMCE module and
     copying the image url into the url textfield.

Configuration
-------------
  1. Modify the fckeditor.config.js file to customize the toolbars to
     your needs (optional).

     You may also copy the needed configuration lines from the default
     FCKeditor configuration settings
     (modules/fckeditor/fckeditor/fckconfig.js), the lines in
     fckeditor.config.js will override most settings.

     It is not advised to change the default toolbars in the
     configuration settings (modules/fckeditor/fckeditor/fckconfig.js)
     because those are included in the FCKeditor package and might
     change without notice when you update the editor.
  
  2. Enable the module as usual from Drupal's admin pages.
  
  3. Grant permissions for use of FCKeditor in
     Administer > User Management > Access Control
     
  4. Under Administer > Settings > FCKeditor, create the fckeditor
     profiles. In each profile you can choose which textareas will be replaced by
     FCKeditor, select default toolbar and configure some more advanced
     settings.

  5. For the Rich Text Editing to work you also need to configure your
     filters for the users that may access Rich Text Editing. Either
     grant those users Full HTML access or use the following:                   
      
      <a> <p> <span> <div> <h1> <h2> <h3> <h4> <h5> <h6> <img> <map> <area> 
      <hr> <br> <br /> <ul> <ol> <li> <dl> <dt> <dd> <table> <tr> <td> <em> 
      <b> <u> <i> <strong> <font> <del> <ins> <sub> <sup> <quote> <blockquote> 
      <pre> <address> <code> <cite> <embed> <object> <strike> <caption>
      
  6. To have a better control over line breaks, you may disable "Line break converter" 
     in the chosen filter.
      
How to enable the File Browser 
---------------------------------------------------
The editor gives the end user the flexibility to create a custom file
browser that can be integrated on it. The included file browser allows
users to view the content of a specific directory on the server and
add new content to that directory (create folders and upload files).

To enable the file browser you need to edit the connector
configuration file in your fckeditor module directory, the file should
be in:
/modules/fckeditor/fckeditor/editor/filemanager/browser/default/connectors/php/config.php
and
/modules/fckeditor/fckeditor/editor/filemanager/upload/php/config.php
(FCKeditor 2.3.x - 2.4.x)

or

/modules/fckeditor/fckeditor/editor/filemanager/connectors/php/config.php
(FCKeditor 2.5+)

In this file you will need to enable the file browser:
$Config['Enabled'] = true ;

To use the drupal files directory you also need to adjust the following 
variables in the connector configuration:
  $Config['UserFilesPath']
and:
  $Config['UserFilesAbsolutePath']
    
Furthermore, you will need to create a "File", "Image", "Flash" and
"Media" subdirectory in your drupal files directory. These directories
must have the same privileges as the drupal files directory. In some
cases these directories must be world writable (chmod 0777).

Security
--------
Note that enabling file uploads is a security risk. That's why there
is a separate permission in Administer > Access Control for enabling
the file browser to certain groups.

Making File Browser more secure
-------------------------------
Please be aware that enabling file browser simply by setting
 $Config['Enabled'] = true ;
theoretically enables it for each user (with a little bit of hackery, 
it is possible to use file browser without appropiate Drupal permissions).
The more secure way of enabling file browser:

  1. instead of setting
       $Config['Enabled'] = true ;
     add the following code: 

       $drupal_path = "../../../../../../../../..";
       if(!file_exists($drupal_path . '/includes/bootstrap.inc')) {
         $drupal_path = "../..";
         do {
           $drupal_path .= "/..";
           $depth = substr_count($drupal_path, "..");
           false;
         }
         while(!($bootstrapFileFound = file_exists($drupal_path . '/includes/bootstrap.inc')) && $depth<10);
       }
       if (!isset($bootstrapFileFound) || $bootstrapFileFound) {
         $cwd = getcwd();
         chdir($drupal_path);
         require_once './includes/bootstrap.inc';
         drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
         $Config['Enabled'] = user_access('allow fckeditor file uploads');
         chdir($cwd);
       }

     straight after 
       $Config['Enabled'] = false ;
         
     This code enables file browser only to users that have "allow fckeditor file uploads" permission.
     note: if you don't set $drupal_path correctly, FCKeditor will find it out by itself.
  
  2. as of Drupal 5.2, additional step is required:
     locate file named settings.php inside your drupal directory
     (usually sites/default/settings.php)
     and set $cookie_domain variable to the appropiate domain.
     (remember to uncomment that line)
     
Plugins: Teaser break and Pagebreak
-----------------------------------
By default, FCKeditor module comes with two plugins that can handle teaser break (<!--break-->) and pagebreak (<!--pagebreak-->).
You can enable any (or even both) of them.
To do this, open fckeditor.config.js and uncomment these three lines:

	FCKConfig.PluginsPath = '../../plugins/' ;
	FCKConfig.Plugins.Add( 'drupalbreak' ) ;
	FCKConfig.Plugins.Add( 'drupalpagebreak' ) 
	
The second step is to add the buttons to toolbar (in the same file).
The button names are: DrupalBreak, DrupalPageBreak;

For example if you have a toolbar with an array of buttons defined as follows:
  ['Image','Flash','Table','Rule','SpecialChar']
simply add those two buttons at the end of array:
  ['Image','Flash','Table','Rule','SpecialChar', 'DrupalBreak', 'DrupalPageBreak']
(remember about single quotes).

Help & Contribution
-------------------
If you are looking for more information, have any troubles in configuration or if 
you found an issue, please visit the official project page:
  http://drupal.org/project/fckeditor

We would like to encourage you to join our team if you can help in any way.
If you can translate FCKeditor module, please use fckeditor.pot file as a template
(located in "po" directory) and send us the translated file so that we could attach it.
Any help is appreciated.
     
Credits
-------
 - FCKeditor for Drupal Core functionality originally written by:
     Frederico Caldeira Knabben
     Jorge Tite (LatPro Inc.)

 - FCKeditor for Drupal 5.x originally written by:
     Ontwerpwerk (www.ontwerpwerk.nl)
 
 - FCKeditor for Drupal 5.x is currently maintained by FCKeditor team.
     http://www.fckeditor.net/

 - FCKeditor - The text editor for internet
     Copyright (C) 2003-2006 Frederico Caldeira Knabben
     http://www.fckeditor.net/
