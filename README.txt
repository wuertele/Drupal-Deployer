// $Id:

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * TODO
 * Database Information

INTRODUCTION
------------

Current Maintainers: brdwor, drawk, marble, and tzoscott 
Original Author: Moshe Weitzman <weitzman@tejasa.com>

Recipe is a module for sharing cooking recipes. 


INSTALLATION
------------
1. Upload and install the module.

2. Adjust Permissions for user roles. NOTE: a 'site editor' role is supported.

3. In Recipe Admin, enable/disable desired features.

4. Enable a Recipes menu item so users may find it...

5. OPTIONAL: Create a taxonomy vocabulary and name it.
   For example: 'Recipe Tags'.

   Under Content Types check 'Recipe', and under Settings check desired 
   options and Save.

   Be sure to create at least one Term.

   In Menus, enable a link to Recipes so users may access the module.


TODO
-----

- Get ingredients into the searchable Index. Requires some SQL expertise. 
  See recipe_update_index()
- emit recipeXML for syndicating recipes. Anyone know of a standard format?
- Let users maintain their own recipe collection just like a blog or 
  personal image gallery
- Integrate with bookmarks.module so users may create a 'recipe box' listing
  the favorite recipes
- Views2 support, including ingredients display.
- Investigate CCK Multigroup and Fields for D7.


DATABASE DESCRIPTION
--------------------

Data is saved in a quite normalized manner. Recipes are collections of 
pointers to ingredients and to quantity terms. New terms can be added by
modifying the schema. New ingredients are added automatically whenever they
are used for the first time. 

Following is an ASCII art attempt to illustrate the DB relationships:

node.nid +--------------+     +------------------+     +-------------+     +--------------+
     ^   | recipe       |     | _node_ingredient |     | _ingredient |     | _unit        |
     |   +--------------+     +------------------+     +-------------+     +--------------+
     +---| nid          |<--  | id               |   +-| id          |  +--| id           |
         | source       |  +--| nid              |   | | name        |  |  | name         |
         | yield        |     | unit_id          |<-+| | link        |  |  | abbreviation |
         | instructions |     | quantity         |  || +-------------+  |  | metric       |
         | notes        |     | ingredient_id    |<-|+                  |  | type         |
         | preptime     |     +------------------+  +-------------------+  +--------------+
         +--------------+

