<?php
// $Id$

/**
 * Available variables:
 *
 * $questions_list represents a list of questions based on $list_style.
 * $list_style represents the type of list.
 * $limit represents the number of items.
 * $answers represents an array of answers.
 *   $answers[$key]['link'] represents the link to the node.
 *   $answers[$key]['body'] represents the body of the node.
 * $questions represents an array of questions.
 * $back_to_top represents a link back to the top of the page. If not available, this will be an empty string ('').
 * $more_link represents an array of "more" links. This may not be available under all conditions, so check its existence with isset().
 * $use_teaser is true if $answer['body'] is a teaser.
 */
?>
<?php print $questions_list ?>
<br />
<?php $key = 0; ?>
<?php while ($key < $limit): ?>
  <?php // Cycle through all the answers and "more" links. $key will represent the applicable position in the arrays. ?>
  <div class="faq_question">
  <?php print $answers[$key]['link']; ?>
  </div>
  <div class="faq_answer">
  <?php print $answers[$key]['body']; ?>
  <p class="faq_top_link">
  <?php print $back_to_top ?>
  </p>
  <?php if (isset($more_link)): ?>
    <div class="faq_more_link">
    <?php print $more_link[$key]; ?>
    </div>
  <?php endif; ?>
  </div>
  <?php // Increment $key to move on to the next position. ?>
  <?php $key++; ?>
<?php endwhile; ?>