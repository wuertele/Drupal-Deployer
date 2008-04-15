<?php
// $Id$

/**
 * Available variables:
 *
 * $questions_list represents a list of questions based on $list_style.
 * $list_style represents the type of list.
 * $limit represents the number of items.
 * $answers represents an array of answers.
 *   $answers[$key]['question'] represents the question.
 *   $answers[$key]['body'] represents the answer.
 *   $answers[$key]['links'] represents the node links, e.g. "Read more".
 * $questions represents an array of questions.
 * $use_teaser is true if $answer['body'] is a teaser.
 */
?>
<?php print $questions_list ?>
<br />
<?php $key = 0; ?>
<?php while ($key < $limit): ?>
  <?php // Cycle through all the answers and "more" links. $key will represent the applicable position in the arrays. ?>
  <div class="faq_question">
  <?php print $answers[$key]['question']; ?>
  </div> <!-- Close div: faq_question -->

  <div class="faq_answer">
  <?php print $answers[$key]['body']; ?>
  <?php print $answers[$key]['links']; ?>
  </div> <!-- Close div: faq_answer -->
  <?php // Increment $key to move on to the next position. ?>
  <?php $key++; ?>
<?php endwhile; ?>
