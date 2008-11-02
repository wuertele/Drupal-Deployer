;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Format Number module for Drupal
;; $Id$
;;
;; Original author: markus_petrux at drupal.org (October 2008)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

OVERVIEW
========

This module provides a method to configure number formats (site default and user
defined) with configurable decimal point and thousand separators.

The function <code>format_number($number, $decimals = 0)</code> can be used by
other contributed or custom modules to display numbers accordingly.

External references:
- http://www.php.net/number_format
- http://en.wikipedia.org/wiki/Decimal_separator


INSTALLATION
============

- Copy all contents of this package to your modules directory preserving
  subdirectory structure.

- Goto Administer > Site building > Modules to install this module.

- Optionally goto Administer > User management > Permissions to assign the
  "configure default number format" permission to roles of your choice.

- Goto Administer > Site configuration > Number format to configure default
  number format for the site. You may also allow users to set override these
  options from their profiles.

- When user-configurable option is enabled, a new fieldset (Number format
  settings) is available when the user profile is edited.


USAGE
=====

- Other modules may use this format_number() function exposed by this module.

/**
 * Format a number with (site default or user defined) thousands separator and
 * decimal point.
 *
 * @param float $number
 *   The number being formatted.
 * @param int $decimals
 *   Number of decimal digits.
 */
function format_number($number, $decimals = 0) {}
