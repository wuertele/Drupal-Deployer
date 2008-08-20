<?php // $Id$ ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language; ?>" xml:lang="<?php print $language->language; ?>">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <!--[if IE]>
    <?php if (file_exists($directory . '/ie.css')): ?>
      <link rel="stylesheet" href="<?php print $base_path . $directory; ?>/ie.css" type="text/css">
    <?php endif; ?>
  <![endif]-->
  <?php print $scripts; ?>
</head>
<?php
$pixture_width = theme_get_setting('pixture_width');
$pixture_width = pixture_validate_page_width($pixture_width);
?>
<body class="<?php print $body_classes; ?>">

  <div id="skip-to-content"><a href="#main-content"><?php print t('Skip to Content'); ?></a></div> 
  
    <div id="page" style="width: <?php print $pixture_width; ?>;">
	
      <div id="header">
        <?php if ($logo): ?>
          <div id="logo">
            <a href="<?php print $base_path; ?>" title="<?php print t('Home'); ?>" rel="home"><img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" id="logo-image" /></a>
          </div>
        <?php endif; ?>
		
        <div id="head-elements">
          <div id="branding">
            <?php if ($site_name): ?>
              <?php
                // Use an H1 only on the homepage
                $tag = $is_front ? 'h1' : 'div';
              ?>
              <<?php print $tag; ?> id='site-name'>
                <a href="<?php print $base_path; ?>" title="<?php print t('Home'); ?>" rel="home">
                  <?php print $site_name; ?>
                </a>
              </<?php print $tag; ?>>
            <?php endif; ?>
			
            <?php if ($site_slogan): ?>
              <div id='site-slogan'>
			    <?php print $site_slogan; ?>
			  </div>
            <?php endif; ?> 
          </div> <!-- /#branding -->
			
			<?php if ($search_box): ?>
              <div id="search-box">
                <?php print $search_box; ?>
              </div> <!-- /#search-box -->
            <?php endif; ?>	    
         </div> <!-- /#head-elements -->
		  
		  <?php if ($primary_links): ?>
            <div id="primary">
              <?php print theme('links', $primary_links); ?>
            </div> <!-- /#primary -->
          <?php endif; ?>
    </div> <!--/#header -->
	
    <?php if ($header): ?>
      <div id="header-blocks" class="region region-header">
        <?php print $header; ?>
      </div> <!-- /#header-blocks -->
    <?php endif; ?>

    <div id="main" class="clear-block <?php if ($header) { print ' with-header-blocks'; } ?>">

      <div id="content"><div id="content-inner">

        <?php if ($mission): ?>
          <div id="mission"><?php print $mission; ?></div>
        <?php endif; ?>

        <?php if ($content_top): ?>
          <div id="content-top" class="region region-content_top">
            <?php print $content_top; ?>
          </div> <!-- /#content-top -->
        <?php endif; ?>

        <?php if ($breadcrumb or $title or $tabs or $help or $messages): ?>
          <div id="content-header">
            <?php print $breadcrumb; ?>
			<a name="main-content" id="main-content"></a>
            <?php if ($title): ?>
              <h1 class="title"><?php print $title; ?></h1>
            <?php endif; ?>
            <?php print $messages; ?>
            <?php if ($tabs): ?>
              <div class="tabs"><?php print $tabs; ?></div>
            <?php endif; ?>
            <?php print $help; ?>
          </div> <!-- /#content-header -->
        <?php endif; ?>

        <div id="content-area">
          <?php print $content; ?>
        </div>
		
		<?php if ($content_bottom): ?>
          <div id="content-bottom" class="region region-content_bottom">
            <?php print $content_bottom; ?>
          </div> <!-- /#content-bottom -->
        <?php endif; ?>

        <?php if ($feed_icons): ?>
          <div class="feed-icons"><?php print $feed_icons; ?></div>
        <?php endif; ?>

      </div></div> <!-- /#content-inner, /#content -->

      <?php if ($left): ?>
        <div id="sidebar-left" class="region region-left">
          <?php print $left; ?>
        </div> <!-- /#sidebar-left -->
      <?php endif; ?>

      <?php if ($right): ?>
        <div id="sidebar-right" class="region region-right">
          <?php print $right; ?>
        </div> <!-- /#sidebar-right -->
      <?php endif; ?>

    </div> <!-- #main -->

    <div id="footer" class="region region-footer">
      <?php print $footer; ?>
	  <div id="footer-message"><a href="http://adaptivethemes.com">A Drupal Theme by Adaptivethemes.com</a></div>
      
	  
    </div> <!-- /#footer -->

  </div> <!--/#page -->

  <?php if ($closure_region): ?>
    <div id="closure-blocks" class="region region-closure"><?php print $closure_region; ?></div>
  <?php endif; ?>

  <?php print $closure; ?>

</body>
</html>