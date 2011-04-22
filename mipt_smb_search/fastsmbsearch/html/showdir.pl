#!/usr/bin/perl -w
# This file is part of Fast SMB Search
#
# Copyright 2001, 02, 03, 04   Alexander Vodomerov
#
# Fast SMB Search is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# Fast SMB Search is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

use strict;
use warnings;
use POSIX;
use CGI qw(:standard);
use Common;
use SMBConf;

print_header();

my $host_id = param('host') || '';
my $where = param('where') || 0;
my $start = param('start') || 0;
my $count = param('count') || 0;
if (not is_number($host_id)) {
	write_error("<b>$host_id</b> (host) is not a valid numeric value.\n");
}
if (not is_number($where)) {
	write_error("<b>$where</b> (where) is not a valid numeric value.\n");
} 
if (not is_number($start)) {
	write_error("<b>$start</b> (start) is not a valid numeric value.\n");
} 
if (not is_number($count)) {
	write_error("<b>$count</b> (count) is not a valid numeric value.\n");
} 

db_connect($db_name, $db_user, $db_pass);

get_browser_type();

my $counter = add_counter();

fsearch_connect($fsearch_socket);
my $server_reply;
# get info about host
my $wkg_id;
my $host_name;
my $proto;
my $host_ip;
my $host_status;

my $info = fsearch_request("HOSTINFO $host_id");
if ($info eq "ERROR") {
	fsearch_close();
	write_error (<<__ERR_TEXT_NO_HOST);
Unknown host. No host with such ID in database.
__ERR_TEXT_NO_HOST
}
my $root_id;
my $max_id;
($host_id, $proto, $host_name, $host_ip, $host_status, $wkg_id, $root_id, $max_id) = split("\t", $info);

my $reply = fsearch_request("SEARCH 0 1 HOST_ID $host_id FILE_ID $where");
if ($reply !~ m/^QUERY_ID ([0-9]+)/) {
	write_error("Invalid response from fsearch server");
}
$reply = fsearch_get_line();
if ($reply !~ m/^COUNT ([0-9]+) ([0-9]+) ([0-9]+(\.[0-9]*)?)/) {
	write_error("Invalid response from fsearch server");    
}
my $found = $1;
if ($found != 1) {
	fsearch_close();
	write_error (<<__ERR_TEXT_NO_FILE);
Unknown file or directory. Common reasons for that are:</p>
<ul>
<li>this host has never been indexed (access denied, too many connections, invalid firewall settings etc)</li>
<li>you followed an outdated link of type <i>scriptname.pl?id=&lt;number&gt;</i>, and &lt;number&gt; no more refers to valid directory id</li>
</ul>
<p>See detailed description of this problem in <a href="faq.html#not-indexing">FAQ</a>.
__ERR_TEXT_NO_FILE
}
$reply = fsearch_get_line();
my ($id, $parent_id, $name, $size, $is_dir, $res_type, $path, $status);
($id, $parent_id, $name, $size, $is_dir, $res_type,
	$path, $wkg_id, $host_id, $proto, $host_name, $host_ip, $status) = split("\t", $reply);
my $is_share;
if ($path) {
	 $path = $path."/".$name;
	 $is_share = 0;
} else {
	 $path = $name;
	 $is_share = 1;
}
my $host_path = make_root_path($proto)."/$host_ip/";
my $rel_path = make_rel_path($proto, $path);
my $full_path = escapeHTML($host_path.$rel_path);

my $files_per_page = $default_count_value;
if (($count > 1) and ($count < 1000)) {
	$files_per_page = $count;
}
my $end = $start + $files_per_page;

$server_reply = fsearch_request("SEARCH $start $end SHORT HOST_ID $host_id PARENT_ID $where SORTORDER FILEDIR, FILENAME");
if ($server_reply eq "ERROR") {
	write_error("Non-existent host or directory");
}
if ($server_reply =~ m/^QUERY_ID ([0-9]+)/) {
	my $query_id = $1;
} else {
	write_error("Invalid response from fsearch server");
}
$server_reply = fsearch_get_line();
my $matched;
my $returned;
my $search_time;
if ($server_reply =~ m/^COUNT ([0-9]+) ([0-9]+) ([0-9]+(\.[0-9]*)?)/) {
	$matched = $1;
	$returned = $2;
	$search_time = $3;
	$end = $matched if ($end > $matched);
}
my $title;
if ($where == 0) {
	$title = "Shares at $host_name";
} else {
	$title = make_display_full_path($proto, $host_name, $path);
}

print_page_header($title);

if ($where == 0) {
	if ($proto == 0) {
		print "<div>Shares at computer <a href=\"$host_path\">$host_name</a>:</div><hr />\n";
	}
	if ($proto == 1) {
		print "<div>Root directory of FTP server <a href=\"$host_path\">ftp://".lc($host_name)."</a>:</div><hr />\n";
	}
} else {
	print "<div>Contents of directory ";
	print "<a href=\"$host_path\">".escapeHTML(make_display_host_path($proto, $host_name))."</a>";
	print separator($proto);
	print "<a href=\"$full_path\">".escapeHTML(make_display_rel_path($proto, $path))."</a>:</div><hr />\n";
}

if ($where == 0) {
	print "<div><a href=\"showwkg.pl?wkg=$wkg_id\">Go to upper level</a></div>\n";
} else {
	if ($is_share) {
		print "<div><a href=\"showdir.pl?host=$host_id&amp;where=0\">Go to parent directory</a></div>\n";
	} else {
		print "<div><a href=\"showdir.pl?host=$host_id&amp;where=$parent_id\">Go to parent directory</a></div>\n";
	} 
}
if ($matched >= $files_per_page)
{
	print "<p>$matched files found. Files ".($start+1)." - $end are showed.</p>\n";
};

print "<table cellpadding=\"6\" cellspacing=\"0\">\n";
my $sum = 0;
my $i	= $start;
for (my $j = 0; $j < $returned; $j++)
{
	my $line = fsearch_get_line();
	($id, $parent_id, $name, $size, $is_dir, $res_type) = split("\t",$line);
	next if ($id == 0);
	$i++;
	print "<tr><td>$i.</td><td>";
	if ($is_dir == 0) {
		print "&nbsp;";
	} else {
		print "<b>&lt;dir&gt;</b>";
	}
	print "</td><td align=\"right\"><b>";
	print str_size($size);
	$sum += $size;
	print "</b></td><td>";
	$name = escapeHTML($name);
	if ($is_dir > 0) {
		print "<a href=\"showdir.pl?host=$host_id&amp;where=$id\">$name</a>";
	} else {
		print "<a href=\"$full_path$name\">$name</a>";
	}
	print "</td></tr>\n";
}
fsearch_close();
print "</table>\n";
print "<hr />\n";
my $pages = ceil($matched / $files_per_page);
my $pages_per_page = 10;
if ($pages > 1) { 
	print '<p class="centeralign"><span class="plusonefont">';
	my $page = floor($start / $files_per_page);
	print "Page <b>".($page+1)."</b> of <b>$pages</b>: ";
	my $page_max = $page + $pages_per_page/2;
	if ($page_max > $pages) {
		$page_max = $pages;
	}
	my $page_min = $page_max - $pages_per_page;
	$page_min = 0 if ($page_min < 0);
	$page_max = $page_min + $pages_per_page;
	if ($page_max > $pages) {
		$page_max = $pages;
	}
	my $url = "showdir.pl?host=$host_id&amp;where=$where";
	if ($files_per_page != $default_count_value) {
		$url .= "&amp;count=$files_per_page";
	}
	for ($i = $page_min; $i < $page_max; $i++) {
		$start = $i * $files_per_page;
		if ($i == $page) {
			print "<b>".($i+1)."</b>&nbsp;";
		} else {
			print "<a href=\"$url&amp;start=$start\">".($i+1)."</a>&nbsp;";
		}
	}
	print "</span></p>\n";
}
print "<p>Total: <b>$matched</b> file(s), total size of ".($returned == $matched ? "" : "shown $returned ")."files (with subdirs): <b>".str_size($sum)."</b>.<br /><br />";
print "Computer <b>$host_name</b> ($host_ip) is ";
print color_status($status);
my $sth = db_query("SELECT last_ping, last_scan, last_online, last_offline FROM hosts WHERE comp_id = ?", $host_id);
my @row = $sth->fetchrow_array();
print " (last checked at <b>".str_time($row[0])."</b>).<br />\n";
if ($status ne "offline") {
	print "Went up at <b>".str_time($row[3])."</b>.";
} else {
	print "Went down at <b>".str_time($row[2])."</b>.";
}
print " Last scanned at <b>".str_time($row[1])."</b>.</p>\n";
print "</body></html>";
