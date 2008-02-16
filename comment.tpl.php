<?php
// $Id$

/**
 * @file comment.tpl.php
 * Default theme implementation for comments.
 *
 * Available variables:
 * - $author: Comment author. Can be link or plain text.
 * - $content: Body of the post.
 * - $date: Date and time of posting.
 * - $links: Various operational links.
 * - $new: New comment marker.
 * - $picture: Authors picture.
 * - $signature: Authors signature.
 * - $status: Comment status. Possible values are:
 *   comment-unpublished, comment-published or comment-review.
 * - $submitted: By line with date and time.
 * - $title: Linked title.
 *
 * These two variables are provided for context.
 * - $comment: Full comment object.
 * - $node: Node object the comments are attached to.
 *
 * @see template_preprocess_comment()
 * @see theme_comment()
 */
?>
<?php if ($comment->preview): ?>
<div class="comment<?php print ($comment->new) ? ' comment-new' : ''; print ' '. $status ?> clear-block">
  <?php if ($comment->new): ?>
    <span class="new"><?php print $new ?></span>
  <?php endif; ?>

  <div class="commentcontent">
    <h3><?php print $title ?></h3>
    <?php print $content ?>
    <?php if ($signature): ?>
    <div class="user-signature clear-block">
      <?php print $signature ?>
    </div>
    <?php endif; ?>
    <?php print $links ?>
  </div>
  <p class="authorcompreview"><span>Submitted by <?php print $author;?></span><?php if ($moderation_message) print " <em>". $moderation_message ."</em>"; ?><br /><small class="commentmetadata">on <?php print $date; ?></small></p>

</div>
<?php else: ?>
<?php if ($first) print '<ol class="commentlist">'; ?>
<li class="<?php print $comment_zebra ?>">
<div class="comment<?php print ($comment->new) ? ' comment-new' : ''; print ' '. $status ?> clear-block">
  <?php if ($comment->new): ?>
    <span class="new"><?php print $new ?></span>
  <?php endif; ?>

  <div class="commentcontent">
    <h3><?php print $title ?></h3>
    <?php print $content ?>
    <?php if ($signature): ?>
    <div class="user-signature clear-block">
      <?php print $signature ?>
    </div>
    <?php endif; ?>
    <?php print $links ?>
  </div>
  <p class="authorcom"><span>Submitted by <?php print $author;?></span><?php if ($moderation_message) print " <em>". $moderation_message ."</em>"; ?><br /><small class="commentmetadata">on <?php print $date; ?></small></p>

</div>
</li>
<?php if ($last) print '<ol class="commentlist">'; ?>
<?php endif; ?>
