-- MySQL dump 8.22
--
-- Host: localhost    Database: dirtbike_drupal
---------------------------------------------------------
-- Server version	3.23.56

--
-- Table structure for table 'amazon_taxonomies'
--

CREATE TABLE amazon_taxonomies (
  short_name varchar(200) NOT NULL default '',
  enabled tinyint(4) default NULL,
  alt varchar(255) default NULL,
  PRIMARY KEY  (short_name),
  KEY enabled (enabled)
) TYPE=MyISAM;

