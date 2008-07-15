<?php
// $Id$
/**
 * FCKeditor - The text editor for Internet - http://www.fckeditor.net
 * Copyright (C) 2003-2008 Frederico Caldeira Knabben
 *
 * == BEGIN LICENSE ==
 *
 * Licensed under the terms of any of the following licenses at your
 * choice:
 *
 *  - GNU General Public License Version 2 or later (the "GPL")
 *    http://www.gnu.org/licenses/gpl.html
 *
 *  - GNU Lesser General Public License Version 2.1 or later (the "LGPL")
 *    http://www.gnu.org/licenses/lgpl.html
 *
 *  - Mozilla Public License Version 1.1 or later (the "MPL")
 *    http://www.mozilla.org/MPL/MPL-1.1.html
 *
 * == END LICENSE ==
 *
 * @file
 * FCKeditor Module for Drupal 6.x
 *
 * This module allows Drupal to replace textarea fields with FCKeditor.
 *
 * This HTML text editor brings to the web many of the powerful functionalities
 * of known desktop editors like Word. It's really  lightweight and doesn't
 * require any kind of installation on the client computer.
 */

function fckeditor_help_delegate($path, $arg) {
  switch ($path) {
    case 'admin/settings/help#description':
      $output = t("Enables the usage of FCKeditor (WYSIWYG editor) instead of plain text fields.");
      break;
    case 'admin/settings/fckeditor':
      if (!empty($arg[3]) && in_array($arg[3], array("addg", "editg"))) {
        $output = t("<p>The Global Profile allows you to define settings that are common for all profiles. Values defined in other profiles will be appended to the global configuration. This way you can avoid repeating some of the settings that are usually the same in each profile.</p>");
        break;
      }
      else if (!empty($arg[3]) && in_array($arg[3], array("add", "edit"))) {
        $output = t("<p>Note: FCKeditor is highly configurable. The most commonly used features are listed below. If you want to take a look at all available settings, open <code>!fckconfig</code> and then customize <code>!fckeditor_config</code> to your needs. This is also the only way to define new toolbar sets. It is advised to not edit <code>fckconfig.js</code> because you may overwrite it accidentally when you update the editor.</p>", array('!fckconfig' => drupal_get_path('module', 'fckeditor') ."/fckeditor/fckconfig.js", '!fckeditor_config' => drupal_get_path('module', 'fckeditor') ."/fckeditor.config.js"));
        break;
      }
      else if (!empty($arg[3]) && in_array($arg[3], array("delete", "deleteg"))) {
        break;
      }
      $output = t("<p>The FCKeditor module allows Drupal to replace textarea fields with a rich text or <acronym title=\"What You See Is What You Get\">WYSIWYG</acronym> editor. This editor brings many of the powerful functionalities of known desktop editors like Word to the web. It's relatively lightweight and doesn't require any kind of installation on the client computer.</p><p>More information about the editor is located at the !fckeditorlink. A small user guide is located at !userguidelink.</p>",
      array(
      '!fckeditorlink' => l(t('FCKeditor homepage'), 'http://www.fckeditor.net'),
      '!userguidelink' => l(t('FCKeditor userguide'), 'http://docs.fckeditor.net/FCKeditor/Users_Guide'))
      );
      $output .= t('<p>Profiles can be defined based on user roles. A FCKeditor profile can define which pages receive this FCKeditor capability, what buttons or themes are enabled for the editor, how the editor is displayed, and a few other editor functions. It is possible also to define the Global Profile that will hold values that will be appended to all other profiles.</p><p>Lastly, only users with the <code>!access1</code> !permission will be able to use FCKeditor. </p>', array('!permission' => l(t('permission'), 'admin/user/permissions'), '!access1' => t('access fckeditor')));
      break;
    case 'admin/help#fckeditor':
      $output = t("<p>The FCKeditor module allows Drupal to replace textarea fields with a rich text or <acronym title=\"What You See Is What You Get\">WYSIWYG</acronym> editor. This editor brings many of the powerful functionalities of known desktop editors like Word to the web. It's relatively lightweight and doesn't require any kind of installation on the client computer.</p><p>More information is located at the !fckeditorlink. A small user guide is located at !userguidelink.</p>",
      array(
      '!fckeditorlink' => l(t('FCKeditor homepage'), 'http://www.fckeditor.net'),
      '!userguidelink' => l(t('FCKeditor userguide'), 'http://docs.fckeditor.net/FCKeditor/Users_Guide'))
      );
      $output .= t('<h3>Configuration</h3><ol><li>Go to the !fckeditorlink and download the latest version of FCKeditor. Then uncompress the contents of the "fckeditor" directory of the downloaded file to %fckeditordir.</li><li>Enable the module as usual from Drupal\'s admin pages.</li><li>Grant permissions for use of FCKeditor in <code>!path2</code><br/>Note: to enable the file browser, read also the <i>How to enable the file browser</i> section.</li><li>Under <code>!path1</code>, adjust the fckeditor profiles. In each profile you can choose which textareas will be replaced by FCKeditor, select default toolbar and configure some more advanced settings.</li><li>For the Rich Text Editing to work you also need to configure your !filterlink for the users that may access Rich Text Editing. Either grant those users Full HTML access or use the following: <br/><code>!filter</code>. </li><li>To have a better control over line breaks, you may disable <code>Line break converter</code> in the chosen filter (recommended).</li><li>Modify the fckeditor.config.js file to custom your needs (optional).<br />You may copy the needed configuration lines from the default FCKeditor configuration settings (!fckconfig), the lines in fckeditor.config.js will override most settings.</li></ol>',
      array(
      '!fckeditorlink' => l(t('FCKeditor homepage'), 'http://www.fckeditor.net/download'),
      '%fckeditordir' => base_path() . drupal_get_path('module', 'fckeditor') .'/fckeditor/',
      '!path1' => l(t('Administer > Site configuration > FCKeditor'), 'admin/settings/fckeditor'),
      '!path2' => l(t('Administer > User Management > Permissions'), 'admin/user/permissions'),
      '!filter' => htmlentities('<a> <p> <span> <div> <h1> <h2> <h3> <h4> <h5> <h6> <img> <map> <area>
      <hr> <br> <br /> <ul> <ol> <li> <dl> <dt> <dd> <table> <tr> <td> <em>
      <b> <u> <i> <strong> <font> <del> <ins> <sub> <sup> <quote> <blockquote>
      <pre> <address> <code> <cite> <embed> <object> <strike> <caption>'),
      '!fckconfig' => base_path() . drupal_get_path('module', 'fckeditor') .'/fckeditor/fckconfig.js',
      '!moduledir' => base_path() . drupal_get_path('module', 'fckeditor') .'/fckeditor',
      '!filterlink' => l(t('filters'), 'admin/settings/filters'))
      );
      $output .= '<a name="fieldinclexcl"></a>';
      $output .= t('<h3>Defining field inclusion/exclusions</h3><p>In order to specifically display FCKeditor on those text areas on which you need it, the FCKeditor module provides a powerfull method to define fields that should be enhanced with FCKeditor. The first choice you have to make is whether you want to display FCKeditor on all textareas and define a list of exceptions, or if you don\'t want to display FCKeditor unless it is specifically enabled. This choice is made in the @inclexclsetting setting.</p><p>Next, the list of exceptions has to be defined. This list follows a specific syntax: the path to a field is defined by appending the field name to the path, separated by a dot. The field <em>edit-log</em> on page <em>node/add/page</em> would thus be addressed as <em>node/add/page.edit-log</em>. You can use wildcards * and ? anywhere in this line. If you have a dot in your field name and it causes problems, you can escape it with a slash (\).</p><p>Some examples:<br/><em>node/add/page</em>: matches all fields on <em>node/add/page</em>.<br/><em>*.edit-log</em>: matches all fields called edit-log on any page.<br/><em>node/add/*</em>: matches all fields on pages such as <em>node/add/page</em>, <em>node/add/story</em>, etc.<br/><em>node/add/*.edit-log</em>: matches all edit-log fields on pages such as <em>node/add/page</em>, <em>node/add/story</em>, etc.<br/><em>node/add/*.edit-user-*</em>: matches fields starting with <em>edit-user-</em> on pages starting with <em>node/add/</em></p>', array('@inclexclsetting' => t('Use inclusion or exclusion mode')));
      $output .= t('<h3>Installation troubleshooting</h3><p>If your FCKeditor does not show you must check if all files are extracted correctly. The directory %fckeditordir should have the following files: <code>fckeditor.js, fckconfig.js, fckstyles.xml, fcktemplates.xml</code> and a directory named <code>editor</code>.</p>',
      array(
      '%fckeditordir' => base_path() . drupal_get_path('module', 'fckeditor') .'/fckeditor/')
      );
      $output .= t('The correct directory structure is as follows: <blockquote><pre>!structure</pre></blockquote>', array(
      '!structure' => "modules\n   fckeditor\n      <em>fckeditor.module</em>\n      fckeditor\n         _samples\n         editor\n         <em>COPY_HERE.txt</em>\n         <em>fckconfig.js</em>\n         ..."
      ));
      $output .= t("<h3>Plugins: Teaser break and Pagebreak</h3><p>By default, FCKeditor module comes with two plugins that can handle teaser break (&lt;!--break--&gt;) and pagebreak (&lt;!--pagebreak--&gt;). You can enable any (or even both) of them.<ol><li>Open <code>!fckeditor.config.js</code> and uncomment these three lines: <pre>!code</pre></li><li>The second step is to add buttons to the toolbar (in the same file). The button names are: <code>DrupalBreak, DrupalPageBreak</code>. For example if you have a toolbar with an array of buttons defined as follows: <pre>!buttons1</pre> simply add those two buttons at the end of array: <pre>!buttons2</pre> (remember about single quotes).</li></ol></p>",
      array(
      '!fckeditor.config.js' => base_path() . drupal_get_path('module', 'fckeditor') .'/fckeditor.config.js',
      '!code' => "
      FCKConfig.PluginsPath = '../../plugins/' ;
      FCKConfig.Plugins.Add( 'drupalbreak' ) ;
      FCKConfig.Plugins.Add( 'drupalpagebreak' ) ;
      ",
      '!buttons1' => "['Image','Flash','Table','Rule','SpecialChar']",
      '!buttons2' => "['Image','Flash','Table','Rule','SpecialChar', 'DrupalBreak', 'DrupalPageBreak']",
      ));
      $output .= t('<h3>Uploading images and files</h3><p>There are three ways of uploading files: By using the built-in file browser, by using a module like !imce or using the core upload module.</p>',
      array(
      '!imce' => l(t('IMCE'), 'http://drupal.org/project/imce')
      )
      );
      // the rest is untranslated for the moment
      $output .= t("<h3>How to enable the file browser</h3><p>The editor gives the end user the flexibility to create a custom file browser that can be integrated on it. The included file browser allows users to view the content of a specific directory on the server and add new content to that directory (create folders and upload files).</p><p><ol><li>To enable file browsing you need to edit the connector configuration file in your fckeditor module directory, the file should be in:<blockquote><code>!config3</code> <br/> (FCKeditor 2.5+)<br/><br/> or <br/><br/><code>!config1</code><br/> and <br/><code>!config2</code> <br/> (FCKeditor 2.3.x - 2.4.x)</blockquote></p><p>In this file(s) you will need to enable the file browser by adding one line that includes file with the special authentication function for Drupal (<code>filemanager.config.php</code>). Add this code: <blockquote><code>!code1</code><br/> (FCKeditor 2.5+)</blockquote> or <blockquote><code>!code2</code> <br/> (FCKeditor 2.3.x - 2.4.x)</blockquote> straight below this line: <blockquote><code>!code3</code></blockquote> The config.php file also holds some other important settings, please take a look at it and adjust it to your needs (optional).</p></li>",
      array('!config1' => base_path() . drupal_get_path('module', 'fckeditor') ."/fckeditor/editor/filemanager/browser/default/connectors/php/config.php",
      '!config2' => base_path() . drupal_get_path('module', 'fckeditor') ."/fckeditor/editor/filemanager/upload/php/config.php",
      '!config3' => base_path() . drupal_get_path('module', 'fckeditor') ."/fckeditor/editor/filemanager/connectors/php/config.php",
      '!filesdir' => file_directory_path(),
      '!code1' => 'require_once "../../../../../filemanager.config.php";', //2.5
      '!code2' => 'require_once "'. str_replace("\\", "\\\\", dirname(__FILE__) . DIRECTORY_SEPARATOR .'filemanager.config.php"'), //2.4
      '!code3' => "\$Config['UserFilesAbsolutePath'] = '' ;",
      )
      );
      $output .= t("<li>As of Drupal 5.2, additional step is required: locate file named <code>settings.php</code> inside your drupal directory (usually <code>sites/default/settings.php</code>) and set <strong><code>&#36;cookie_domain</code></strong> variable to the appropiate domain (remember to uncomment that line). If you not do this, FCKeditor will claim that file browser is disabled</li>");
      $output .= t('<li>Enabling file uploads is <strong>a security risk</strong>. That\'s why you have to grant a !link to enable the file browser to certain groups (assign the &quot;!allowupload&quot; permissions).</li>', array('!link' => l(t('separate permission'), 'admin/user/permissions'), "!allowupload" => t("allow fckeditor file uploads")));
      $output .= t('<li>Lastly, adjust the !fb for each !profile.</li></ol>', array('!fb' => t('File browser settings'), '!profile' => l(t('profile'), 'admin/settings/fckeditor')));
      $output .= t("<h3>Modules: Image Assist</h3><p>Image Assist can be integrated with FCKeditor. To do this, simply copy the <code>!iaf1</code> file to <code>!iaf2</code>.</p>", array("!iaf1" => drupal_get_path('module', 'fckeditor') ."/img_assist_fckeditor.js", "!iaf2" => drupal_get_path('module', 'img_assist') ."/img_assist_fckeditor.js"));

      break;
  }
  return !empty($output) ? $output : "";
}
?>