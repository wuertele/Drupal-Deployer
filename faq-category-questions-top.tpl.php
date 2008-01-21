<?php
// $Id$

/**
 * Available variables:
 *
 * $display_header tells whether the category header should be displayed
 * $category_name represents the category name
 * $header_title represents the link to the category
 * $category_depth (greather than 0 when it's a subcategory)
 * $description
 * $question_count represents the number of questions in category
 * $term_img represents the html for the category image. This is empty if the taxonomy image module is not enabled.
 *
 * $subcat_list represents an array of subcategories
 * Each subcategory stored in the $subcat_list array has the following information:
 *   $subcat['link'] represents the link to the subcategory
 *   $subcat['description']
 *   $subcat['count'] represents the number of question in subcategory
 *   $subcat['img'] represents the subcategory (taxonomy) image
 * $subcat_list_style represents the style of the list, either ol or ul (ordered or unordered)
 * $subcat_body_list represents subcategories, recursively themed (by this template)
 * $container_class is the class attribute of the element containing subcategories, either 'faq_qa' or 'faq_qa_hide'. This is used by javascript to open/hide categories.
 *
 * $question_list represents an array of question links
 * $question_list_style, either ol or ul
 *
 * $answer_category_name tells whether the category name should be displayed before answers
 * $display_faq_count tells whether the number of questions in (sub)categories should be displayed
 * $use_teaser tells whether $node['body'] contains the full body or just the teaser
 *
 * $nodes represents an array of nodes with questions and answers.
 * Each node stored in the $nodes array has the following information:
 *   $node['link'] represents the question and a link to the node (question).
 *   $node['body'] represents the body of the node (answer).
 *   $node['more_link'] represents a "more" link. This may not be available, so check that it exists with isset().
 *
 * $question_label represents the question label.
 * $answer_label represents the answer label.
 * $back_to_top represents a link back to the top of the page.
 */

if ($category_depth > 0) {
  $hdr = 'h6';
}
else {
  $hdr = 'h5';
}

?><div class="faq_category_group">

  <?php // category header with title, link, image, description, and count of questions inside ?>
  <div class="faq_qa_header">
  <?php if ($display_header): ?>
    <<?php print $hdr; ?> class="faq_header">
    <?php print $term_img; ?>
    <?php print $header_title; ?>
    <?php if ($display_faq_count): ?>
      (<?php print $question_count; ?>)
    <?php endif; ?>
    </<?php print $hdr; ?>>
  <?php else: ?>
    <?php print $term_img; ?>
  <?php endif; ?>
  <?php if (!empty($description)): ?>
    <div class="faq_qa_description"><p><?php print $description ?></p></div>
  <?php endif; ?>
  <div class="clear-block"></div>
  </div>

  <?php // list subcategories, with title, link, description, count ?>
  <?php if (!empty($subcat_list)): ?>
    <div class="item-list">
    <<?php print $subcat_list_style; ?> class="faq_category_list">
    <?php foreach ($subcat_list as $i => $subcat): ?>
      <li>
      <?php print $subcat['link']; ?>
      <?php if ($display_faq_count): ?>
        (<?php print $subcat['count']; ?>)
      <?php endif; ?>
      <?php if (!empty($subcat['description'])): ?>
      <div class="faq_qa_description"><p><?php print $subcat['description']; ?></p></div>
      <?php endif; ?>
      <div class="clear-block"></div>
      </li>
    <?php endforeach; ?>
    </<?php print $subcat_list_style; ?>>
  </div>
  <?php endif; ?>

  <div class="<?php print $container_class; ?>">

  <?php // include subcategories ?>
  <?php foreach ($subcat_body_list as $i => $subcat_html): ?>
    <div class="faq_category_indent"><?php print $subcat_html; ?></div>
  <?php endforeach; ?>

  <?php // list question links ?>
  <?php if (!empty($question_list)): ?>
    <div class="item-list">
    <<?php print $question_list_style; ?> class="faq_ul_questions_top">
    <?php foreach ($question_list as $i => $question_link): ?>
      <li>
      <?php print $question_link; ?>
      </li>
    <?php endforeach; ?>
    </<?php print $question_list_style; ?>>
  </div>
  <?php endif; ?>

  <?php //display header before answers in some layouts ?>
  <?php if ($answer_category_name): ?>
    <<?php print $hdr; ?> class="faq_header">
    <?php print $term_img; ?>
    <?php print $category_name; ?>
    </<?php print $hdr; ?>>
    <div class="clear-block"></div>
  <?php endif; ?>

  <?php // list questions (in title link) and answers (in body) ?>
  <div>
  <?php foreach ($nodes as $i => $node): ?>
    <div class="faq_question"><?php //strong question label here? ?>
    <?php print $node['link']; ?>
    </div>
    <div class="faq_answer">
    <strong><?php print $answer_label; ?></strong>
    <?php print $node['body']; ?>
    <?php if (!empty($node['more_link'])): ?>
      <p class="faq_more_link"><?php print $node['more_link']; ?></p>
    <?php endif; ?>
    <?php if (!empty($back_to_top)): ?>
      <p class="faq_top_link"><?php print $back_to_top; ?></p>
    <?php endif; ?>
    </div>
  <?php endforeach; ?>
  </div>

  </div>

</div>
