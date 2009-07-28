;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Measured Value Field module.
;; $Id$
;;
;; Started as fork of Money CCK field (http://drupal.org/project/money)
;;
;; Credit goes to Money CCK module maintainers:
;;   markus_petrux (http://drupal.org/user/39593)
;;   Wim Leers (original author) (http://drupal.org/user/99777)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

OVERVIEW
========

This module defines the Measured Value CCK field. It uses the Units module API,
 to get a list of units to work with.

The form element for amount is reused from the Formatted Number CCK module.
Decimal points and thousands separators are formatted using the Format Number
API module, where these options are configured from site and/or user settings.


REQUIREMENTS
============

- CCK (http://drupal.org/project/cck)
- Units (http://drupal.org/project/units)
- Format Number API (http://drupal.org/project/format_number)
- Formatted Number CCK (http://drupal.org/project/formatted_number)


RECOMMENDED
===========

- Checkall (http://drupal.org/project/checkall)


INSTALLATION
============

- Please, make sure all required modules are installed first.

- Copy all contents of this package to your modules directory preserving
  subdirectory structure.

- Goto Administer > Site building > Modules to install this module.

- Create or edit content types and start adding Money fields. :)
