/* $Id$ */

-- SUMMARY --

Drupal Administration Menu is re-building and automagically slicing the whole
menu tree below /admin including all invisible local tasks into the Drupal
Administration Menu. So administrators need less time to access pages which
are only visible after one or two clicks normally.

For a full description visit the project page:
  http://drupal.org/project/admin_menu
Bug reports, feature suggestions and latest developments:
  http://drupal.org/project/admin_menu/issues


-- REQUIREMENTS --

None.


-- INSTALLATION --

* Copy admin_menu module to your modules directory and enable it on the admin
  modules page.

* Drupal 4.7: If "Admin Menu Block" is not automatically activated after module
  installation, make sure you visit the block configuration page (admin/block)
  for each theme you want to have admin_menu activated for.


-- CUSTOMIZATION --

* You have two options to override the admin menu icon:
  
  1) Disable it via CSS in your theme:

     body #admin_menu_icon { display: none; }

  2) Alter the image by overriding the theme function:

     Copy the whole function theme_admin_menu_icon() into your template.php,
     rename it to f.e. phptemplate_admin_menu_icon() and customize the output.


-- TROUBLESHOOTING --

* If your theme uses absolute or fixed positioned elements and the default
  margin-top for <BODY> is not sufficient, you need to override admin_menu's
  stylesheet in your theme.


-- FAQ --

Q: After upgrading to 5.x-1.2, admin_menu disappeared. Why?

A: Prior to release 5.x-1.2, Drupal Administration Menu was output in a block.
   Since 5.x-1.2, it is output via hook_footer(). Some custom themes may not
   (yet) output $closure, so admin_menu could no longer be displayed. If you
   decided to move the 'administer' tree into a new menu and disabled that menu
   block, a site could become (temporarily) unmaintainable. Either way, you
   should fix your theme by adding the following code in front of the closing
   HTML (</html>) tag:
<code>
   <?php echo $closure; ?>
</code>

Q: I enabled "Aggregate and compress CSS files", but I found admin_menu.css is
   still there, is it normal?

A: Yes, this is the intended behavior. Since admin_menu is only visible for
   logged-on administrative users, it would not make sense to load its
   stylesheet for all, including anonymous users.


-- THEME-SPECIFIC TROUBLESHOOTING --

No more troubleshooting since release 5.x-1.2. :)


-- CONTACT --

Current maintainers:
* Daniel F. Kudwien (sun) - dev@unleashedmind.com
* Stefan M. Kudwien (smk-ka) - dev@unleashedmind.com

This project has been sponsored by:
* UNLEASHED MIND
  Specialized in consulting and planning of Drupal powered sites, UNLEASHED
  MIND offers installation, development, theming, customization, and hosting
  to get you started. Visit http://www.unleashedmind.com for more information.

