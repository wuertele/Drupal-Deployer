$Id$

CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * Frequently Asked Questions (FAQ)
 * Known Issues
 * More Information
 * How Can You Contribute?


INTRODUCTION
------------

Current Maintainer: Dave Reid <http://drupal.org/user/53892>
Co-maintainer: Kiam <http://drupal.org/user/55077>
Co-maintainer: Earnie <http://drupal.org/user/86710>
Co-maintainer: Darren Oh <http://drupal.org/user/30772>
Original Author: Matthew Loar <http://drupal.org/user/24879>

XML Sitemap automatically creates a sitemap that conforms to the sitemaps.org
specification. This helps search engines keep their search results up to date.


INSTALLATION
------------

See http://drupal.org/getting-started/5/install-contrib for instructions on
how to install or update Drupal modules.

Once XML Sitemap is installed and enabled, you can adjus the settings for your
site's sitemap at admin/settings/xmlsitemap. Your can view your site's sitemap
at http://yoursite.com/sitemap.xml.

It is highly recommended that you have clean URLs enabled for this module.


FREQUENTLY ASKED QUESTIONS (FAQ)
--------------------------------

- There are no frequently asked questions at this time.


KNOWN ISSUES
------------

- I get the following error from Google Webmaster Tools:
    "Invalid date - An invalid date was found. Please fix the date or formatting
    before resubmitting.- Parent tag: url"
  Most likely you are using PHP 5.1.1 or PHP 5.1.2, which introduced a new
  date/time constant DATE_W3C but forgot to include a semicolon in the timezone
  separator. This bug has been fixed in PHP 5.1.3 and above. You should upgrade.


MORE INFORMATION
----------------

- To issue any bug reports, feature or support requests, see the module issue
  queue at http://drupal.org/project/issues/xmlsitemap.

- For additional documentation, see the online module handbook at
  http://drupal.org/handbook/modules/gsitemap.

- You can view the sitemap.org specification at http://sitemaps.org.

- You can view the module's API documentation at
  http://project.davereid.net/api/xmlsitemap


HOW CAN YOU CONTRIBUTE?
---------------------

- Write a review for this module at drupalmodules.com.
  http://drupalmodules.com/module/xml-sitemap

- Help translate this module on launchpad.net.
  http://project.davereid.net/translate/projects/xmlsitemap

- Help keep development active by dontating to the maintainer's DrupalCon Paris
  fund. http://davereid.chipin.com/

- Report any bugs, feature requests, etc. in the issue tracker.
  http://drupal.org/project/issues/xmlsitemap
