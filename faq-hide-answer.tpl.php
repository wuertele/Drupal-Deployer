<?php
// $Id$

/**
 * Available variables:
 *
 * $nodes represents an array of nodes.
 * Each $node array contains the following information:
 *   $node['question'] represents the question.
 *   $node['body'] represents the answer.
 *   $node['links'] represents the node links, e.g. "Read more".
 * $use_teaser is true if $node['body'] is a teaser.
 */
?><div>
<?php foreach ($nodes as $node): ?>
  <?php // Cycle through each of the nodes. We now have the variable $node to work with. ?>
  <div class="faq_question faq_dt_hide_answer">
  <?php print $node['question']; ?>
  </div> <!-- Close div: faq_question faq_dt_hide_answer -->

  <div class="faq_answer faq_dd_hide_answer">
  <?php print $node['body']; ?>
		<?php print $node['links']; ?>
  </div> <!-- Close div: faq_answer faq_dd_hide_answer -->
<?php endforeach; ?>
</div> <!-- Close div -->
