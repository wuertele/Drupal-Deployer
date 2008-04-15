<?php
// $Id$

/**
 * Available variables:
 *
 * $nodes represents an array of nodes.
 * Each node stored in the $nodes array has the following information:
 *   $node['question'] represents the question.
 *   $node['body'] represents the answer.
 *   $node['links'] represents the node links, e.g. "Read more".
 * $question_label represents the question label.
 * $answer_label represents the answer label.
 * $use_teaser tells whether $node['body'] contains the full body or just the teaser
 */
?><div>
<?php foreach ($nodes as $node): ?>
  <?php // Cycle through the $nodes array so that we now have a $node variable to work with. ?>
  <br />
  <div class="faq_question">
  <strong>
  <?php print $question_label; ?>
  </strong>
  <?php print $node['question']; ?>
  </div> <!-- Close div: faq_question -->

  <div class="faq_answer">
  <strong>
  <?php print $answer_label; ?>
  </strong>
  <?php print $node['body']; ?>
  <?php print $node['links']; ?>
  </div> <!-- Close div: faq_answer -->
<?php endforeach; ?>
</div> <!-- Close div -->
