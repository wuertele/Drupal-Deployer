; Id$
=======================
Arrange Fields
=======================
Richard Peacock (richard@richardpeacock.com)

This module lets you drag-and-drop the fields of any CCK content type into the 
positions you would like for editing. This makes it super simple to have forms 
with inline fields, which you can change at any point. Tab indexing is also respected, 
so no matter how you arrange the fields, the users can still tab through them easily.

=======================
Restrictions
=======================
 - This module does not work so well with fields with "unlimited" as their number of
   values.
 
 - Fields within fieldsets cannot be arranged (yet).  But, the fieldset itself
   can be re-arranged.
   
   
======================
Directions
======================

- Unpack the module files into /sites/all/modules/arrange_fields.

- Visit your admin/build/modules page in Drupal and enable the module.

- Visit your admin/user/permissions and give authorized users the
  "administer arrange fields" permission, if desired.  (Otherwise, only the
  admin user will be able to use it).
