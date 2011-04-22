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

my $lang = get_language();

db_connect($db_name, $db_user, $db_pass);

my $counter = add_counter();

my $sth = db_query('SELECT workgroups.name, id, sum(stat.size), count(hosts.comp_id), prefix FROM workgroups, hosts, stat WHERE workgroups.id=hosts.wkg_id AND hosts.comp_id=stat.comp_id AND (res_type=0 OR res_type IS NULL) GROUP BY wkg_id ORDER BY name ASC');

my @wkgs = ();
my $cnt = 0;
while (my @row = $sth->fetchrow_array) {
	my %params = ();
	$cnt++;
	$params{'cnt'} = $cnt;
	$params{'name'} = $row[0];
	my $id = $row[1];
	$params{'href'} = "showwkg.pl?wkg=$id";
	$params{'share'} = str_size $row[2];
	$params{'count'} = $row[3];
	$params{'prefix'} = $row[4];
	push (@wkgs, \%params);
}

$sth = db_query('SELECT MAX(last_ping) AS last_ping, MAX(last_scan) AS last_scan FROM hosts WHERE last_scan < NOW()');
my @row = $sth->fetchrow_array;

my $path = get_script_path();
my $template = HTML::Template->new('filename' => "$path/shownet_$lang.tmpl");
$template->param('last_scan' => str_time($row[1]));
$template->param('last_ping' => str_time($row[0]));

$template->param('wkg_loop' => \@wkgs);
$template->param('wkg_count' => $cnt);

print $template->output;

