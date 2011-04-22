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
use Common;
use SMBConf;

print_header();
print_page_header('Advanced Search');

print q{<h1>Fast SMB Search</h1>
<h2>Advanced search</h2>};

get_browser_type();

db_connect($db_name, $db_user, $db_pass);
my $counter = add_counter();

my $what = param('what') || '';
my $restype = param('restype') || 'all';
my $invrestype = param('invrestype') || 'no';
my $wkg = param('wkg') || 0;
my $proto = param('proto');
my $status = param('status') || 'all';
my $minsize = param('minsize') || 0;
my $maxsize = param('maxsize') || 0;
my $unit = param('unit') || 0;
my $show = param('show') || 'all';
my $start = param('start') || 0;
my $count = param('count') || $default_count_value;
my $pathmatch = param('pathmatch') || 0;
my $useregexp = param('useregexp');
my $sort = param('sort') || 'hostprio';
my $sort2 = param('sort2') || 'filepath';
my $sort3 = param('sort3') || 'nosort';
my $use_base = param('use_base') || 0;
my $unstrict = param('unstrict') || 'no';
my $base_query_host = param('base_query_host');
my $base_query_file = param('base_query_file');
my $inv_sort = param('inv_sort');
my $inv_sort2 = param('inv_sort2');
my $inv_sort3 = param('inv_sort3');
if (($count < 1) or ($count > 1000)) {
	$count = $default_count_value;
}

print q(<form action="fastsearch.pl" method="get">
<table cellpadding="5">
<tr>
<td colspan="2">);
print "<input type=\"text\" name=\"what\" size=\"40\" value=\"";
print escapeHTML($what);
print '" />';
print q(&nbsp;
</td>
<td>
<input type="submit" name="go" value="Search" />
</td></tr>
<tr>
<td>);
print '<input type="checkbox" name="use_base" value="1"';
if ($use_base) {
  print " checked=\"checked\"";
}
print " />within results\n";
print '</td><td>';
print '<input type="checkbox" name="pathmatch" value="1"';
if ($pathmatch) {
  print " checked=\"checked\"";
}
print " />allow path traverse\n";
if (defined($base_query_host)) {
  print "<input type=\"hidden\" name=\"base_query_host\" value=\"".escapeHTML($base_query_host)."\" />\n";
}
if (defined($base_query_file)) {
  print "<input type=\"hidden\" name=\"base_query_file\" value=\"".escapeHTML($base_query_file)."\" />\n";
}
print "</td></tr>\n<tr>\n";
print '<tr><td><input type="checkbox" name="unstrict" value="yes"';
if ($unstrict eq 'yes') {
    print ' checked="checked"';
}
print' /> allow mistakes';
print "</td><td>&nbsp;</td></tr>\n";
print "<td>Search only in &nbsp;\n";
write_res_type_options($restype);
print '</td><td>&nbsp;<input type="checkbox" name="invrestype" value="yes"';
if ($invrestype eq 'yes') {
  print " checked=\"checked\"";
}
print "/> all except selected";
print q(
</td></tr>
</table>
<p>
Search in subnets: 
<select name="wkg">);
my $sth=db_query("SELECT id,name,prefix FROM workgroups");
my %options = ('0' => 'All');
while(my($wkg_id,$name,$prefix)=$sth->fetchrow_array) {
  $options{$wkg_id}="[$prefix] $name";
}
#my %options = ('0' => 'All', '1' => 'MSU', '3' => 'HACKERS', '4' => 'V', '5' => 'GZ-V' );
write_options( \%options, $wkg );
print "</select>&nbsp;&nbsp;in &nbsp;";
print "<select name=\"status\">";
my @arr_options = ('all', 'online');
write_simple_options(\@arr_options, $status);
print "</select>&nbsp;&nbsp;&nbsp;";
print "<select name=\"proto\">";
%options = ('-1' => 'all', '0' => 'smb', '1' => 'ftp');
write_options( \%options, $proto );
print q(
</select>
&nbsp; hosts</p>
<p>Size: from <input type="text" name="minsize" size="16" /> to <input type="text" name="maxsize" size="16" /> in <select name="unit">
);
%options = ('0' => 'bytes', '1' => 'kilobytes', '2' => 'megabytes', '3' => 'gigabytes');
write_options( \%options, $unit );
print '</select></p>';
print '<p>Show &nbsp; ';
print '<select name="show">';
%options = ( 'all' => 'Both files and dirs',
		  'files' => 'Only files',
		  'dirs' => 'Only directories');
write_options(\%options, $show);
print "</select></p>\n";
print '<table cellpadding="5" border="0">';
print "<tr>\n<td>\n";
print "Primary sort</td><td>\n";
print "<select name=\"sort\">";
%options = ( 'hostprio' => 'Host', 'hoststatus' => 'Host status',
		  'hostwkg' => 'Workgroup', 'hostname' => 'Hostname',
		  'filedir' => 'Dir/file', 'filepath' => 'Path', 'filename' => 'Name',
		  'filesize' => 'Size', 'nosort' => 'No sort' );
write_options(\%options, $sort);
print "</select></td>\n";
print '<td><input type="checkbox" name="inv_sort" value="1"';
if ($inv_sort) {
  print ' checked="checked"';
}
print " />inverse</td></tr>\n";
print "<tr><td>Secondary sort</td>\n";
print '<td><select name="sort2">';
write_options(\%options, $sort2);
print "</select></td>\n";
print '<td><input type="checkbox" name="inv_sort2" value="1"';
if ($inv_sort2) {
  print ' checked="checked"';
}
print " />inverse</td></tr>\n";
print "<tr><td>Third sort</td>\n";
print '<td><select name="sort3">';
write_options(\%options, $sort3);
print "</select></td>\n";
print '<td><input type="checkbox" name="inv_sort3" value="1"';
if ($inv_sort3) {
  print ' checked=\"checked\"';
}
print " />inverse</td></tr>\n";
print "</table>\n";
print "<p>Show &nbsp;&nbsp;<input type=\"text\" name=\"count\" size=\"4\" value=\"$count\" />&nbsp; files per page\n";
print qq(
</p>
</form>
<p>Look <a href="faq.html">here</a> for details.</p>
</body></html>);
