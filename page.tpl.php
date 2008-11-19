<?php // $Id$
/**
 * @file
 *  page.tpl.php
 *
 * Theme implementation to display a single Drupal page.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language; ?>" xml:lang="<?php print $language->language; ?>">
<head>
  <title><?php print $head_title; ?></title>
		<meta http-equiv="Content-Style-Type" content="text/css" />
  <?php print $head; ?>
  <?php print $styles; ?>
  <!--[if IE]>
    <?php if (file_exists($directory .'/ie.css')): ?>
      <link type="text/css" rel="stylesheet" media="all" href="<?php print $base_path . $directory; ?>/ie.css" >
    <?php endif; ?>
  <![endif]-->
	<?php print $scripts; ?>
  <?php if ((theme_get_setting('pixture_superfish')) && ($superfish)): ?>
    <script type="text/javascript">
      $(document).ready(function() {
        $('#superfish-inner ul').superfish();
      });
    </script>
  <?php endif; ?>
</head>
<?php
  $pixture_width = theme_get_setting('pixture_width');
  $pixture_width = pixture_validate_page_width($pixture_width);
?>
<body id="pixture-reloaded" class="<?php print $body_classes; ?> <?php print $logo ? 'with-logo' : 'no-logo' ; ?>">
<!--withsf-->
  <div id="skip-to-content"><a href="#main-content"><?php print t('Skip to Content'); ?></a></div>

    <div id="page" style="width: <?php print $pixture_width; ?>;">

      <div id="header">

        <?php if (!empty($logo)): ?>
          <div id="logo">
            <a href="<?php print $base_path; ?>" title="<?php print t('Home'); ?>" rel="home">
              <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" id="logo-image" />
            </a>
          </div>
        <?php endif; ?>

        <div id="head-elements">

          <?php if (!empty($search_box)): ?>
            <div id="search-box">
              <?php print $search_box; ?>
            </div> <!-- /#search-box -->
          <?php endif; ?>

          <div id="branding">
            <?php if (!empty($site_name)): ?>
              <?php
                // Use an H1 only on the homepage
                $tag = $is_front ? 'h1' : 'div';
              ?>
              <<?php print $tag; ?> id="site-name">
                <a href="<?php print $base_path; ?>" title="<?php print t('Home'); ?>" rel="home">
                  <strong><?php print $site_name; ?></strong>
                </a>
              </<?php print $tag; ?>>
            <?php endif; ?>
            <?php if (!empty($site_slogan)): ?>
              <div id="site-slogan"><em><?php print $site_slogan; ?></em></div>
            <?php endif; ?>
          </div> <!-- /#branding -->

        </div> <!-- /#head-elements -->

        <?php if ((!empty($primary_links)) || (!empty($superfish))): ?>
          <!-- Primary || Superfish -->
          <div id="<?php print $primary_links ? 'primary' : 'superfish' ; ?>">
            <div id="<?php print $primary_links ? 'primary' : 'superfish' ; ?>-inner">
              <?php if (!empty($primary_links)) {
                      print theme('links', $primary_links);
                    }
                    elseif (!empty($superfish)) {
                      print $superfish;
                    }
              ?>
            </div> <!-- / inner -->
          </div> <!-- /primary || superfish -->
        <?php endif; ?>

    </div> <!--/#header -->

    <?php if (!empty($header)): ?>
      <div id="header-blocks" class="region region-header">
        <?php print $header; ?>
      </div> <!-- /#header-blocks -->
    <?php endif; ?>

    <div id="main" class="clear-block <?php print $header ? 'with-header-blocks' : 'no-header-blocks' ; ?>">

      <div id="content"><div id="content-inner">

        <?php if (!empty($mission)): ?>
          <div id="mission"><?php print $mission; ?></div>
        <?php endif; ?>

        <?php if (!empty($content_top)): ?>
          <div id="content-top" class="region region-content_top">
            <?php print $content_top; ?>
          </div> <!-- /#content-top -->
        <?php endif; ?>

        <div id="content-header" class="clearfix">
          <?php if (!empty($breadcrumb)): ?><?php print $breadcrumb; ?><?php endif; ?>
          <a name="main-content" id="main-content"></a>
          <?php if (!empty($title)): ?><h1 class="title"><?php print $title; ?></h1><?php endif; ?>
          <?php if (!empty($tabs)): ?><div class="tabs"><?php print $tabs; ?></div><?php endif; ?>
					<?php if (!empty($messages)): print $messages; endif; ?>
          <?php if (!empty($help)): print $help; endif; ?>
        </div> <!-- /#content-header -->

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

      <?php if (!empty($left)): ?>
        <div id="sidebar-left" class="region region-left">
          <?php print $left; ?>
        </div> <!-- /#sidebar-left -->
      <?php endif; ?>

      <?php if (!empty($right)): ?>
        <div id="sidebar-right" class="region region-right">
          <?php print $right; ?>
        </div> <!-- /#sidebar-right -->
      <?php endif; ?>

    </div> <!-- #main -->

    <div id="footer" class="region region-footer">
      <?php if (!empty($footer)): print $footer; endif; ?>
      <div id="footer-message">
        <?php print $footer_message; ?>
      </div> <!-- /#footer-message -->
    </div> <!-- /#footer -->

  </div> <!--/#page -->

  <?php if (!empty($closure_region)): ?>
    <div id="closure-blocks" class="region region-closure">
      <?php print $closure_region; ?>
    </div>
  <?php endif; ?>

  <?php print $closure; ?>

</body>
</html>