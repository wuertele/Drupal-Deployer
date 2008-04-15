<?php
// $Id$

/**
 * Available variables:
 *
 * $display_header tells whether the category header should be displayed.
 * $header_title represents the link to the category.
 * $category_name represents the category name.
 * $category_depth (greater than 0 when it's a subcategory).
 * $description
 * $question_count represents the number of questions in category.
 * $term_img represents the html for the category image. This is empty if the taxonomy image module is not enabled.
 *
 * $subcat_body_list represents subcategories, recursively themed (by this template)
 * $answer_category_name tells whether the category name should be displayed before answers
 * $display_faq_count tells whether the number of questions in (sub)categories should be displayed
 * $display_answers tells whether there should be any output
 * $use_teaser tells whether $node['body'] contains the full body or just the teaser
 *
 * $nodes represents an array of nodes with questions and answers.
 * Each node stored in the $nodes array has the following information:
 *   $node['question'] represents the question.
 *   $node['body'] represents the answer.
 *   $node['links'] represents the node links, e.g. "Read more".
 */


if ($category_depth > 0) {
  $hdr = 'h6';
}
else {
  $hdr = 'h5';
}

$depth = 0;

?><?php if ($display_answers): ?>
  <?php if ($answer_category_name): ?>
    <?php while ($depth < $category_depth): ?>
      <div class="faq_category_indent">
    <?php $depth++; endwhile; ?>
  <?php endif; ?>

  <div class="faq_category_menu">

  <?php if ($display_header): ?>
    <<?php print $hdr; ?> class="faq_header">
    <?php print $term_img; ?>
    <?php print $category_name; ?>
    </<?php print $hdr; ?>>
    <div class="clear-block"></div>
    <div class="faq_category_group">
    <div>
  <?php endif; ?>

  <?php if (!$answer_category_name || $display_header): ?>

    <!-- include subcategories -->
    <?php foreach ($subcat_body_list as $i => $subcat_html): ?>
      <?php print $subcat_html; ?>
    <?php endforeach; ?>

    <?php if (!$display_header): ?>
      <div class="faq_category_group">
      <div>
    <?php endif; ?>

    <!-- list questions (in title link) and answers (in body) -->
    <?php foreach ($nodes as $i => $node): ?>

      <div class="faq_question"><?php //strong question label here? ?>
      <?php print $node['question']; ?>
      </div> <!-- Close div: faq_question -->

      <div class="faq_answer">
      <strong><?php print $answer_label; ?></strong>
      <?php print $node['body']; ?>
      <?php print $node['links']; ?>
      </div> <!-- Close div: faq_answer -->

    <?php endforeach; ?>

  <?php endif; ?>

  </div> <!-- Close div -->
  </div> <!-- Close div: faq_category_group -->

  <?php if ($answer_category_name): ?>
    <?php while ($depth > 0): ?>
      </div> <!-- Close div: faq_category_indent -->
    <?php $depth--; endwhile; ?>
  <?php endif; ?>
<?php endif; //if display_answers ?>
