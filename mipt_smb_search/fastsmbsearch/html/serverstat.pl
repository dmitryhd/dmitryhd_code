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
print_page_header('Fast SMB Search: Server statistics');

db_connect($db_name, $db_user, $db_pass);

my $counter = add_counter();

print q{<h2>FSearch server statistics</h2>};
fsearch_connect($fsearch_socket);
my $reply = fsearch_request("SERVERSTAT");
while ($reply ne "END") {
	print "<div>$reply</div>\n";
	$reply = fsearch_get_line();
}
fsearch_close();
print "</body></html>";
