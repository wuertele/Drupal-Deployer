<div class="<?php print $classes; ?>">
<?php print $smallimage; ?>
<div><strong><?php print l($title, $detailpageurl); ?></strong> (<?php print $theatricalreleaseyear; ?>)</div>
<div><strong><?php print t('Director'); ?>:</strong> <?php print $director; ?></div>
<div><strong><?php print t('Rating'); ?>:</strong> <?php print $audiencerating; ?></div>
<div><strong><?php print t('Running time'); ?>:</strong> <?php print t('@time minutes', array('@time' => $runningtime)); ?></div>
</div>
