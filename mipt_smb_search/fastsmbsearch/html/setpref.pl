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

my $smb_root = param('smb_root');

my $c = new CGI::Cookie(-name => 'smb_root',
	-value   =>  $smb_root,
	-expires =>  '+1y');

# print HTTP header
print header(-type => $content_type, -charset => 'koi8-r', -cookie => $c);

print <<EOF;
<?xml version="1.0" encoding="KOI8-R"?>
<!DOCTYPE html
	PUBLIC "-//W3C//DTD XHTML 1.1//EN"
	 "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en-US">
<head>
	<title>Fast SMB Search: Preferences</title>
	<meta http-equiv="Content-Type" content="text/html; charset=koi8-r" />
	<link rel="stylesheet" type="text/css" href="/style.css" />
</head>
<body>
<p>smb_root is set to <b>$smb_root</b></p>
EOF
