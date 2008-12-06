<?php // $Id$
/**
 * @file
 *  node.tpl.php
 *
 * Theme implementation to display a node.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
?>
<div class="node <?php print $node_classes; ?> <?php if (is_front) { print 'front-node'; } ?>" id="node-<?php print $node->nid; ?>">
  <div class="node-inner-0"><div class="node-inner-1">
    <div class="node-inner-2"><div class="node-inner-3">

      <?php if ($page == 0): ?>
        <h2 class="title"><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
      <?php endif; ?>

      <?php if ($unpublished): ?>
        <h4 class="unpublished"><?php print t('Unpublished'); ?></h4>
      <?php endif; ?>

      <?php if (!empty($picture)) print $picture; ?>

      <?php if (!empty($submitted)): ?>
        <div class="submitted"><abbr title="<?php print format_date($node->created, 'custom', "l, F j, Y - H:i"); ?>">
	       <?php print $date; ?></abbr> <?php print t('by'); ?> <em><?php print $name; ?></em>
        </div>
      <?php endif; ?>

      <?php if (count($taxonomy)): ?>
        <div class="taxonomy"><?php print t('Posted in ') . $terms; ?></div>
      <?php endif; ?>

      <div class="content clearfix">
        <?php print $content; ?>
      </div>

      <?php if (!empty($links)): ?>
        <div class="actions clearfix"><?php print $links; ?></div>
      <?php endif; ?>

    </div></div>
  </div></div>
</div> <!-- /node -->