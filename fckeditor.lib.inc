<?php
function fckeditor_load_toolbar_options() {
  $arr = array();
  $module_drupal_path = drupal_get_path('module', 'fckeditor');
  $fckconfig_js = $module_drupal_path .'/fckeditor/fckconfig.js';
  $fckeditor_config_js = $module_drupal_path .'/fckeditor.config.js';
  if (file_exists($fckconfig_js) && is_readable($fckconfig_js)) {
    $fp = @fopen($fckconfig_js, "r");
    if ($fp) {
      while (!feof($fp)) {
        $line = fgets($fp, 1024);
        $matches = array();
        if (preg_match('/FCKConfig\.ToolbarSets\[("|\')(.*?)\\1\]/i', $line, $matches)) {
          $arr[$matches[2]] = ucfirst($matches[2]);
        }
      }
      fclose($fp);
    }
  }
  if (file_exists($fckeditor_config_js) && is_readable($fckeditor_config_js)) {
    $fp = @fopen($fckeditor_config_js, "r");
    if ($fp) {
      while (!feof($fp)) {
        $line = fgets($fp, 1024);
        $matches = array();
        if (preg_match('/FCKConfig\.ToolbarSets\[("|\')(.*?)\\1\]/i', $line, $matches)) {
          $arr[$matches[2]] = ucfirst($matches[2]);
        }
      }
      fclose($fp);
    }
  }

  //oops, we have no information about toolbars, let's use hardcoded array
  if (empty($arr)) {
    $arr = array(
    'Basic' => 'Basic',
    'Default' => 'Default',
    );
  }
  asort($arr);

  return $arr;
}

function fckeditor_load_skin_options() {
  $arr = array();
  $module_drupal_path = drupal_get_path('module', 'fckeditor');
  $skin_dir = $module_drupal_path .'/fckeditor/editor/skins';
  if (is_dir($skin_dir)) {
    $dh = @opendir($skin_dir);
    if (FALSE !== $dh) {
      while (($file = readdir($dh)) !== FALSE ) {
        if (in_array($file, array(".", "..", "CVS", ".svn"))) {
          continue;
        }
        if (is_dir($skin_dir . DIRECTORY_SEPARATOR . $file)) {
          $arr[$file] = ucfirst($file);
        }
      }
      closedir( $dh );
    }
  }

  //oops, we have no information about skins, let's use only default
  if (empty($arr)) {
    $arr = array(
    'default' => 'Default',
    );
  }
  asort($arr);

  return $arr;
}

function fckeditor_load_lang_options() {
  $arr = array();
  $module_drupal_path = drupal_get_path('module', 'fckeditor');
  $lang_dir = $module_drupal_path .'/fckeditor/editor/lang';
  if (is_dir($lang_dir)) {
    $dh = @opendir($lang_dir);
    if (FALSE !== $dh ) {
      while (($file = readdir($dh)) !== FALSE) {
        if (in_array($file, array(".", "..", "CVS", ".svn"))) {
          continue;
        }
        $matches = array();
        if (is_file($lang_dir . DIRECTORY_SEPARATOR . $file) && preg_match('/^(.*?)\.js$/', $file, $matches)) {
          $lang = $matches[1];
          $arr[$lang] = strtoupper($lang);
        }
      }
      closedir( $dh );
    }
  }

  //oops, we have no information about languages, let's use those available in FCKeditor 2.4.3
  if (empty($arr)) {
    $arr = array(
    'af' => 'Afrikaans',
    'ar' => 'Arabic',
    'bg' => 'Bulgarian',
    'bn' => 'Bengali/Bangla',
    'bs' => 'Bosnian',
    'ca' => 'Catalan',
    'cs' => 'Czech',
    'da' => 'Danish',
    'de' => 'German',
    'el' => 'Greek',
    'en' => 'English',
    'en-au' => 'English (Australia)',
    'en-ca' => 'English (Canadian)',
    'en-uk' => 'English (United Kingdom)',
    'eo' => 'Esperanto',
    'es' => 'Spanish',
    'et' => 'Estonian',
    'eu' => 'Basque',
    'fa' => 'Persian',
    'fi' => 'Finnish',
    'fo' => 'Faroese',
    'fr' => 'French',
    'gl' => 'Galician',
    'he' => 'Hebrew',
    'hi' => 'Hindi',
    'hr' => 'Croatian',
    'hu' => 'Hungarian',
    'it' => 'Italian',
    'ja' => 'Japanese',
    'km' => 'Khmer',
    'ko' => 'Korean',
    'lt' => 'Lithuanian',
    'lv' => 'Latvian',
    'mn' => 'Mongolian',
    'ms' => 'Malay',
    'nb' => 'Norwegian Bokmal',
    'nl' => 'Dutch',
    'no' => 'Norwegian',
    'pl' => 'Polish',
    'pt' => 'Portuguese (Portugal)',
    'pt-br' => 'Portuguese (Brazil)',
    'ro' => 'Romanian',
    'ru' => 'Russian',
    'sk' => 'Slovak',
    'sl' => 'Slovenian',
    'sr' => 'Serbian (Cyrillic)',
    'sr-latn' => 'Serbian (Latin)',
    'sv' => 'Swedish',
    'th' => 'Thai',
    'tr' => 'Turkish',
    'uk' => 'Ukrainian',
    'vi' => 'Vietnamese',
    'zh' => 'Chinese Traditional',
    'zh-cn' => 'Chinese Simplified',
    );
  }

  asort($arr);

  return $arr;
}