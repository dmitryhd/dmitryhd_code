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
use CGI qw(:standard escape);
use Time::HiRes qw(gettimeofday);
use Text::Iconv;
use HTML::Template;
use Common;
use SMBConf;

print_header();

my $lang = get_language();
my $enc = param('enc') || 'koi8-r';
my $what = param('what') || '';
$what =~ s/^\s+//;
$what =~ s/\s+$//;

if (($enc eq 'utf-8') || ($enc eq 'cp1251')) {
	my $from_utf = Text::Iconv->new($enc, "koi8-r");
	$what = $from_utf->convert($what) || '';
}

my $esc_what = escapeHTML($what);
my $restype = param('restype') || 'all';
my $invrestype = param('invrestype') || 'no';
my $wkg = param('wkg') || 0;
my $proto = param('proto');
my $req_status = param('status') || 'all';
my @unitmult = qw( 1 1024 1048576 1073741824 ); 
my $unit = param('unit') || 0;
my $minsize = (param('minsize') || 0) * $unitmult[$unit];
my $maxsize = (param('maxsize') || 0) * $unitmult[$unit];
my $show = param('show') || 'all';
my $start = param('start') || 0;
my $count = param('count') || $default_count_value;
my $pathmatch = param('pathmatch') || 0;
my $useregexp = param('useregexp') || 'no';
my $sort = param('sort') || 'hostprio';
my $sort2 = param('sort2') || 'filepath';
my $sort3 = param('sort3') || 'nosort';
my $use_base = param('use_base') || 0;
my $base_query_host = param('base_query_host');
my $base_query_file = param('base_query_file');
my $inv_sort = param('inv_sort') || 0;
my $inv_sort2 = param('inv_sort2') || 0;
my $inv_sort3 = param('inv_sort3') || 0;
my $unstrict = param('unstrict') || 'no';
if (($count < 1) or ($count > 1000)) {
	$count = $default_count_value;
}
my $files_per_page = $count;

db_connect($db_name, $db_user, $db_pass);
get_browser_type();
my $counter = add_counter();
guess_client();

my $time_start = gettimeofday();

my $sth = db_query('SELECT id, prefix FROM workgroups');
my %wkg_prefix = ();
while (my @row = $sth->fetchrow_array()) {
    $wkg_prefix{$row[0]} = $row[1];
}

my $res_id = res_type_id($restype);
if ($res_id == -1) {
	write_error("<b>$restype</b> is invalid resource type");
}
if ($res_id == 11) {
    $res_id = 0;
    $show = 'dirs';
}

if ((length($what)<2) and ($res_id ==0) and ($use_base == 0))
{
	write_error("<b>$esc_what</b> is too short. 2 letters or more required (otherwise you can specify file type).\n");
}
if (not is_number($wkg)) {
	write_error("<b>$wkg</b> is not a valid numeric value.\n");
}
if (not is_number($minsize)) {
	write_error("<b>$minsize</b> is not a valid numeric value.\n");
}
if (not is_number($maxsize)) {
	write_error("<b>$maxsize</b> is not a valid numeric value.\n");
}
if (not is_number($start)) {
	write_error("<b>$start</b> is not a valid numeric value.\n");
}
my %avail_sorts = ( 'hostprio' => 1, 'hoststatus' => 1, 'hostwkg' => 1, 
	'hostname' => 1, 'filedir' => 1, 'filepath' => 1, 'filename' => 1,
	'filesize' => 1, 'nosort' => 1);
if (not $avail_sorts{$sort}) {
	write_error("<b>$sort</b> is unknown sort order.\n");
}
if (not $avail_sorts{$sort2}) {
	write_error("<b>$sort2</b> is unknown sort order.\n");
}
if (not $avail_sorts{$sort3}) {
	write_error("<b>$sort3</b> is unknown sort order.\n");
}

if( not defined( $base_query_file ) ) {
	$use_base = 0;
}
if ($use_base and ($base_query_file eq "")) {
	$use_base = 0;
}

while ($what =~ m#^(\s+|\\)#) {
	$what =~ s#^(\s+|\\)##g;
}
while ($what =~ m#([^ \\])\\#) {
	$what =~ s#([^ \\])\\#$1 \\#g;
}
while ($what =~ m#\\([^ \\])#) {
	$what =~ s#\\([^ \\])#\\ $1#g;
}

my $search_query;
my $empty = 1;
if ($useregexp eq "yes") {
	$search_query = "REGEXP \"$what\"";
	$empty = 0;
} else {
	if (length($what) >= 2) {
		my @words = ();
		my $rest = $what;
		while (1) {
			$rest =~ s/^ +//g;
			last if $rest eq '';
			if ($rest =~ s/^([^ "]+)//) {
				push @words, $1;
				next;
			}
			if ($rest =~ s/^"([^"]+)"//) {
				push @words, $1;
				next;
			}
			write_error("Unrecognized search string: <b>".escapeHTML($rest)."</b> in <b>$esc_what</b>");
		}
		if ($#words == 0) {
			$pathmatch = 0;
		}
		if ($pathmatch) {
			for my $word (@words) {
				if (($word eq '\\') || ($word eq '\\\\')) {
					write_error("Cannot use metasymbols <b>\\</b> and <b>\\\\</b> in path match mode");
				}
			}
			$search_query = ($unstrict eq 'yes') ? "ERRSTR(\"$words[$0]\",1)" : "SUBSTR \"$words[0]\"";
			for (my $i = 1; $i <= $#words; $i++) {
			        my $strtest = ($unstrict eq 'yes') ? "ERRSTR(\"$words[$i]\",1)" : "SUBSTR \"$words[$i]\"";
				$search_query = "$search_query, $strtest";
			}
			$search_query = "PATHMATCH ($search_query)";
			$empty = 0;
		} else {
			$search_query = ($unstrict eq 'yes') ? "ERRSTR(\"$words[0]\",1)" : "SUBSTR \"$words[0]\"";
			$empty = 0;
			for (my $i = 1; $i <= $#words; $i++) {
				if ($words[$i] eq '\\') {
					$search_query = "PARENT ($search_query)";
					$empty = 1;
				} else {
					if ($words[$i] eq '\\\\') {
						$search_query = "ANYPARENT ($search_query)";
						$empty = 1;
					} else {
					        my $strtest = ($unstrict eq 'yes') ? "ERRSTR(\"$words[$i]\",1)" : "SUBSTR \"$words[$i]\"";
						$search_query = "($search_query) AND ($strtest)";
						$empty = 0;
					}
				}
			}
		}
	} else {
		$search_query = "TRUE";
	}
}
if ($res_id > 0) {
	my $res_if = "RES_TYPE $res_id";
	if ($invrestype eq 'yes') {
		$res_if = "NOT ($res_if)";
	}
	$search_query = "($res_if) AND ($search_query)";
	$empty = 0;
}
if ($minsize > 0) {
	$search_query = "($search_query) AND (MINSIZE $minsize)";
}
if ($maxsize > 0) {
	$search_query = "($search_query) AND (MAXSIZE $maxsize)";
}
if ($show eq "files" ) {
	$search_query = "(ISDIR 0) AND ($search_query)";
}
if ($show eq "dirs" ) {
	$search_query = "(ISDIR 1) AND ($search_query)";
}
my $end = $start + $files_per_page;
my $host_if = "ALL";
if ($wkg > 0) {
	$host_if = "WKG $wkg";
}
if ($proto =~ m/^\d+$/) {
	$host_if = $host_if." AND PROTO $proto";
}
if ($req_status eq "online") {
	$host_if = $host_if." AND ONLINE";
}
if ($use_base) {
	if ($host_if ne "ALL") {
		$host_if = "($base_query_host) AND ($host_if)";
	}
	$search_query = "($search_query) AND ($base_query_file)";
}
my $sort_string;
if ($sort ne "nosort") {
	$sort_string = "SORTORDER ".uc($sort);
	$sort_string .= " REVERSE" if ($inv_sort);
	if ($sort2 ne "nosort") {
		$sort_string .= ", ".uc($sort2);
		$sort_string .= " REVERSE" if ($inv_sort2);
		if ($sort3 ne "nosort") {
			$sort_string .= ", ".uc($sort3);
			$sort_string .= " REVERSE" if ($inv_sort3);
		}
	}
}
my $req_string = "SEARCH $start $end $host_if $search_query $sort_string";
if ($empty)
{
	write_error("Too weak query. A very large number of results expected. Try more strong restricion.\n");
}

my $script_path = get_script_path();
my $template = HTML::Template->new('filename' => "$script_path/fsearch_$lang.tmpl");

$template->param('what' => $esc_what);
$template->param('files_per_page' => $files_per_page);
my @res_type_options = template_res_type_options($restype);
$template->param('res_type' => \@res_type_options);
$template->param('invrestype' => $invrestype);
$template->param('show' => $show);
$template->param('req_status' => $req_status);
$template->param('pathmatch' => $pathmatch);
$template->param('sort' => $sort);
$template->param('sort2' => $sort2);
$template->param('sort3' => $sort3);
$template->param('inv_sort' => $inv_sort);
$template->param('inv_sort2' => $inv_sort2);
$template->param('inv_sort3' => $inv_sort3);
$template->param('unstrict' => $unstrict);
$template->param('base_query_host' => escapeHTML($host_if));
$template->param('base_query_file' => escapeHTML($search_query));

fsearch_connect($fsearch_socket);

my $reply = fsearch_request($req_string);
if ($reply eq "ERROR") {
	write_error("Invalid search request");
}
if ($reply eq "FULLSCANREQUIRED") {
    write_error(<<__ERR_FULL_SCAN);
Cannot found any index for you query.</p>
<p>This usually means that all words you are searching for are used very often (e.g. <b>of</b>, <b>the</b>, <b>to</b>).
A very large number of results would be found for this query if it will be executed. Try to use more specific words.
This error can be also occured when you have entered popular file extension, for example <b>mp3</b> or <b>html</b>.
Such words occur in millions of files, it is absolutely meaningless to search for them.
</p>
<p>
__ERR_FULL_SCAN
}
if ($reply =~ m/^QUERY_ID ([0-9]+) HINT .+$/) {
	my $query_id = $1;
	my $hint_level = $2;
} else {
	write_error('Invalid response from fsearch server');
}
$reply = fsearch_get_line();
if ($reply !~ m/^COUNT ([0-9]+) ([0-9]+) ([0-9]+(\.[0-9]*)?)/) {
	write_error('Invalid response from fsearch server');
}
my $matched = $1;
my $returned = $2;
my $search_time = $3;
$end = $matched if ($end > $matched);

my $time_query = gettimeofday() - $time_start;

if (not $client_host_name) {
	$client_host_name = get_client_ip();
}

$template->param('page_counter' => $counter);
$template->param('matches' => $matched);
$template->param('not_all' => ($matched > $files_per_page));
$template->param('start' => ($start + 1));
$template->param('end' => $end);

my $sum = 0;
my $i = $start;
my @files = ();
for (my $j = 0; $j < $returned; $j++)
{
	my %params = ();
	my $line = fsearch_get_line();
	my ($id, $parent_id, $name, $size, $is_dir,
		$res_type, $path, $wkg_id, $host_id, $proto, $host_name, $host_ip, $status) = split("\t",$line);
	$i++;
	$params{'cnt'} = $i;
	if ($is_dir == 0) {
		$params{'dir'} = '&nbsp;';
	} else {
		$params{'dir'} = '<b>&lt;dir&gt;</b>';
	}
	$params{'prefix'} = $wkg_prefix{$wkg_id};
	$params{'size'} = str_size($size);
	$sum += $size;
	$params{'status'} = status_name($status);
	$params{'stat_style'} = $status;
	$params{'path_url'} = "showdir.pl?host=$host_id&amp;where=$parent_id";
	$params{'separator'} = separator($proto);
	my $disp_path = make_display_host_path($proto, $host_name);
	if ($path ne '') {
		$disp_path .= separator($proto);
		$disp_path .= make_display_rel_path($proto, $path);
	}
	$params{'disp_path'} = escapeHTML($disp_path);
	$params{'name'} = escapeHTML($name);
	my $file_url;
	if ($is_dir > 0) {
		$file_url = "showdir.pl?host=$host_id&amp;where=$id";
	} else {
		my $host_path = make_root_path($proto)."/$host_ip/";
		my $rel_path = make_rel_path($proto, $path);
		$file_url = escapeHTML($host_path.$rel_path.$name);
	}
	$params{'file_url'} = $file_url;
	push (@files, \%params);
}
fsearch_close();
$template->param('file_loop' => \@files);

my $time_end = gettimeofday();
my $time = $time_end - $time_start;
my $date = localtime();

$template->param('total_size' => str_size($sum));
$template->param('proc_time' => sprintf("%5.2f",$time));
$template->param('search_time' => $search_time);
$template->param('gen_date' => $date);

my $pages = ceil($matched / $files_per_page);
my $pages_per_page = 10;
$template->param('has_more_pages' => ($pages > 1));
if ($pages > 1) { 
	my $page = floor($start / $files_per_page);
	$template->param('cur_page' => $page + 1);
	$template->param('num_pages' => $pages);
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
	my $url = "fastsearch.pl";
	$url .= "?go=Fast+search";
	$url .= "&what=".escape($what);
	if ($minsize) {
		$url .= "&minsize=$minsize";
	}
	if ($maxsize) {
		$url .= "&maxsize=$maxsize";
	}
	if ($pathmatch) {
		$url .= "&pathmatch=1";
	}
	if ($sort ne 'nosort') {
		$url .= "&sort=$sort";
		$url .= "&inv_sort=1" if ($inv_sort);
		if ($sort2 ne 'nosort') {
			$url .= "&sort2=$sort2";
			$url .= "&inv_sort2=1" if ($inv_sort2);
		}
		if ($sort3 ne 'nosort') {
			$url .= "&sort3=$sort3";
			$url .= "&inv_sort3=1" if ($inv_sort3);
		}
	}
	$url .= "&restype=$restype" if ($restype ne 'all');
	$url .= "&invrestype=$invrestype" if ($invrestype ne 'no');
	$url .= "&show=$show" if ($show ne 'all');
	$url .= "&status=$req_status" if ($req_status ne 'all');
	$url .= "&wkg=$wkg" if ($wkg > 0);
	$url .= "&proto=$proto" if ($proto =~m/^\d+$/);
	if ($use_base == 1) {
		$url .= "&use_base=1&base_query_host=".escape($base_query_host);
		$url .= "&base_query_file=".escape($base_query_file);
	}
	if ($files_per_page ne $default_count_value) {
		$url .= "&count=$files_per_page";
	}
	my @page_loop = ();
	for ($i = $page_min; $i < $page_max; $i++) {
		my %params = ();
		$start = $i * $files_per_page;
		$params{'num'} = $i+1;
		$params{'url'} = escapeHTML("$url&start=$start");
		$params{'is_current'} = ($i == $page);
		push (@page_loop, \%params);
	}
	$template->param('page_loop' => \@page_loop);
}
print $template->output;
