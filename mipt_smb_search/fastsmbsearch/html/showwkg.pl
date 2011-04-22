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
use CGI qw(:standard);
use HTML::Template;
use Common;
use SMBConf;

print_header();

db_connect($db_name, $db_user, $db_pass);

my $counter = add_counter();
my $lang = get_language();

my $wkg = param('wkg');
if (not is_number($wkg)) {
	write_error("<b>$wkg</b> is not a valid numeric value.");
}

my $sth = db_query('SELECT name FROM workgroups WHERE id=?', $wkg);
my @row = $sth->fetchrow_array();
my $wkg_name = $row[0];

$sth = db_query('SELECT name,hosts.comp_id,status,ip,proto,size,share_count FROM hosts LEFT JOIN stat ON (hosts.comp_id=stat.comp_id) WHERE ((res_type=0) OR (res_type is null)) AND (wkg_id=?) ORDER BY FIND_IN_SET(status, "offline,error,unchecked,online") DESC, name ASC', $wkg);
if ($sth->rows == 0) {
	write_error("Nothing found -- possible syntax error");
};

my $total = 0;
my $cnt = 0;
my @hosts = ();
while (@row = $sth->fetchrow_array) {
	my %params = ();
	$cnt++;
	$params{'cnt'} = $cnt;
	$params{'name'} = $row[0];
	my $id = $row[1];
	$params{'href'} = "showdir.pl?host=$id";
	my $status = $row[2];
	$params{'status'} = status_name($status);
	$params{'stat_style'} = $status;
	$params{'ip'} = $row[3];
	$params{'proto'} = proto_name($row[4]);
	my $size = $row[5] || 0;
	$params{'size'} = str_size($size);
	$params{'share_count'} = $row[6];
	$total += $size;
	push (@hosts, \%params);
}

my $path = get_script_path();
my $template = HTML::Template->new('filename' => "$path/showwkg_$lang.tmpl");
$template->param('total_size' => str_size($total));

$template->param('wkg_name' => $wkg_name);
$template->param('host_loop' => \@hosts);
$template->param('host_count' => $cnt);

print $template->output;

