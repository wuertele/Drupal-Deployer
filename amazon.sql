# Table: 'amazonitem'
# 
CREATE TABLE `amazonitem` (
  `ASIN` varchar(10) NOT NULL default '',
  `DetailPageURL` varchar(255) default '',
  `SmallImageURL` varchar(255) default '',
  `SmallImageHeight` int(10) unsigned default '0',
  `SmallImageWidth` int(10) unsigned default '0',
  `MediumImageURL` varchar(255) default '',
  `MediumImageHeight` int(10) unsigned default '0',
  `MediumImageWidth` int(10) unsigned default '0',
  `LargeImageURL` varchar(255) default '',
  `LargeImageHeight` int(10) unsigned default '0',
  `LargeImageWidth` int(10) unsigned default '0',
  `Author` varchar(255) NOT NULL default '',
  `Binding` varchar(100) default '',
  `listAmount` int(10) unsigned default '0',
  `listCurrencyCode` char(3) default '',
  `listFormattedPrice` varchar(10) default '',
  `Title` varchar(100) NOT NULL default '',
  `Amount` int(10) unsigned default '0',
  `CurrencyCode` char(3) default '',
  `FormattedPrice` varchar(10) default '',
  `Availability` varchar(50) default '',
  `PriceDate` datetime default '0000-00-00 00:00:00',
  PRIMARY KEY  (`ASIN`),
  UNIQUE KEY `ASIN` (`ASIN`),
  KEY `Title` (`Title`),
  KEY `Author` (`Author`,`Title`)
) TYPE=MyISAM; 

# Table: 'amazonnode'
# 
CREATE TABLE `amazonnode` (
  `nid` int(10) unsigned NOT NULL default '0',
  `ntype` varchar(16) NOT NULL default '',
  `ASIN` varchar(10) NOT NULL default ''
) TYPE=MyISAM; 

