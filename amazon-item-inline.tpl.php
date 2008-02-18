<?php
// $Id$

/**
 * @file amazon-item.tpl.php
 *
 * Theme template to display an Amazon.com product.
 *
 * Available variables:
 * - $title: The (sanitized) title of the product.
 * - $type: The css-friendly version of the items's Amazon product group.
 * - $detailpageurl: The URL of the product's page on Amazon.com.
 * - $image: The default image of the product.
 * - $participants: A sanitized array of authors, actors, etc who
 *   participated in the creation of the product.
 * - $image: The default image of the product.
 *
 * - many others I haven't written down yet
 *
 * Other variables:
 * - $item: Full Amazon product record. Contains data that may not be safe.
 * - $variation: The level of detail requested by the calling function.
 */
?>
<span class="amazon-item amazon-item-<?php print strtolower($type); ?>"><?php print l($title, $detailpageurl, array('class' => 'amazon-link')); ?><?php if($participants) { print ' (by '. implode(', ', $participants) .')'; } ?></span>
