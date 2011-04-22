-- MySQL dump 9.11
--
-- Host: localhost    Database: smbsearch
-- ------------------------------------------------------
-- Server version	4.0.24_Debian-5-log

--
-- Table structure for table `counter`
--

CREATE TABLE `counter` (
  `page` varchar(128) NOT NULL default '',
  `count` int(11) NOT NULL default '0',
  PRIMARY KEY  (`page`)
) TYPE=MyISAM;

--
-- Table structure for table `hosts`
--

CREATE TABLE `hosts` (
  `comp_id` int(11) NOT NULL auto_increment,
  `wkg_id` int(11) NOT NULL default '0',
  `ip` tinytext NOT NULL,
  `name` tinytext NOT NULL,
  `proto` int(4) NOT NULL default '0',
  `last_ping` timestamp(14) NOT NULL,
  `last_scan` timestamp(14) NOT NULL default '00000000000000',
  `status` varchar(10) NOT NULL default '',
  `last_online` timestamp(14) NOT NULL default '00000000000000',
  `last_offline` timestamp(14) NOT NULL default '00000000000000',
  `share_count` int(11) NOT NULL default '0',
  `charset` tinytext,
  `user` tinytext,
  `password` tinytext,
  PRIMARY KEY  (`comp_id`),
  KEY `ip` (`ip`(15)),
  KEY `name` (`name`(32))
) TYPE=MyISAM;

--
-- Table structure for table `stat`
--

CREATE TABLE `stat` (
  `comp_id` int(11) NOT NULL default '0',
  `res_type` int(11) NOT NULL default '0',
  `size` bigint(20) NOT NULL default '0',
  KEY `comp_id` (`comp_id`)
) TYPE=MyISAM;

--
-- Table structure for table `workgroups`
--

CREATE TABLE `workgroups` (
  `id` int(11) NOT NULL default '0',
  `name` tinytext NOT NULL,
  `prefix` char(3) default NULL,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;

