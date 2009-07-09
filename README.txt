Views Display Tabs
------------------

Views Display Tabs exposes a view's displays as links which you can then style
as tabs if you so choose.

The module requires an AJAX-capable browser. There's no fallback or graceful
degradation for non-AJAX-capable browser at this point.

How to install and use
----------------------

   1. Download the module, extract it, then copy it your modules directory and
      enable it.
   2. Go to admin/settings/viewsdisplaytabs. You will see a list of views that
      have AJAX enabled. If there are no views in the list, make sure there's
      at least one view that has AJAX enabled. See the instructions on the
      settings page for more information.
   3. Check the module you want to enable display tabs for and check the
     displays you want to show as tabs. Choose whether you want the tabs to show
      a throbber (loading animation) while the view is loading.
   4. Your view's header will now contain the displays as item lists. You may
      theme these lists as tabs if you so choose. Clicking a display's link will
      update the view and replace the current display with the one you clicked.
      This only works with AJAX at the moment (no graceful degradation for
      browsers with poor JavaScript support).


REQUIREMENTS
In order to use this module you will need at least one view which has AJAX
enabled. The view must have at least two displays that render as HTML. Displays
of type Block or Page render as HTML. These will become the tabs the user can
click.

DISPLAYS
You can control what displays to expose as tabs by checking them. Click
Displays to reveal a list of displays, check the checkbox next to each of them
to have it show as a tab.

THROBBER
Check 'throbber' to have a throbber (loading animation) show while the tab is
loading.

DEFAULT ACTIVE
The 'default active' select field allows you to set a tab to be active by
default. Doing this won't make the display associated with that tab load by
default it will only *make it seem as if that display were loaded by default*.

This is useful as it will give the site's users the impression that one tab is
already active. They will also conclude that what they see is a result of the
filtering done with that tab and that they can return to that filtering again
simply by clicking that tab.

Note: For this to be of any use, the first display that Views loads *must have
the same configuration as the one you set as default active*.

The first display Views will load is the display used to render the view onto
your page. If you have placed you view in a block, the display of that block
will load first. If your view is a page on your site with its own site path,
then the display with that path will load first.

Make sure that the first display to load, page, block or else (there are several
modules that add additional display plugins to Views) is identical to another
display. Then set that other display as 'default active'.

GROUPING
We had a need to group tabs, why we've made it possible to group tabs using a
separator character. By setting a separator character for a view and then using
the following naming convention for your tabs you may group them. Each group
will then become its own list of links.

Assuming ':' (colon) is used as separator:

[title of group]:[title of tab]

For example:

Articles:Most viewed
Articles:Most commented
Articles:Newest
Blog posts:Most viewed
Blog posts:Most commented
Blog posts:Newest

EDIT
This is a link you can use to quickly edit your view and then get back to
the Views Display Tabs setting as soon as you've saved your changes.

ABOUT
This module was developed by Jakob Persson with ideas and feedback from
Joakim Stai. Jakob and Joakim both work at NodeOne, Sweden's leading Drupal
consultancy.

Find out more about NodeOne and other modules by us at:
http://nodeone.se
http://drupal.org/node/131670

If you have questions about this module, please refer to the issue queue:
http://drupal.org/project/issues/viewsdisplaytabs

The author may be contacted through his contact form:
http://drupal.org/user/37564

Copyright @ imBridge NodeOne AB 2009