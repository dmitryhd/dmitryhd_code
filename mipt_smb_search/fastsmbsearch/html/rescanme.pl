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
print_page_header('Fast SMB Search: Scan request form');

db_connect($db_name, $db_user, $db_pass);

my $ip = get_client_ip();
my $go = param('go') || '';

my $sth = db_query('SELECT name, status, last_scan FROM hosts WHERE ip=?', $ip);
if ($sth->rows == 0) {
	print qq(
<div>Sorry, I cannot determine your host. May be you are viewing this site through proxy?<br />
If you have File Sharing service and your host is not automatically detected by networks scanner,
please send e-mail to <a href=\"mailto:$admin_email\">$admin_email</a><br />
Please, specify <b>your IP, hostname, workgroup and current time</b> in bug report<br />
</div>
</body></html>);
	exit;
} 

my @row = $sth->fetchrow_array();
my $name = $row[0];
my $status = color_status($row[1]);
my $last_scan_time = str_time($row[2]);

print qq(<h1>Scan request</h1>
<div>You are looking this page from computer <b>$name</b> ($status), <b>$ip</b>.<br />
If you feel this wrong, please send e-mail to <a href=\"mailto:$admin_email\">$admin_email</a></div>
<p>Your host hast been scanned at <b>$last_scan_time</b>.</p>
);

if (length($go) > 0) {
	db_query('UPDATE hosts SET status = "unchecked" WHERE ip = ?', $ip);
	print "<p class='largetext'>Your request has been accepted!</p>\n";
	print "<p>Please wait 15-20 minutes for next scanner run.</p>\n";
	print "</body></html>";
	exit;
}
print <<FOOTER;
<form action="rescanme.pl" method="post">
<p>Press the following button and your host will be included in the next scan <br /><br />
<input type="submit" name="go" value="Rescan me!" />
</p>
</form>
</body></html>
FOOTER
