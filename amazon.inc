<?php
  define('AMAZON_SEARCH_AMAZON_SERVICE', 'AWSECommerceService');
  define('AMAZON_SEARCH_AMAZON_SUBSCRIPTION_ID', '0JEKXTWNECEXBJGY7RR2');
  define('AMAZON_SEARCH_AMAZON_ASSOCIATE_TAG', '');
  define('AMAZON_SEARCH_AMAZON_ITEM_SEARCH','ItemSearch');
  define('AMAZON_SEARCH_AMAZON_ITEM_LOOKUP','ItemLookup');

  global $_amazon_search_amazon_shop_URI;
  global $_amazon_search_amazon_shop_index;

  /**
   * $_amazon_search_amazon_shop_URI
   * holds the REST addresses for all the Amazon branches.
   *
   * It does not because they use different BrowseNodes values, product identifiers and affiliate programs
   * Making them work with these other Amazon branches is a matter of harvesting data for
   * $_amazon_search_browse_nodes.
   *
   */
  $_amazon_search_amazon_shop_URI =
    array('US'=>'http://webservices.amazon.com/onca/xml?Service='.AMAZON_SEARCH_AMAZON_SERVICE,
          'UK'=>'http://webservices.amazon.co.uk/onca/xml?Service='.AMAZON_SEARCH_AMAZON_SERVICE,
          'DE'=>'http://webservices.amazon.de/onca/xml?Service='.AMAZON_SEARCH_AMAZON_SERVICE,
          'JP'=>'http://webservices.amazon.co.jp/onca/xml?Service='.AMAZON_SEARCH_AMAZON_SERVICE,
          'FR'=>'http://webservices.amazon.fr/onca/xml?Service='.AMAZON_SEARCH_AMAZON_SERVICE,
          'CA'=>'http://webservices.amazon.ca/onca/xml?Service='.AMAZON_SEARCH_AMAZON_SERVICE
          );

  /**
   * the index used for $_amazon_search_amazon_shop_URI.
   *
   */
  $_amazon_search_amazon_shop_index = 'US';

  /**
   * Build a REST URI from array of parameters.
   *
   * @param $Params
   *    array keys are request parameter names, values are URL encoded.
   *    For the list of available request parameters, see
   *    http://www.amazon.com/gp/aws/sdk/main.html?s=AWSEcommerceService&v=4-0&p=ApiReference/ItemSearchOperation .
   * @return
   *    a string containing the REST query URI.
   */
  function amazon_search_RESTURI($Params) {

    global $_amazon_search_amazon_shop_URI;
    global $_amazon_search_amazon_shop_index;
    /** if the referring page passes variable named 'AmazonShop' it will
      * be copied and used as the index for $_amazon_search_amazon_shop_URI
    */
    $searchParams = array();
    foreach ($Params as $key=>$value) {
      if ($key == 'AmazonShop') {
        $_amazon_search_amazon_shop_index = $value;
      } else {
        $searchParams[$key] = $value;
      }
    }

    $AmazonBookSearchParams = array(
    'SubscriptionId'=>AMAZON_SEARCH_AMAZON_SUBSCRIPTION_ID,
    'AssociateTag'=>AMAZON_SEARCH_AMAZON_ASSOCIATE_TAG, 'Operation'=>AMAZON_SEARCH_AMAZON_ITEM_SEARCH
    );

    /** if the referring page passes in variables with the above names
      * the above variables will be overwritten
    */
    $AmazonBookSearcher = array_merge($AmazonBookSearchParams, $searchParams);

    $AmazonQueryString = '';
    foreach ($AmazonBookSearcher as $key => $value) {
      $AmazonQueryString .= '&'."$key=".urlencode($value);
    }

    return $_amazon_search_amazon_shop_URI[$_amazon_search_amazon_shop_index] . $AmazonQueryString;
  }

  /**
   * Everything from here to _amazon_items_array_from_DOM() is used by _amazon_items_array_from_DOM() to turn
   * the returned XML into a nice linear array that will be written as "<b>$key:</b> $value"
   *
   * TO DO: handle <Creator role="xxx">
   */
  function _amazon_element_content($Element) {

    $Content_Text = $Element->first_child();
    return $Content_Text->node_value();
  }

  function _amazon_image_to_array($Amazon_Item, $ImageSize) {
    foreach ($Amazon_Item->child_nodes() as $ImageData) {
      $Amazon_Image[$ImageSize.$ImageData->tagname()] = _amazon_element_content($ImageData);
    }
    return $Amazon_Image;
  }

  function _amazon_pricing_to_array($Amazon_Item, $Array_Prefix = '') {
    $Amazon_Pricing = array();
    foreach ($Amazon_Item->child_nodes() as $PricingData) {
      $Amazon_Pricing[$Array_Prefix.$PricingData->tagname()] = _amazon_element_content($PricingData);
    }
    return $Amazon_Pricing;
  }

  function _amazon_item_attributes_to_array($Amazon_Item) {
    $Amazon_ItemAttributes = array();
    foreach ($Amazon_Item->child_nodes() as $ItemAttributesData) {
      switch ($ItemAttributesData->tagname()) {
        case 'Author': {
          $Amazon_ItemAttributes['Author'][] = _amazon_element_content($ItemAttributesData);
          break;
        }
        case 'Artist': {
          $Amazon_ItemAttributes['Artist'][] = _amazon_element_content($ItemAttributesData);
          break;
        }
        case 'Actor': {
          $Amazon_ItemAttributes['Actor'][] = _amazon_element_content($ItemAttributesData);
          break;
        }
        case 'Composer': {
          $Amazon_ItemAttributes['Composer'][] = _amazon_element_content($ItemAttributesData);
          break;
        }
        case 'Conductor': {
          $Amazon_ItemAttributes['Conductor'] = _amazon_element_content($ItemAttributesData);
          break;
        }
        case 'Orchestra': {
          $Amazon_ItemAttributes['Orchestra'] = _amazon_element_content($ItemAttributesData);
          break;
        }
        case 'Binding':
        case 'Title':
        case 'DetailPageURL':{
          $Amazon_ItemAttributes[$ItemAttributesData->tagname()] = _amazon_element_content($ItemAttributesData);
          break;
        }
        case 'ListPrice': {
          $Amazon_ItemAttributes = array_merge($Amazon_ItemAttributes, _amazon_pricing_to_array($ItemAttributesData, 'list'));
          break;
        }
        // default: throw it away
      }
    }
    return $Amazon_ItemAttributes;
  }

  function _amazon_offer_listing_to_array($Amazon_Item) {
    $Amazon_Offer = array();
    foreach ($Amazon_Item->get_elements_by_tagname('Offer') as $OfferData) {
      foreach ($OfferData->get_elements_by_tagname('OfferListing') as $_Offer) {
        foreach ($_Offer->child_nodes() as $_OfferDetail) {
          switch ($_OfferDetail->tagname()) {
            case 'Price': {
              $Amazon_Offer = array_merge($Amazon_Offer, _amazon_pricing_to_array($_OfferDetail, ''));
              break;
            }
            default: {
              $Amazon_Offer[$_OfferDetail->tagname()] = _amazon_element_content($_OfferDetail);
              break;
            }
          }
        }
      }
    }
    return $Amazon_Offer;
  }

  /**
   * creates an array of amazon_item objects.
   *
   * Creates an array of amazon_item objects from an xmldom object.
   *
   * @param $amazonDOM
   *    a valid xmldom object.
   * @return
   *    an array of amazon_item objects.
  */
  function _amazon_items_array_from_DOM($amazonDOM) {
    $AmazonItemList = array();
    $_Amazon_items = $amazonDOM->get_elements_by_tagname('Item');
    foreach ($_Amazon_items as $Item) {
      $_CurrentItem = array();
      foreach ($Item->child_nodes() as $_Item) {
        switch ($_Item->tagname()) {
          // one case for every structured tag that must be saved
          case 'SmallImage': {
            $_CurrentItem = array_merge($_CurrentItem, _amazon_image_to_array($_Item, 'SmallImage'));
            break;
          }
          case 'MediumImage': {
            $_CurrentItem = array_merge($_CurrentItem, _amazon_image_to_array($_Item, 'MediumImage'));
            break;
          }
          case 'LargeImage': {
            $_CurrentItem = array_merge($_CurrentItem, _amazon_image_to_array($_Item, 'LargeImage'));
            break;
          }
          case 'ItemAttributes': {
            $_CurrentItem = array_merge($_CurrentItem, _amazon_item_attributes_to_array($_Item));
            break;
          }
          case 'DetailPageURL':
          case 'ASIN': {
            $_CurrentItem[$_Item->tagname()] = _amazon_element_content($_Item);
            break;
          }
          case 'Offers': {
            $_CurrentItem = array_merge($_CurrentItem, _amazon_offer_listing_to_array($_Item));
          }
          default: {
            break;
          }
        }
      }
      $AmazonItemList[] = array2object($_CurrentItem);
    }
    return $AmazonItemList;
  }


  // parses the record keeping stuff returned by the query
  function _amazon_operation_request($_AmazonDOM) {
    $OperationRequest = array();
    $OperationRequest_data = $_AmazonDOM->get_elements_by_tagname('OperationRequest');
    $OperationRequest_items = $OperationRequest_data[0];
    // get the items needed to repeat the query, as originally passed
    foreach ($OperationRequest_items->child_nodes() as $Item) {
      switch ($Item->tagname()) {
        // one case for every structured tag that must be saved
        case 'Arguments': {
          foreach ($Item->child_nodes() as $_Argument) {
            $_argument_name = $_Argument->get_attribute('Name');
            if (($_argument_name == 'MerchantId') or ($_argument_name == 'Service')
                or ($_argument_name == 'AssociateTag') or ($_argument_name == 'SubscriptionId')
                or ($_argument_name == 'Operation') or ($_argument_name == 'ResponseGroup')) {
              $OperationRequest['Arguments']['def'][$_argument_name] = $_Argument->get_attribute('Value');
            } elseif (($_argument_name == 'ItemPage')) {
              $OperationRequest['stats']['$_argument_name'] = $_Argument->get_attribute('Value');
            } else {
              $OperationRequest['Arguments']['var'][$_argument_name] = $_Argument->get_attribute('Value');
            }
          }
          if (!$OperationRequest['stats']['ItemPage']) {
            $OperationRequest['stats']['ItemPage'] = 1;
          }
          break;
        }
        case 'RequestId': {
          break;
        }
        case 'HTTPHeaders': {
          break;
        }
        case 'RequestProcessingTime': {
          break;
        }
        default: {
          break;
        }
      }
    }
    // get the number of "ItemPage"s available so we know how many we can browse
    $Items_data = $_AmazonDOM->get_elements_by_tagname('Items');
    foreach ($Items_data as $Item) {
      $_CurrentItem = array();
      foreach ($Item->child_nodes() as $_Item) {
        switch ($_Item->tagname()) {
          case 'TotalResults': {
            $OperationRequest['stats']['TotalResults'] = _amazon_element_content($_Item);
            break;
          }
          case 'TotalPages': {
            $OperationRequest['stats']['TotalPages'] = _amazon_element_content($_Item);
            break;
          }
          default: {
            break;
          }
        }
      }
    }
    return $OperationRequest;
  }

  /**
   * The actual REST request is done here.
   *
   *  @param $Amazon_Params
   *      an array of parameters to pass to Amazon as part of the REST query.
   *      the element name and value are passed as the parameter name and value
   *      respectively
   *  @result
   *      a string containing the query result as XML.
  */
  function amazon_get_XML($Amazon_Params) {
    $AmazonXML = '';
    $url = amazon_search_RESTURI($Amazon_Params);
    // Obviously this will be switched for an equivalent Drupal function
    // Nope. drupal_http_request() strips the doctype tag at the start of the XML, making it useless
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    ob_start();
    curl_exec($ch);
    $AmazonXML = ob_get_clean();
    curl_close($ch);

    return $AmazonXML;
  }

  /**
   * Returns a XML object with the result of the query.
   *
   *  @param $Amazon_Params
   *      an array of parameters to pass to Amazon as part of the REST query.
   *      the element name and value are passed as the parameter name and value
   *      respectively
   *  @return
   *      a domxml object
  */
  function amazon_get_DOM($Amazon_Params) {
    return domxml_open_mem(amazon_get_XML($Amazon_Params), DOMXML_LOAD_PARSING);
  }

  /**
    * Returns an array of amazon_item objects.
   *
   *  @param $Amazon_Params
   *      an array of parameters to pass to Amazon as part of the REST query.
   *      the element name and value are passed as the parameter name and value
   *      respectively
   *  @return
   *      an array of amazon_item objects.
  */
  function amazon_get_items($Amazon_Params) {
    $_amazon_search_amazonDOM = amazon_get_DOM($Amazon_Params);
    return _amazon_items_array_from_DOM($_amazon_search_amazonDOM);
  }

?>