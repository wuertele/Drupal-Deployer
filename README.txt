Readme
------

This module allows you to manage and display Frequently Asked Questions nodes.
It uses Drupal's taxonomy module to classify questions and provides
FAQ-friendly output.

Note the function theme_faq_highlights(), which shows the top five
recently-created FAQs. This is called in one of the two blocks this module
ships with but can also be called in a php-filtered node if desired. 



Requirements
------------

This module has been tested on Drupal 4.6 and includes a database defintion
for mysql only.



Installation
------------

1. Create the SQL tables. This depends a little on your system, but the most
   common method is:
        mysql -u username -ppassword drupal < faq.mysql

2. Copy faq.module to the Drupal modules/ directory.

3. Enable faq in the "site settings | modules" administration screen.

4. Set access control so that admin users can add FAQs.

5. Create your FAQ categories using Drupal's taxonomy module. The faq module
   currently doesn't support tiered hierarchy.

6. Start adding faq nodes. The questions, broken down by category, appear at
   http://www.yoursite.com/faq .
