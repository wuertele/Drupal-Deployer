Views Display Tabs
------------------

Views Display Tabs exposes a view's displays as links which you can then style
as tabs if you so choose. In the future, this module may provide its own CSS
to display the links as tabs by default. Right now you have to do the
styling yourself.

In order to use this module you will need at least one view which has AJAX
enabled. The view must have at least two displays. As this module was developed
for a specific feature for a client of ours, it's not extremely generic (yet).
It works with views that have (apart from the default display) at least two
displays: page or block (what the user sees first), and one or more displays of
type embed. These will become the tabs the user can click.

You can control what displays to expose as tabs by enter a filter string in the
settings for the view here below. The filter will be matched against the machine
readable id of the display (usually page_1, block_1 or embed_1). If you only
want your displays of type embed to show as tabs, type embed in the
filter field.

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

This module was developed by Jakob Persson with ideas and feedback from
Joakim Stai. Jakob and Joakim both work at NodeOne, Sweden's largest Drupal
consultancy.

Find out more about NodeOne and other modules by us at:
http://nodeone.se
http://drupal.org/node/131670

If you have questions about this module, please refer to the issue queue:
http://drupal.org/project/issues/viewsdisplaytabs

The author may be contacted through his contact form:
http://drupal.org/user/37564

Copyright @ imBridge NodeOne AB 2009