<?php

// this script is needed if upgrading from a version before 1/10/2004
// to run this script, copy it to the root of your drupal site.
// moves 'cooking instructions' from node.body to recipe.instructions and sets node.body to everything concatenated

include_once "includes/bootstrap.inc";
include_once "includes/common.inc";

if ($_POST['op'] == t('Update')) {
  $result = db_query("SELECT n.*, r.* FROM recipe r JOIN node n ON r.nid = n.nid");
  while ($node = db_fetch_object($result)) {
    db_query("UPDATE recipe SET instructions = '%s' WHERE nid = %d", $node->body, $node->nid);
    $result2 = db_query("SELECT ingredient FROM recipe_ingredients WHERE nid = $node->nid");
    while ($ingredient = db_fetch_object($result2)) {
      $ingredients[] = $ingredient->ingredient;
    }
    $body = array($node->body, $node->source, $node->yield, $node->preptime, $node->notes);
    $body = array_merge($body, $ingredients);
    $body = '<p>'. implode('</p><p>', $body). '</p>';
    db_query("UPDATE node SET body = '%s' WHERE nid = %d", $body, $node->nid);
    $i++;
  }
  print "$i recipes updated";
}
else {
  $output = form_item(t('Confirm'), t('Do you really want to upgrade all your recipes to the new format? Please backup your <code>node</code> and <code>recipe</code> tables beforehand.'));
  $output .= form_button(t('Update'));
  print form($output);
}
  
?>