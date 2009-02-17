// $Id$

Nodereference Views Select
==========================

Provides a widget for Node Reference which displays the node teasers instead of a select box, and allows choosing the referenced nodes
from a Modal panel. The list is optionally sortable, depending on JQuery UI Sortables. The modal supports pagination, exposed filters
and exposed sorting, all using AJAX.

In order to use:
1) Install the module
2) Create a View with the "Sortable teaser list for Node Reference" style plugin for the page display.
3) Configure any filters you would like for the view. Exposed filters are rendered inside the modal panel. Currently only drop-down filters are 
supported, attempting to use any other filter might result in unexpected behavior (or just won't do anything).
4) To configure sorting within the modal, add fields to the view and set them as sortable. The module will use the Table Header in order to 
generate the sort header.
5) Configure your Node Reference field, select the "Teaser List + Modal Panel using Views" widget. Select the view you have previously created
in the "Advanced - Nodes that can be referenced (View)" field, save the form, then visit this page again to set field settings such as number of
teasers per page in the widget (not modal; not supported yet) or whether the widget teaser list is sortable.

The module depends on Views and CCK (naturally) and on JQuery UI for the sorting behavior.

Module development sponsored by ewave.co.il

shai dot yallin at gmail.com