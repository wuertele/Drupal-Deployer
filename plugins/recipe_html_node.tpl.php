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

<?php if (!$page): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

  <?php if ($recipe_summary_box): ?>
    <div class="recipe-summary"><?php print $recipe_summary_box ?></div>
  <?php endif;?>

  <div class="recipe-description">
    <?php print $recipe_description ?>
  </div>

  <?php if ($recipe_ingredients): ?>
    <div class="recipe-ingredients"><?php print $recipe_ingredients ?></div>
  <?php endif;?>

  <div class="recipe-instructions">
    <?php print $recipe_instructions ?>
  </div>

  <?php if ($recipe_notes): ?>
    <div class="recipe-notes"><?php print $recipe_notes ?></div>
  <?php endif;?>

  <?php if ($content): ?>
    <div class="content"><?php print $content ?></div>
  <?php endif;?>

  <?php print $links; ?>
</div>