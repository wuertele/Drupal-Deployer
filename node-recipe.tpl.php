<?php
// $Id$

/**
 * @file node-recipe.tpl.php
 *
 * Theme implementation to display a recipe.
 */
?>
<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block">

<?php print $picture ?>

<?php if (!$page): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

  <div class="meta">
  <?php if ($submitted): ?>
    <span class="submitted"><?php print $submitted ?></span>
  <?php endif; ?>

  <?php if ($terms): ?>
    <div class="terms terms-inline"><?php print $terms ?></div>
  <?php endif;?>
  </div>

  <?php if (isset($node->content['recipe_summary_box'])): ?>
    <div class="recipe-summary">
      <?php print $node->content['recipe_summary_box']['#value'] ?>
    </div>
  <?php endif;?>

  <div class="recipe-description">
    <?php print $node->content['recipe_description']['#value'] ?>
  </div>

  <div class="recipe-ingredients">
    <?php print $node->content['recipe_ingredients']['#value'] ?>
  </div>

  <div class="recipe-instructions">
    <?php print $node->content['recipe_instructions']['#value'] ?>
  </div>

  <?php if (isset($node->content['recipe_notes'])): ?>
    <div class="recipe-notes">
      <?php print $node->content['recipe_notes']['#value'] ?>
    </div>
  <?php endif;?>

  <?php print $links; ?>
</div>