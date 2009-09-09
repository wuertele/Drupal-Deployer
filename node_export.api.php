<?php
// $Id$

/**
 * @file
 * Documents node_export's hooks for api reference.
 */

/**
 * Override export access on a node.
 *
 * Let other modules alter this - for example to only allow some users to
 * export specific nodes or types.
 *
 * @param &$access
 *   Boolean access value for current user.
 * @param $node
 *   The node to determine access for.
 */
function hook_node_export_access_alter(&$access, $node) {
  // no example code
}

/**
 * Manipulate a node on import/export.
 *
 * @param &$node
 *   The node to alter.
 * @param $original_node
 *   The unaltered node.
 * @param $method
 *   'export' for exports, and 'prepopulate' or 'save-edit' for imports
 *   depending on the method used.
 */
function hook_node_export_node_alter(&$node, $original_node, $method) {
  // no example code
}

/**
 * Override one line of the export code output.
 *
 * @param &$out
 *   The line of output.
 * @param $tab
 *   The $tab variable from node_export_node_encode().
 * @param $key
 *   The $key variable from node_export_node_encode().
 * @param $value
 *   The $value variable from node_export_node_encode().
 * @param $iteration
 *   The $iteration variable from node_export_node_encode().
 */
function hook_node_export_node_encode_alter(&$out, $tab, $key, $value, $iteration) {
  // Start with something like this, and work on it:
  $out = $tab ."  '". $key ."' => ". node_export_node_encode($value, $iteration) .",\n";
}