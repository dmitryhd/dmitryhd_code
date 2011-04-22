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
use Filesys::SmbClient;
use MIME::Types 'by_suffix';
use Text::Iconv;
use Sys::Hostname;
# Limit memory usage to workaround grave bugs in libsmbclient
use BSD::Resource;

# WARNING! This script requires libsmbclient from samba-3.x !

my $version = "0.1-pre2";
my $logfile = "/var/tmp/smbgate.log";
my $hostname = hostname();

my $workgroup = "MSU";
my $username = "username";
my $password = "";
my $memlimit = 15; # memory limit (in Mb)

my $script = "/smb";

my $max_old_size = 20000000;

my %icon_hash = ( 'image/.*' => 'image2.gif', 'text/.*' => 'text.gif', 'application/msword' => 'text.gif',
			'audio/.*' => 'sound2.gif', 'video/.*' => 'movie.gif', '(exe|com|dll)' => 'binary.gif',
			'.*' => 'unknown.gif' );

my $from_utf = Text::Iconv->new("utf-8", "koi8-r");
my $to_utf = Text::Iconv->new("koi8-r", "utf-8");

# returns icon url for specified mime type
sub icon_by_mime
{
	my $mime = $_[0];
	for my $r (keys %icon_hash) {
		if ($mime =~ m/$r/) {
			return $icon_hash{$r};
		}
	}
}

# set memory limit
my $success = setrlimit(RLIMIT_VMEM, $memlimit * 1024 * 1024, $memlimit * 1024 * 1024);

# get script path
my $page = $ENV{'PATH_INFO'} || '';
# remove head /
$page =~ s#^/##;
# remove trailing /
$page =~ s#/+$##;
my $file = $page;

my $header = qq(<?xml version="1.0" encoding="KOI8-R"?>
<!DOCTYPE html
	PUBLIC "-//W3C//DTD XHTML 1.1//EN"
	 "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en-US">
<head>
	<title>Index of $file</title>
	<link rel="stylesheet" type="text/css" href="/style.css" />
</head>);

if ($file eq '') {
	print "Content-Type: text/html; charset=koi8-r\n";
	print "Status: 400 Bad request\n\n";
	print $header;
	print "<body><p><b>Error</b>: empty URL is not allowed</p></body></html>";
	exit;
}

# try to handle clients supplying filename in UTF-8 encoding (default is KOI8-R)
my $autoconvert = 0;
my $orig = $from_utf->convert($file);
if (defined ($orig) && ($orig ne $file)) {
	$file = $orig;
	$autoconvert = 1;
}

my $utf_file = $to_utf->convert($file);
if (not defined($utf_file)) {
	print "Content-Type: text/html; charset=koi8-r\n";
	print "Status: 400 Bad request\n\n";
	print $header;
	print "<body><p><b>Error</b>: filename contains bad character (in must be valid string in KOI8-R encoding).</p>";
	if ($autoconvert) {
		print "<p>This can be caused by wrong automatic conversion from UTF-8 to KOI8-R. ";
		print "If you think the requested file really exists, report this as bug.</p>";
	}
	print "</body></html>";
	exit;
}

# write connection to log
open LOG, ">>$logfile";
my $remote_addr = $ENV{'REMOTE_ADDR'};
my $remote_port = $ENV{'REMOTE_PORT'};
my $path_info = $ENV{'PATH_INFO'};
my $user_agent = $ENV{'HTTP_USER_AGENT'};
my $curtime = scalar(localtime());
print LOG "$curtime: process $$: client at $remote_addr:$remote_port ($user_agent) requested file $path_info\n";
close LOG;

my $smb_file = $to_utf->convert("smb://$file");

# We need handling of 'Range: bytes=0-' requests as range requests
my $has_range = 0;
my $start = 0;
my $range = $ENV{'HTTP_RANGE'};
if (defined($range)) {
	if ($range =~ m/bytes=(\d+)/) {
		$start = $1;
		$has_range = 1;
	}
}
my ($mediatype, $encoding) = by_suffix($file);

my $smb = new Filesys::SmbClient(username => $username, password=> $password, workgroup => $workgroup, debug => 0);

my $parent = $file;
$parent =~ s#[^/]+$##;

my $filesize = 0;
if ($parent ne '') {
	# don't stat host-level url
	my @info = $smb->stat($smb_file);
	if ($#info == 0) {
		my $err = $!;
		# There are sometimes strange bugs: we cannot open normal file (and error is empty)
		# return retriable error so the client can resume download
		if (not defined($err) or ($err eq '')) {
			print "Content-Type: text/html; charset=koi8-r\n";
			print "Status: 503 Service Unavailable\n\n";
			print $header;
			print "<body>Sorry, internal libsmbclient error :(</body></html>";
			exit;
		}
		print "Content-Type: text/html; charset=koi8-r\n";
		print "Status: 404 Not found\n\n";
		print $header;
		print "<body><p><b>Error</b>: cannot stat file <b>$file</b>: <i>$err</i>.</p>";
		if ($autoconvert) {
			print "<p>This can be caused by wrong automatic conversion from UTF-8 to KOI8-R. ";
			print "If you think the requested file really exists, report this as bug.</p>";
		}
		print "</body></html>";
		exit;
	}
	$filesize = $info[7];
}

my $proto = $ENV{'SERVER_PROTOCOL'};
if (($proto ne 'HTTP/1.1') && ($filesize > $max_old_size)) {
	print "Content-Type: text/html; charset=koi8-r\n";
	print "Status: 400 Bad request\n\n";
	print $header;
	print "<body><p><b>Error</b>: your web browser is not HTTP/1.1-capable. ";
	print "Downloading large file (>$max_old_size) requires partial HTTP requests ";
	print "which works only under HTTP/1.1 protocol.</p></body></html>";
	exit;
}

# file exists - try to give it

# try as directory
my $fd = $smb->opendir($smb_file);
if ($fd > 0) {
	print "Content-Type: text/html; charset=koi8-r\n\n";
	print $header;
	print qq(
<body>
<h1>Index of $file</h1>
<hr />
<table border="0">
<tr><td>
<img src="/icons/back.gif" alt="back" />);
	my $parent_url = "$script/$parent";
	print "</td><td><a href=\"$parent_url\">";
	print "Parent directory</a></td></tr>\n";

	my @dir = $smb->readdir_struct($fd);
	@dir = sort { $a->[1] cmp $b->[1] } @dir;
	for my $f (@dir) {
		my $name = $from_utf->convert($f->[1]);
		# skip names that can't be converted to koi8-r
		next if (not defined ($name));
		next if ($name eq ".");
		next if ($name eq "..");
		print "<tr><td>";
		my $type = $f->[0];
		my $icon = "unknown.gif";
		if ($type == SMBC_FILE) {
			($mediatype, $encoding) = by_suffix($name);
			$icon = icon_by_mime($mediatype);
		}
		if ($type == SMBC_DIR) {
			$icon = "folder.gif";
		}
		print "<img src=\"/icons/$icon\" alt=\"$mediatype\" />";
		my $url = escapeHTML("$script/$file/$name");
		print "</td><td><a href=\"$url\">".escapeHTML($name)."</a></td></tr>\n";
	}
	print "</table>\n";
	print "<hr />\n<address>SMBGate version $version at $hostname</address>\n";
	print "</body></html>\n";
	exit;
}

# try as a file
$fd = $smb->open($smb_file, '0444');
if ($fd <= 0) {
	my $err = $!;
	# There are sometimes strange bugs: we cannot open normal file (and error is empty)
	# return retriable error so the client can resume download
	if (not defined($err) or ($err eq '')) {
		print "Content-Type: text/html; charset=koi8-r\n";
		print "Status: 503 Service Unavailable\n\n";
		print $header;
		print "<body>Sorry, internal libsmbclient error :(</body></html>";
		exit;
	}
	print "Content-Type: text/html; charset=koi8-r\n";
	print "Status: 404 Forbidden\n\n";
	print $header;
	print "<body><p><b>Error</b>: cannot open file <b>$file</b>: <i>$err</i>.</p></body></html>";
	exit;
}

my $status;
if ($has_range) {
	$status = "206 Partial content";
} else {
	$status = "200 OK";
}
print "Content-Type: $mediatype\n";
print "Status: $status\n";
my $cont_len = $filesize - $start;
print "Content-Length: $cont_len\n";
if ($has_range) {
	my $end = $filesize-1;
	print "Content-Range: bytes $start-$end/$filesize\n";
	$smb->seek($fd, $start);
}
print "\n";
while (defined(my $l = $smb->read($fd, 4096))) {
	print $l;
}

$smb->close($fd);

