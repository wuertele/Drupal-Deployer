This is actually a package rather that contains two modules (amazon.module and amazon_search.module) that share code to use Amazon's ECS4 REST interface. The code (in amazon.inc) will work with all Amazon.com stores if the data (in amazon_init.inc) is updated to use the codes local to each store.

-----------------------------------------------------------------------

amazon.module creates two new node types:
1 - The node type 'amazon' is for stories about a book sold by Amazon.com. I picture it being used for reviews. One can have as many 'amazon' nodes connected to a single product as you wish.
2 - The node type 'amazon-node' is pure product information. They can be created individually or automatically for each review or related product link entered. This lets your users build your database of products. The administrative interface has an Amazon.com search facility that lets you look up books by keyword or list of ASINs and import them into 'amazon-node' nodes in bulk. Only one 'amazon-node' is created per ASIN.

A field for related products is added to all node types. A comma delimited list of ASINs will produce Amazon links for each ASIN at the bottom of the post.

Because the 'amazon-node' nodes are standard nodes they can have categories attached and taxinomy based pages are automatically available.

The module also has a block that displays a random item linked to the Amazon detail page for the item.

-----------------------------------------------------------------------

amazon_search.module is a drop-in search engine for Amazon.com's book store. The search returns a minimally formatted list of books, including the image Amazon.com provides, the list and Amazon.com prices, a purchase link and each author's name links to an Amazon search for more books by that same author. 

-----------------------------------------------------------------------

The current state of the include file with the Amazon code retrieves these fields (the values are from an actual search):

[ASIN] => 0140280197 
[DetailPageURL] => http://www.amazon.com/exec/obidos/redirect?tag=prometheus606-20%26link_code=xm2%26camp=2025%26creative=165953%26path=http://www.amazon.com/gp/redirect.html%253fASIN=0140280197%2526location=/o/ASIN/0140280197%25253FSubscriptionId=0JEKXTWNECEXBJGY7RR2 
[SmallImageURL] => http://images.amazon.com/images/P/0140280197.01._SCTHUMBZZZ_.jpg [SmallImageHeight] => 60 
[SmallImageWidth] => 42 
[MediumImageURL] => http://images.amazon.com/images/P/0140280197.01._SCMZZZZZZZ_.jpg 
[MediumImageHeight] => 140 
[MediumImageWidth] => 99 
[LargeImageURL] => http://images.amazon.com/images/P/0140280197.01._SCLZZZZZZZ_.jpg 
[LargeImageHeight] => 475 
[LargeImageWidth] => 336 
[Author] => Array ( [0] => Robert Greene ) 
[Binding] => Paperback 
[listAmount] => 1700 
[listCurrencyCode] => USD 
[listFormattedPrice] => $17.00 
[Title] => The 48 Laws of Power 
[Amount] => 1156 
[CurrencyCode] => USD 
[FormattedPrice] => $11.56 
[Availability] => 
[PriceDate] => 2005-03-03 19:28:29

-----------------------------------------------------------------------

To do:
Administration interface needs work
Consider disallowing its use until an Associate ID is entered
