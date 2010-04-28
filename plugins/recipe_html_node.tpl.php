<?php
// $Id$

/**
 * @file node-recipe.tpl.php
 *
 * Theme implementation to display a recipe.
 */
?>
<div id="node-<?php print $node->nid; ?>" class="node clear-block">

<?php print $picture ?>

  <h1><?php print $node->title ?></h1>

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