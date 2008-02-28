<?php
// $Id$

/**
 * Available variables:
 *
 * $nodes represents an array of nodes.
 * Each node stored in the $nodes array has the following information:
 *   $node['link'] represents a link to the node.
 *   $node['body'] represents the body of the node.
 *   $node['more_link'] represents a "more" link. This may not be available, so check that it exists with isset().
 * $question_label represents the question label.
 * $answer_label represents the answer label.
 * $back_to_top represents a link back to the top of the page.
 */
?><div>
<?php foreach ($nodes as $node): ?>
  <?php // Cycle through the $nodes array so that we now have a $node variable to work with. ?>
  <br />
  <div class="faq_question">
  <strong>
  <?php print $question_label; ?>
  </strong>
  <?php print $node['link']; ?>
  </div> <!-- Close div: faq_question -->

  <div class="faq_answer">
  <strong>
  <?php print $answer_label; ?>
  </strong>
  <?php print $node['body']; ?>
  <p class="faq_top_link">
  <?php print $back_to_top; ?>
  </p>
  <?php if (!empty($node['more_link'])): ?>
    <p class="faq_more_link">
    <?php print $node['more_link']; ?>
    </p>
  <?php endif; ?>
  </div> <!-- Close div: faq_answer -->
<?php endforeach; ?>
</div> <!-- Close div -->
