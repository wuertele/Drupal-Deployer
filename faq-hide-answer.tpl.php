<?php
// $Id$

/**
 * Available variables:
 *
 * $nodes
 *   The array of nodes to be displayed.
 *   Each $node array contains the following information:
 *     $node['question'] is the question text.
 *     $node['body'] is the answer text.
 *     $node['links'] represents the node links, e.g. "Read more".
 * $use_teaser
 *   Is true if $node['body'] is a teaser.
 */
?><div>
<?php if (count($nodes)): ?>
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
<?php endif; ?>
</div> <!-- Close div -->
