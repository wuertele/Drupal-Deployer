<?php
  global $_amazon_search_browse_fields;
  global $_amazon_search_browse_nodes;
  /**
    * $_amazon_search_browse_fields holds the field to search for each SearchIndex.
    * Valid combinations can be found at
    *
    * http://www.amazon.com/gp/aws/sdk/main.html?s=AWSEcommerceService&v=4-0&p=ApiReference/UsSearchIndexMatrixArticle
    *
  */
  $_amazon_search_browse_fields =
  array(
    'Books' => array('Author'=>'Author','Keywords'=>'Keywords','Title'=>'Title'),
  );

  /**
    * $_amazon_search_browse_nodes holds an array of Browse Node values for the SearchIndex used as the key.
    * It lets you limit a search by topic. Valid combinations can be found at
    *
    * http://www.amazon.com/gp/aws/sdk/main.html?s=AWSEcommerceService&v=4-0&p=ApiReference/BrowseNodeValuesArticle
    *
  */
  $_amazon_search_browse_nodes =
  array(
    'US'=> array(
      1=>'Arts & Photography',
      2=>'Biographies & Memoirs',
      3=>'Business & Investing',
      4=>'Children\'s Books',
      4366=>'Comics & Graphic Novels',
      5=>'Computers & Internet',
      6=>'Cooking, Food & Wine',
      13643=>'Engineering',
      86=>'Entertainment',
      301889=>'Gay & Lesbian',
      10=>'Health, Mind & Body',
      9=>'History',
      48=>'Home & Garden',
      49=>'Horror',
      10777=>'Law',
      17=>'Literature & Fiction',
      13996=>'Medicine',
      18=>'Mystery & Thrillers',
      53=>'Nonfiction',
      290060=>'Outdoors & Nature',
      20=>'Parenting & Families',
      173507=>'Professional & Technical',
      21=>'Reference',
      22=>'Religion & Spirituality',
      23=>'Romance',
      75=>'Science',
      25=>'Science Fiction & Fantasy',
      26=>'Sports',
      28=>'Teens',
      27=>'Travel'
    ),
    'UK' => array(
      507848 => "Address Books, Journals & More",
      91 => "Art, Architecture & Photography",
      51 => "Audio Cassettes",
      267859 => "Audio CDs",
      67 => "Biography",
      68 => "Business, Finance & Law",
      69 => "Children's Books",
      274081 => "Comics & Graphic Novels",
      71 => "Computers & Internet",
      72 => "Crime, Thrillers & Mystery",
      637262 => "e-Books",
      62 => "Fiction",
      66 => "Food & Drink",
      275835 => "Gay & Lesbian",
      74 => "Health, Family & Lifestyle",
      65 => "History",
      64 => "Home & Garden",
      63 => "Horror",
      89 => "Humour",
      61 => "Mind, Body & Spirit",
      73 => "Music, Stage & Screen",
      275389 => "Poetry, Drama & Criticism",
      59 => "Reference & Languages",
      58 => "Religion & Spirituality",
      88 => "Romance",
      57 => "Science & Nature",
      56 => "Science Fiction & Fantasy",
      564334 => "Scientific, Technical & Medical",
      60 => "Society, Politics & Philosophy",
      55 => "Sports, Hobbies & Games",
      83 => "Travel & Holiday",
      52 => "Young Adult"
    ),
    'DE' => array(
      4185461 => "Antiquarische Bücher",
      117 => "Belletristik",
      403434 => "Business & Karriere",
      120 => "Börse & Geld",
      124 => "Computer & Internet",
      11063821 => "Erotik",
      288100 => "Fachbücher",
      548400 => "Film, Kultur & Comics",
      280652 => "Kinder- & Jugendbücher",
      122 => "Kochen & Lifestyle",
      287480 => "Krimis & Thriller",
      403432 => "Lernen & Nachschlagen",
      1199902 => "Musiknoten",
      121 => "Naturwissenschaften & Technik",
      143 => "Politik, Biografien & Geschichte",
      536302 => "Ratgeber",
      298002 => "Reise & Sport",
      188795 => "Religion & Esoterik",
      142 => "Science Fiction, Fantasy & Horror"
    ),
    'JP' => array(
      466284 => "Literature & Fiction",
      571582 => "Philosophy & Religion",
      571584 => "Society & Politics",
      492152 => "Nonfiction",
      466286 => "Travel & Geography",
      466282 => "Business & Career",
      492054 => "Investment & Financial Management",
      466298 => "Computer & Internet",
      466294 => "Art & Photography",
      466296 => "Entertainment",
      466292 => "Sports & Hobby",
      466304 => "Home & Family",
      466302 => "Foreign Language Reference",
      466306 => "Children's Books",
      466280 => "Cartoons & Anime",
      466300 => "New Releases",
      746102 => "Sheet Music"
    ),
    'FR' => array(
      468256 => "Actions éditoriales",
      692186 => "Auteurs de A à Z",
      655842 => "Formats",
      463892 => "Idées cadeaux",
      585472 => "Meilleures ventes",
      4140171 => "Occasions et livres rares",
      518572 => "Par éditeur",
      564382 => "Partners",
      301130 => "Thèmes"
    ),
    'CA' => array(
      933484 => "Arts & Photography",
      934986 => "Biographies & Memoirs",
      935522 => "Business & Investing",
      4142731 => "Calendars",
      935948 => "Children's Books",
      939082 => "Computers & Internet",
      940804 => "Cooking, Food & Wine",
      941112 => "Engineering",
      941378 => "Entertainment",
      942318 => "Gay & Lesbian",
      942402 => "Health, Mind & Body",
      927728 => "History",
      943356 => "Home & Garden",
      943958 => "Horror",
      927730 => "Law",
      927790 => "Literature & Fiction",
      948300 => "Medicine",
      948808 => "Mystery & Thrillers",
      927734 => "Nonfiction",
      950152 => "Outdoors & Nature",
      950640 => "Parenting & Families",
      950756 => "Professional & Technical",
      952366 => "Reference",
      953420 => "Religion & Spirituality",
      955190 => "Romance",
      956280 => "Science",
      957368 => "Science Fiction & Fantasy",
      959466 => "Sports",
      959978 => "Teens",
      960696 => "Travel"
    )
  );
?>