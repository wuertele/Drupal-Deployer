<?php
// $Id$

/**
 * Available variables:
 *
 * $category_name represents the category name
 * $category_depth (greather than 0 when it's a subcategory)
 * $term_img represents the html for the category image. This is empty if the taxonomy image module is not enabled.

 * $subcat_body_list represents subcategories' answers, recursively themed (by this template)

 * $display_answers tells whether there should be any output
 * $answer_category_name tells whether the category name should be displayed before answers
 * $display_header tells whether the header should be displayed before answers

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

  <div class="faq_category_group">

  <?php if ($display_header): ?>
    <<?php print $hdr; ?> class="faq_header">
    <?php print $term_img; ?>
    <?php print $category_name; ?>
    </<?php print $hdr; ?>>
    <div class="clear-block"></div>
    <div>
  <?php endif; ?>

  <?php if (!$answer_category_name || $display_header): ?>

    <?php // include subcategories ?>
    <?php foreach ($subcat_body_list as $i => $subcat_html): ?>
      <?php print $subcat_html; ?>
    <?php endforeach; ?>

    <?php if (!$display_header): ?>
      <div>
    <?php endif; ?>

    <?php // list questions (in title link) and answers (in body) ?>
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

  <?php endif; ?>

  </div>

  <?php if ($answer_category_name): ?>
    <?php while ($depth > 0): ?>
      </div>
    <?php $depth--; endwhile; ?>
  <?php endif; ?>
<?php endif; //if display_answers ?>
