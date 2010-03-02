<?php
// $Id$
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
  <?php print $styles; ?>
  <?php print $scripts; ?>
 </head>

<body>
<div id="wrapper">
<div id="header">
<div class="logo">
				<?php if ($logo): ?>
					<a href="<?php print $base_path; ?>" title="<?php print t('Home'); ?>"><img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" /></a>
				<?php endif; ?>
				<?php if ($site_name) : ?><h1 class="logo-name"><a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>"><?php print $site_name; ?></a></h1><?php endif; ?>
				<?php if ($site_slogan) : ?><div class='logo-text'><?php print $site_slogan; ?></div><?php endif; ?>
</div>

</div> <!-- end header -->

<div id="menu">

<?php if (isset($primary_links)) : ?>
			<?php print theme('links', $primary_links, array('class' => 'links', 'id' => 'nav')); ?>
		<?php endif; ?>

</div> <!-- end menu -->

<div id="content">
<?php if ($mission) : ?><div id="mission"><?php print $mission; ?></div><?php endif; ?>
			<?php if ($content_top) : ?><div id="content-top" class="node"><?php print $content_top; ?></div>
			<?php endif; ?>
			<?php if (!$is_front) print $breadcrumb; ?>
			<?php if ($show_messages) { print $messages; }; ?>
			<?php if ($tabs) : ?><div class="tabs"><?php print $tabs; ?></div><?php endif; ?>
			<?php if ($title) : ?><h1 class="title"><?php print $title; ?></h1><?php endif; ?>
			<?php print $help; ?>
			<?php print $content; ?>
			<?php print $feed_icons; ?>

</div> <!-- end content -->

<div id="sidebar">
<?php if ($right): ?>
				<?php print $right; ?>
		<?php endif; ?>
</div> <!-- end sidebar -->


<div id="footer">
  <?php print $footer_message; ?>
  <?php print $footer; ?>
</div><!-- end footer -->
<div id="notice">Theme provided by <a href="http://www.danetsoft.com">Danang Probo Sayekti</a>.</div>
</div> <!-- end wrapper -->
<?php print $closure; ?>
</body>
</html>