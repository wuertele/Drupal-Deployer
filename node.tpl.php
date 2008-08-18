<?php // $Id$ ?>
<div class="node <?php print $node_classes; ?>" id="node-<?php print $node->nid; ?>">
  <div class="node-inner-0"><div class="node-inner-1">
    <div class="node-inner-2"><div class="node-inner-3">
	
      <?php if ($page == 0): ?>
        <h2 class="title"><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
      <?php endif; ?>

      <?php if ($unpublished): ?>
        <div class="unpublished"><?php print t('Unpublished'); ?></div>
      <?php endif; ?>

      <?php if ($picture) print $picture; ?>

      <?php if ($submitted): ?>
        <div class="submitted"><abbr title="<?php print format_date($node->created, 'custom', "l, F j, Y - H:i"); ?>">
	      <?php print format_date($node->created, 'custom', "F j, Y"); ?></abbr> <?php print t('by'); ?> <em><?php print $name; ?></em>
        </div>
      <?php endif; ?>

      <?php if (count($taxonomy)): ?>
        <div class="taxonomy"><?php print t('Posted in ') . $terms; ?></div>
      <?php endif; ?>

      <div class="content clearfix">
        <?php print $content; ?>
      </div>

      <?php if ($links): ?>
        <div class="actions clearfix"><?php print $links; ?></div>
      <?php endif; ?>
		  
    </div></div>
  </div></div>
</div> <!-- /node -->
