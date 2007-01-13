/* $Id$ */

Drupal Administration Menu is re-building and automagically slicing the whole
menu tree below /admin including all invisible local tasks into the Drupal
Administration Menu. So administrators need less time to access pages which
are only visible after one or two clicks normally.

-- INSTALLATION --

* Copy admin_menu module to your modules directory and enable it on the admin
modules page.

* If "Admin Menu Block" is not automatically activated after module
installation, make sure you visit the block configuration page
(admin/build/block/) for each theme you want to have admin_menu activated for.

* If you are using a separate admin theme, make sure the block is activated
for this theme, too.


-- AUTHORS --

Daniel F. Kudwien, dev@unleashedmind.com
Stefan M. Kudwien, dev@unleashedmind.com

