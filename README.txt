/* $Id$ */

Drupal Administration Menu is re-building and automagically slicing the whole
menu tree below /admin including all invisible local tasks into the Drupal
Administration Menu. So administrators need less time to access pages which
are only visible after one or two clicks normally.

-- INSTALLATION --

* Copy admin_menu module to your modules directory and enable it on the admin
  modules page.

* Drupal 4.7: If "Admin Menu Block" is not automatically activated after module
  installation, make sure you visit the block configuration page (admin/block)
  for each theme you want to have admin_menu activated for.


-- TROUBLESHOOTING --

* If your theme uses absolute or fixed positioned elements and the default
  margin-top for <BODY> is not sufficient, you need to override admin_menu's
  stylesheet in your theme.


-- FAQ --

Q: I enabled "Aggregate and compress CSS files", but I found admin_menu.css is
   still there, is it normal?

A: Yes, this is the intended behavior. Since admin_menu is only visible for
   logged-on administrative users, it would not make sense to load its
   stylesheet for all, including anonymous users.


-- CUSTOMIZATION --

* You have two options to override the admin menu icon:
  
  1) Disable it via CSS in your theme:

     body #admin_menu_icon { display: none; }

  2) Alter the image by overriding the theme function:

     Copy the whole function theme_admin_menu_icon() into your template.php,
     rename it to f.e. phptemplate_admin_menu_icon() and customize the output.


-- THEME-SPECIFIC TROUBLESHOOTING --

No more troubleshooting since release 5.x-1.2. :)


-- AUTHORS --

Daniel F. Kudwien, dev@unleashedmind.com
Stefan M. Kudwien, dev@unleashedmind.com

