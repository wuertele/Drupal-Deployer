<?php
/* This class does not handle shopping basket details */
class AmazonProduct {
    var $url;
    var $Asin;
    var $ProductName;
    var $Catalog;
    var $Authors = array();
    var $Artists = array();
    var $ErrorMsg;
    var $Description;
    var $ReleaseDate;
    var $Manufacturer;
    var $ImageUrlSmall;
    var $ImageUrlMedium;
    var $ImageUrlLarge;
    var $ListPrice;
    var $OurPrice;
    var $UsedPrice;
    // reviews
    var $ReviewRating = array();
    var $ReviewSummary = array();
    var $ReviewComment = array();
    // similar products
    var $SimilarProducts = array();
    //
    var $Features = array();
    //
    var $Tracks = array();
    var $ByArtist = array();
    function AmazonProduct() {
        // Does nothing for the moment
        return;
    }
    function getSaving() {
        // Returns Amazon saving, if any
        $difference = (float)substr($this->ListPrice, 1) - (float)substr($this->OurPrice, 1);
        if ($difference > 0) {
            $save = sprintf('%.2f', $difference);
        } else {
            $save = false;
        }
        return $save;
    }
    function isEmptyImg() {
      $arImageSize = getImageSize($this->ImageUrlSmall) ;

      if (is_array($arImageSize)) {
        return ($arImageSize[1] == 1);
      } else {
        return 0;
      }
    }
}
?>
