<?php // $Id$ ?>
<div class="comment <?php print $comment_classes; ?>">
  <div class="comment-inner-0"><div class="comment-inner-1">
    <div class="comment-inner-2"><div class="comment-inner-3">

    <?php if ($title): ?>
      <h3 class="title"><?php print $title; if (!empty($new)): ?> <span class="new"><?php print $new; ?></span><?php endif; ?></h3>   
	<?php endif; ?>

    <?php if ($unpublished): ?>
      <div class="unpublished"><?php print t('Unpublished'); ?></div>
    <?php endif; ?>

    <div class="submitted">
      <?php print $submitted; ?>
    </div>
  
    <?php print $picture; ?>

    <div class="content <?php if ($picture) { print 'with-picture'; } ?>">
      <?php print $content; ?>
    </div>

    <?php if ($links): ?>
      <div class="links clearfix"><?php print $links; ?></div>
    <?php endif; ?>

    </div></div>
  </div></div>
</div> <!-- /comment -->
