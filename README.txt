$Id$

README file for the clone module for Drupal 6.x.

The export module allows users to export a node and then import it to another 
website. The authorship is set to the current user, the menu and url aliases 
are reset.

Users with the "export node" permission can utilize this functionality. A new
tab will appear on node pages with the word "export".

This module makes reasonable checks on access permissions.  A user cannot export 
a node unless they can use the input format of that node, and unless they have
permission to create new nodes of that type based on a call to node_access().

Settings can be accessed at admin/settings/export.  On this page you can
set whether the publishing options are reset when making a export of a node.  
This is set for each node type individually.

This module seems to work with common node types, however YMMV, especially with
nodes that have any sort of image or file  attachments.   In all cases, but 
especially if you are using a complex (CCK) or custom node type, you should 
evaluate this module on a test site with a copy of your database before 
attempting to use it on a live site.

To install this module, copy the folder with the .info and .module files to the 
/sites/all/modules  OR /modules directory of your Drupal installation and enable
it at /admin/build/modules.  A new permission is available, but there are no 
changes to the database structure. On the settings page you may choose
node types to be omitted from the export mechanism - no users will be able
to try to export a node of an omitted type.

To install this module, copy the folder with all the files to the
/sites/all/modules  OR /sites/default/modules directory of your Drupal 
installation and enable it at /admin/build/modules.  Two new permissions are 
available, but there are no changes to the database structure.

Note: this module was originally built upon code from the node_clone module
maintained by Peter Wolanin (pwolanin@drupal) at http://drupal.org/project/node_clone
which was derived from code posted by Steve Ringwood (nevets@drupal) at http://drupal.org/node/73381#comment-137714

