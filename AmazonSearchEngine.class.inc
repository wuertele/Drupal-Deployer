<?php
/* 
   This file is free for anyone to use or modify as they wish. 
   Author: Simon Willison, July 2002
   See http://scripts.incutio.com/
*/

class AmazonSearchEngine {
    var $server = 'xml.amazon.com';
    var $server_xml_url;
    var $search;
    var $dev_token;
    var $associates_id;
    var $type = 'lite';
    var $version;
    var $results;  // Array of results
    var $xml_format;
    var $xml_content;
    var $totalResults = 0;
    var $_parser;   // The XML parser
    var $_currentResult;
    var $_grabtags; // Tags to grab the contents of
    var $_currentTagContents;

    function AmazonSearchEngine($dev_token, $associates_id = 'none', $version = '1.0', $atype = 'lite', $f = 'xml') {
        $this->dev_token = $dev_token;
        $this->associates_id = $associates_id;

        $this->version = $version;
        if ($this->version == '1.0') {
          $this->server_xml_url = 'onca/xml';
        } else if ($this->version == '2.0') {
          $this->server_xml_url = 'onca/xml2';
        }

        $this->type = $atype;
        $this->xml_format = $f;

        $this->results = array();
        $this->_grabtags = array(
            'Asin', 'ProductName', 'Catalog', 'ErrorMsg',
            'Description', 'ReleaseDate', 'Manufacturer', 'ImageUrlSmall',
            'ImageUrlMedium', 'ImageUrlLarge', 'ListPrice', 'OurPrice',
            'UsedPrice', 'Author', 'Artist', 'Rating', 'Summary', 'Comment',
            'Product', 'Feature', 'TotalResults', 'Track', 'ByArtist'
        );
    }

    function doFetch($url) {
      $fp = fsockopen($this->server, 80);
      fputs($fp, "GET " . $url . " HTTP/1.0\n\n");
      $body = false;
      $xml = '';
      while(($line = fgets($fp, 1024))) {
        $body = $body || trim($line) == "";
        if($body) {
          $xml .= $line;
        }
      }
      fclose($fp);
      return trim($xml);
    }

    function doSearch($url) {
        if(file_exists('includes/Snoopy.class.inc')) {
          $s = new Snoopy;
          $s->fetch($url);
          $contents = $s->results;
        } else {
          $contents = $this->doFetch($url);
        }
        $this->xml_content = $contents;
        if ($this->xml_format == 'xml') {
          $this->_parser = xml_parser_create();
          xml_parser_set_option($this->_parser, XML_OPTION_CASE_FOLDING, false);
          xml_set_object($this->_parser, $this);
          xml_set_element_handler($this->_parser, 'tag_open', 'tag_close');
          xml_set_character_data_handler($this->_parser, 'cdata');
          if (!xml_parse($this->_parser, $contents)) {
              die(sprintf('XML error: %s at line %d',
                  xml_error_string(xml_get_error_code($this->_parser)),
                  xml_get_current_line_number($this->_parser)));
          }
          xml_parser_free($this->_parser);
        }
    }

    function getVersionParam() {
      $version_text = ($this->version == '1.0') ? 'v=1.0&' : '';
      return $version_text;
    }

    function getSortParam($rsort = '') {
      $sort_text = ($rsort == '') ? '' : (($this->version == '2.0') ? "&sort=$rsort" : '');
      return sort_text;
    }

    function getBaseAmazonURL() {
      $version_text = $this->getVersionParam();
      $url = "http://{$this->server}/{$this->server_xml_url}?{$version_text}t={$this->associates_id}&dev-t={$this->dev_token}";
      return $url;
    }

    function getFinalAmazonURL($url = '') {
      return "{$url}&f={$this->xml_format}";
    }

    function searchTerm($term, $mode = 'books', $page = 1, $rsort = '') {
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&KeywordSearch=".urlencode($term)."&mode=$mode&type={$this->type}&page=$page{$sort_text}";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    function searchISBN($isbn) {
        $isbns = is_array($isbn) ? strtoupper(implode(",", $isbn)) : strtoupper($isbn);
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&AsinSearch=$isbns";
        $url .= "&type={$this->type}";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    function searchAuthor($author, $page = 1, $rsort = '') {
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&AuthorSearch=".urlencode($author)."&mode=books";
        $url .= "&type={$this->type}&page=$page{$sort_text}";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    function searchRelated($asin, $page = 1) {
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&SimilaritySearch=$asin";
        $url .= "&type={$this->type}&page=$page";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    /* extra functions */

    function browseNode($browsenode, $mode = 'books', $page = 1, $rsort = '') {
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&BrowseNodeSearch=$browsenode&mode=$mode";
        $url .= "&type={$this->type}&page=$page{$sort_text}";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    function listMania($listid, $page = 1, $rsort = '') {
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&ListManiaSearch=$listid";
        $url .= "&type={$this->type}";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    function searchAsin($asin, $page = 1) {
        $asins = is_array($asin)?strtoupper(implode(",", $asin)):strtoupper($asin); 
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&AsinSearch=$asins";
        $url .= "&type={$this->type}";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    function searchUPC($upc, $mode = 'music', $page = 1, $rsort = '') {
        $upcs = is_array($upc)?implode(",", $upc):$upc;
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "&UpcSearch=$upcs&mode=$mode";
        $url .= "&type={$this->type}";
        $this->doSearch($this->getFinalAmazonURL($url));
    }

    function searchWishList($wl, $page = 1, $rsort = '') {
      if ($this->version == '2.0') {
        $sort_text = $this->getSortParam($rsort);
        $url = $this->getBaseAmazonURL();
        $url .= "WishlistSearch=$wl&type={$this->type}";
        $this->doSearch($this->getFinalAmazonURL($url));
      }
    }

    /* end special functions */

    function tag_open($parser, $tag, $attributes)
    { 

        if ($tag == 'Details') {
            // We've hit a new result
            $this->_currentResult = new AmazonProduct;
            $this->_currentResult->url = $attributes['url'];
        }
        if (in_array($tag, $this->_grabtags)) {
            $this->_currentTag = $tag;
        } else {
            $this->_currentTag = '';
        }
    }

    function cdata($parser, $cdata)
    {
        if ($this->_currentTag) {
            $this->_currentTagContents = trim($cdata);
        }
    }

    function tag_close($parser, $tag)
    {
        switch ($tag) {
            case 'Details':
                // We've hit the end of the result
                $this->results[] = $this->_currentResult;
                break;
            case 'Author':
                $this->_currentResult->Authors[] = $this->_currentTagContents;
                break;
            case 'Artist':
                $this->_currentResult->Artists[] = $this->_currentTagContents;
                break;
            case 'Rating':
                $this->_currentResult->ReviewRating[] = $this->_currentTagContents;
                break;
            case 'Summary':
                $this->_currentResult->ReviewSummary[] = $this->_currentTagContents;
                break;
            case 'Comment':
                $this->_currentResult->ReviewComment[] = $this->_currentTagContents;
                break;
            case 'Product':
                $this->_currentResult->SimilarProducts[] = $this->_currentTagContents;
                break;
            case 'Feature':
                $this->_currentResult->Features[] = $this->_currentTagContents;
                break;
            case 'TotalResults':
                $this->totalResults = $this->_currentTagContents;
                break;
            case 'Track':
                $this->_currentResult->Tracks[] =  $this->_currentTagContents;
                break;
            case 'ByArtist':
                $this->_currentResult->ByArtist[] = $this->_currentTagContents;
                break;
            default:
                if (in_array($tag, $this->_grabtags)) {
                    $this->_currentResult->$tag = $this->_currentTagContents;
                }
        }
    }
}
?>
