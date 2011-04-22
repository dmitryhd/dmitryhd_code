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

my $restype = param('restype') || 'all';
my $onlyonline = param('onlyonline') || 'off';

my $res_id = res_type_id($restype);
if ($res_id == -1) {
	write_error("<b>$restype</b> is invalid resource type");
}
if ($res_id == 11) {
	write_error("<b>$restype</b> is invalid resource type");
}

my @res_type_options = template_res_type_options($restype);

my $query = 'SELECT hosts.comp_id AS id, size, hosts.name, proto, status, prefix, proto FROM hosts,stat,workgroups WHERE (hosts.comp_id = stat.comp_id) AND (hosts.wkg_id = workgroups.id) AND (res_type=?) and (size>0)';
if ($onlyonline eq "on") {
	$query .= ' AND (hosts.status = "online")';
}
$query .= ' ORDER BY size DESC';

my $sth = db_query($query, $res_id);
my $sum = 0;
my $host_count = 0;
my @hosts = ();
while (my @row = $sth->fetchrow_array) {
	my %params;
	$host_count++;
	$params{'counter'} = $host_count;
	$params{'bold'} = ($host_count <= 3);
	my $id = $row[0];
	$params{'href'} = "showdir.pl?host=$id";
	my $size = $row[1];
	$params{'size'} = sprintf("%.1f", $size/1048576);
	$params{'name'} = $row[2];
	$params{'status'} = status_name($row[4]);
	$params{'stat_style'} = $row[4];
	$params{'prefix'} = $row[5];
	$params{'proto'} = proto_name($row[6]);
	push(@hosts, \%params);
	$sum += $size;
}
my $date = localtime();

my $path = get_script_path();
my $template = HTML::Template->new('filename' => "$path/stat_$lang.tmpl");

$template->param('host_loop' => \@hosts);
$template->param('total_hosts' => $host_count);
$template->param('total_size' => str_size($sum));
$template->param('only_online' => ($onlyonline eq 'on'));
$template->param('gen_date' => $date);
$template->param('page_counter' => $counter);
$template->param('about_info' => $about_info);
$template->param('res_type' => \@res_type_options);
print $template->output;
