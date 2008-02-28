<?php
// $Id$

/**
 * Available variables:
 *
 * $nodes represents an array of nodes.
 * Each $node array contains the following information:
 *   $node['link'] represents the link to the node.
 *   $node['body'] represents the question.
 *   $node['more_link'] represents a "more" link, if available.
 * $use_teaser is true if $node['body'] is a teaser.
 */
?><div>
<?php foreach ($nodes as $node): ?>
  <?php // Cycle through each of the nodes. We now have the variable $node to work with. ?>
  <div class="faq_question faq_dt_hide_answer">
  <?php print $node['link']; ?>
  </div> <!-- Close div: faq_question faq_dt_hide_answer -->

  <div class="faq_answer faq_dd_hide_answer">
  <?php print $node['body']; ?>
  <?php if (!empty($node['more_link'])): ?>
    <p class="faq_more_link">
    <?php print $node['more_link']; ?>
    </p>
  <?php endif; ?>
  </div> <!-- Close div: faq_answer faq_dd_hide_answer -->
<?php endforeach; ?>
</div> <!-- Close div -->
