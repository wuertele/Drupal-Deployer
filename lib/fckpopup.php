<?php 
/*
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2005 Frederico Caldeira Knabben
 * 
 * Licensed under the terms of the GNU Lesser General Public License:
 *     http://www.opensource.org/licenses/lgpl-license.php
 * 
 * For further information visit:
 *     http://www.fckeditor.net/
 * 
 * "Support Open Source software. What about a donation today?"
 * 
 * File Name: fckpopup.php
 *   full size FCKeditor window that gets and save text from parent window.
 *  Taylored for use by Drupal,but could be customised for anything else.
 *  Accepts the following inputs:
 *    fckpopup.php?name=$name&SkinPath=$SkinPath&Toolbar=$Toolbar
 *  where:
 *    $name = name of window, not important to functionality
 *    $SkinPath = point to chosed FCKeditor skin
 *    $Toolbar = sets which toolbar to use
 *    
 * 
 * File Authors:
 *     Davide (davide@skeda.com.au)
 */
$FCKname     = ($HTTP_GET_VARS["name"]) 
               ? " - (" . $HTTP_GET_VARS["name"] . ")" 
               : null;
$FCKSkinPath = ($HTTP_GET_VARS["SkinPath"]) 
               ? "oFCKeditor.Config['SkinPath'] = '" . $HTTP_GET_VARS["SkinPath"] . "' ;" 
               : null;
$FCKToolbar  = ($HTTP_GET_VARS["Toolbar"]) 
               ? "oFCKeditor.ToolbarSet = '" . $HTTP_GET_VARS["Toolbar"] . "' ;" 
               : null;
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
  <head>
    <title>FCKeditor <?php echo $FCKname; ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta name="robots" content="noindex, nofollow">
    <script type="text/javascript" src="fckeditor.js"></script>
    <script type="text/javascript" language="JavaScript">
    <!--
    function sendEditor()
    {
      // Get the editor instance that we want to interact with.
      var oEditor = FCKeditorAPI.GetInstance('FCKeditor1') ;

      // Get the editor contents in XHTML.
      window.opener.oFCKta.value = oEditor.GetXHTML( true );
      parent.window.close();
    }
    -->
    </script>
  </head>
  <body style="margin:0px; padding:0px;">
    <form method="post" onSubmit="sendEditor();">
      <script type="text/javascript">
      <!--
        var oFCKeditor = new FCKeditor( 'FCKeditor1' ) ;
        oFCKeditor.BasePath  = '' ;
        oFCKeditor.Height  = '98%' ;
        <?php echo $FCKSkinPath ?>
        oFCKeditor.Config['ShowBorders'] = false ;
        oFCKeditor.Config['ToolbarCanCollapse'] = false ;

        <?php echo $FCKToolbar; ?>
        oFCKeditor.Value  = window.opener.oFCKta.value ;
        oFCKeditor.Create() ;
      //-->
      </script>
    </form>
  </body>
</html>