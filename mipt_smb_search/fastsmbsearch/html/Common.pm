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

package Common;

use strict;
use warnings;
use diagnostics;
use DBI;
use Socket;
use CGI qw(:standard);
use CGI::Cookie;
use Apache2::Connection ();
use vars qw(@ISA @EXPORT);

BEGIN {
	use Exporter;
	our ($VERSION, $ISA, @EXPORT);
	
	$VERSION = '0.3';
	
	@ISA = qw(Exporter);
	@EXPORT = qw(db_connect write_error str_size str_date str_time db_query get_client_ip guess_client write_options write_simple_options add_counter get_browser_type color_status make_root_path make_rel_path make_display_host_path win_path make_display_rel_path separator make_display_full_path fsearch_connect fsearch_request fsearch_get_line fsearch_close is_number print_footer print_page_header get_script_path get_language print_header write_res_type_options template_options template_res_type_options res_type_id status_name proto_name icon_by_mime $about_info $admin_email $default_count_value $content_type $client_host $client_host_ip $client_host_name);

}

our $default_count_value = 100;
our $admin_email = 'alex@sectorb.msk.ru';
our $about_info = "<b>Fast SMB Search</b> is written by Sasha, write to <a href=\"mailto:$admin_email\">$admin_email</a>.";

our $content_type;
our $client_host;
our $client_host_ip;
our $client_host_name;

my $db;

my @defined_languages = ('en', 'ru');
my $lang_default = 'en';
my $lang;

my $header_printed;

my $is_external;
my $is_konqueror;
my $is_local;
my $is_mozilla;
my $is_windows;
my $ftp_user_root;
my $smb_user_root;

my %res_hash = ( 'all' => 0, 'film' => 1, 'music' => 2, 'doc' => 3, 'picture' => 4,
	'unix' => 5, 'clip' => 6, 'cdimage' => 7, 'archive' => 9, 'winexe' => 10, 'dir' => 11 );

my %res_type_options_en = ( 'all' => 'All files', 'film' => 'Films', 'clip' => 'Clips',
		'music' => 'Music', 'doc' => 'Documents', 
		'picture' => 'Pictures', 'unix' => 'Unix files',
		'cdimage' => 'CD Images', 'archive' => 'Archives', 'winexe' => 'Win exe', 'dir' => 'Dirs' );

my %res_type_options_ru = ( 'all' => '÷ÓÅ ÆÁÊÌÙ', 'film' => 'æÉÌØÍÙ', 'clip' => 'ëÌÉÐÙ',
		'music' => 'íÕÚÙËÁ', 'doc' => 'äÏËÕÍÅÎÔÙ', 
		'picture' => 'ëÁÒÔÉÎËÉ', 'unix' => 'æÁÊÌÙ Unix',
		'cdimage' => 'ïÂÒÁÚÙ CD', 'archive' => 'áÒÈÉ×Ù', 'winexe' => 'éÓÐÏÌÎÉÍÙÅ', 'dir' => 'ëÁÔÁÌÏÇÉ' );

my %res_type_options = ( 'en' => \%res_type_options_en, 'ru' => \%res_type_options_ru );

my %status_names_en = ('online' => 'ONLINE', 'offline' => 'OFFLINE', 'error' => 'ERROR', 'unchecked' => 'UNCHECKED');
my %status_names_ru = ('online' => 'äïóôõðåî', 'offline' => '÷ùëìàþåî', 'error' => 'ïûéâëá', 'unchecked' => 'îå ðòï÷åòåî');

my %status_names = ( 'en' => \%status_names_en, 'ru' => \%status_names_ru );

my %proto_names_en = (0 => 'smb', 1 => 'ftp');
my %proto_names_ru = (0 => 'smb', 1 => 'ftp');
my %proto_names = ( 'en' => \%proto_names_en, 'ru' => \%proto_names_ru );

my @size_names_en = ('b', 'Kb', 'Mb', 'Gb', 'Tb');
my @size_names_ru = ('Â', 'ëÂ', 'íÂ', 'çÂ', 'ôÂ');
my %size_names = ( 'en' => \@size_names_en, 'ru' => \@size_names_ru );

my $dtd = ['-//W3C//DTD XHTML 1.0 Strict//EN', "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"];

my $counter;

my %icon_hash = ( 'image/.*' => 'image2.gif', 'text/.*' => 'text.gif', 'application/msword' => 'text.gif',
			'audio/.*' => 'sound2.gif', 'video/.*' => 'movie.gif', '(exe|com|dll)' => 'binary.gif',
			'.*' => 'unknown.gif' );

# open connection to database
sub db_connect
{
	my $db_name = $_[0];
	my $db_user = $_[1];
	my $db_pass = $_[2];
	$db = DBI->connect($db_name, $db_user, $db_pass);
	if (not $db) {
		print "Cannot connect to database $db_name<br>";
		print $DBI::errstr;
		exit;
	}
}

# get language from cookies
sub get_language
{
	return $lang;
}

# sets language
sub set_language
{
	my $new_lang = $_[0];
	my $found = 0;
	for my $l (@defined_languages) {
		if ($new_lang eq $l) {
			$found = 1;
		}
	}
	if ($found) {
		$lang = $new_lang;
	}
}

# set content type
# currently set application/xhtml+xml for Mozilla & Co (because it accepts xml)
# and text/html for other
sub get_content_type
{
	$content_type = 'text/html';
	my $r = Apache2::RequestUtil->request();
	my $accept = $r->headers_in->{'Accept'};
	if (defined ($accept)) {
		if ($accept =~ m#application/xhtml\+xml#) {
			$content_type = 'application/xhtml+xml';
		}
	}
}

# print HTTP header (also deals with language)
sub print_header
{
	get_content_type();
	# get language from cookie
	my %cookies = fetch CGI::Cookie;
	$lang = $lang_default;
	if (exists($cookies{'lang'})) {
		my $c = $cookies{'lang'};
		set_language($c->value() || $lang_default);
	}
	# form parameter has priority over cookie value
	my $l = param('lang');
	if ($l) {
		set_language($l);
	}
	# now save language
	my $c = new CGI::Cookie(-name => 'lang',
		-value   =>  $lang,
		-expires =>  '+1y');
	# print HTTP header
	print header(-type => $content_type, -charset => 'koi8-r', -cookie => $c);
	# Reset flags that HTML header was printed
	$header_printed = 0;
}

# prints page header
sub print_page_header
{
	my $title = $_[0];
	print start_html(-title => $title,
		-encoding => 'koi8-r',
		-declare_xml => 1,
		-style => {-src => '/style.css'},
		-dtd => $dtd);
	$header_printed = 1;
}

# write error message, some info and exit
sub write_error
{
	my $msg = $_[0];
	fsearch_close();

	my $error_title = "Fast SMB Search: Error occurred";
	if (not $header_printed) {
		print_page_header($error_title);
	}
	print "<p><b>Error</b>: $msg</p>\n";
	print "<hr /><div>$about_info\n";
	print "Any bugreports or suggestions are welcome!</div>\n";
	print "</body></html>";
	exit;
}

# return string, representing this size (8 chars)
sub str_size
{
	my $s = $_[0];
	my $names = $size_names{$lang};
	return sprintf("%d %s", $s, $names->[0]) if ($s < 1024);
	my $cnt = 0;
	while ($s > 1024) {
	    last if ($cnt >= 4);
	    $s /= 1024;
	    $cnt++;
	}
	return sprintf("%.1f %s", $s, $names->[$cnt]);
}

# return string, representing date (DD.MM.YYYY format)
sub str_date
{
	my $s = $_[0];
	return substr($s, 6, 2) . "." . substr($s, 4, 2) . "." . substr($s, 0, 4);
}

# return string, representing time (DD.MM.YYYY format)
sub str_time
{
	my $s = $_[0];
	return $s;
}

# Do database query
sub db_query
{
	my $query = $_[0];
	my $sth = $db->prepare($query);
	$sth->execute(@_[1 .. ($#_)]) || print "Cannot execute database query $query\n";
	return $sth;
}

# return IP of client (handle IPv4 in IPv6)
sub get_client_ip
{
	my $r = Apache2::RequestUtil->request();
	my $c = $r->connection;
	my $ip = $c->remote_ip();
	return if (not defined($ip));
	if (substr($ip, 0, 7) eq "::ffff:") {
		$ip = substr($ip, 7);
	}
	return $ip;
}

# try to guess computer of client
sub guess_client
{
	my $ip = get_client_ip();
	
	undef $client_host;
	undef $client_host_name;
	$client_host_ip = $ip;
	my $sth = db_query('SELECT comp_id, name, status FROM hosts WHERE ip=?', $ip);
	if ($sth->rows == 0) {
		return "";
	} else {
		my @row = $sth->fetchrow_array();
		my $status = $row[2];
		if ($status ne "offline") {
			$client_host = $row[0];
			$client_host_name = $row[1];
		}
	}
}

# write options for HTML <select>
sub write_options
{
	my $ref_options = $_[0];
	my %options = %$ref_options;
	my $selected = $_[1];
	
	for my $key (sort keys %options) {
		my $value = $options{$key};
		print "<option value=\"$key\"";
		print ' selected="selected"' if ($key eq $selected);
		print ">$value</option>\n";
	}
}

# write options for HTML <select> where values = comments
sub write_simple_options
{
	my $ref_options = $_[0];
	my @options = @$ref_options;
	my $selected = $_[1];
	foreach my $value (@options) {
		print "<option value=\"$value\"";
		print ' selected="selected"' if ($value eq $selected);
		print ">$value</option>\n";
	}
}

# write options in special HTML::Template format
sub template_options
{
	my $ref_options = $_[0];
	my %options = %$ref_options;
	my $selected = $_[1];

	my @result = ();
	for my $key (sort keys %options) {
		my %params;
		$params{'opt_value'} = $key;
		$params{'opt_name'} = $options{$key};
		$params{'opt_selected'} = $key eq $selected;
		push (@result, \%params);
	}
	return @result;
}

# write HTML code for file type options
sub write_res_type_options
{
	print "<select name=\"restype\">";
	my $res_type = $_[0];
	write_options( $res_type_options{$lang}, $res_type );
	print "</select>\n";
}

# write file type options in HTML template
sub template_res_type_options
{
	my $res_type = $_[0];
	return template_options( $res_type_options{$lang}, $res_type );
}

# get resource type id from string
sub res_type_id
{
	my $restype = $_[0];
	my $res_id = -1;
	$res_id = $res_hash{$restype} if (exists($res_hash{$restype}));
	return $res_id;
}

# page counter
sub add_counter
{
	$counter = 0;
	
	my $r = Apache2::RequestUtil->request();
	my $page = $r->uri();
	my $c = $r->connection;
	my $ip = $c->remote_ip();
	if (not defined ($page)) {
		return 0;
	}
	
	# cut off all ?blah=blah&blah= ...
	$page =~ s/\?.*//g;

	my $sth = db_query('SELECT count FROM counter WHERE page = ?', $page);
	if ($sth->rows > 0) {
		my @r = $sth->fetchrow_array;
		$sth->finish;
		$counter = $r[0];
	} else {
		db_query('INSERT INTO counter VALUES (?,"0")', $page);
		$counter = 0;
	}
	$counter ++;
	db_query('UPDATE counter SET count = ? WHERE page = ?', $counter, $page);
	return $counter;
}

# return directory where script is located
sub get_script_path
{
	my $r = Apache2::RequestUtil->request();
	my $filename = $r->filename();
	$filename =~ s#[^/]+$##g;
	return $filename;
}

# setup global variables upon HTTP_USER_AGENT and REMOTE_ADDR
sub get_browser_type
{
	my $r = Apache2::RequestUtil->request();
	my $user_agent = $r->headers_in->{'User-Agent'};
	if (not defined ($user_agent)) {
		return;
	}
	# Always reset global variables, because it may be left from previous request
	$is_konqueror = 0;
	$is_konqueror = 1 if $user_agent =~ m/Konqueror/;
	$is_mozilla = 0;
	$is_mozilla = 1 if $user_agent =~ m/Gecko/;
	$is_mozilla = 0 if $is_konqueror;
	$is_windows = 0;
	$is_windows = 1 if $user_agent =~ m/MSIE/;
	$is_windows = 1 if $user_agent =~ m/Windows/;
	$is_external = 0;
	my $client_addr = get_client_ip();
	$is_local = 0;
	$is_local = 1 if $client_addr =~ m/^127\.0\./;
	$is_local = 1 if $client_addr =~ m/^172\.16\./;
	$is_local = 1 if $client_addr =~ m/^10\./;
	$is_external = 1 - $is_local;

	$ftp_user_root = 'ftp:/';
	$smb_user_root = 'file:/';
	if ($is_external) {
		$smb_user_root = '/smb';
	} else {
		if ($is_windows) {
			if ($is_mozilla) {
				$smb_user_root = 'file:////';
			}
		} else {
			if ($is_konqueror) {
				$smb_user_root = 'smb:/';
			}
			if ($is_mozilla) {
				$smb_user_root = '/smb';
			}
		}
	}
	my %cookies = fetch CGI::Cookie;
	if (exists($cookies{'ftp_root'})) {
		$smb_user_root = $cookies{'ftp_root'}->value();
	}
	if (exists($cookies{'smb_root'})) {
		$smb_user_root = $cookies{'smb_root'}->value();
	}
}

# returns name of the status in user language
sub status_name
{
	my $status = $_[0];
	return $status_names{$lang}->{$status};
}

# returns name of the proto in user language
sub proto_name
{
	my $proto = $_[0];
	return $proto_names{$lang}->{$proto};
}

# write status as colorized text string
sub color_status
{
	my $s = $_[0];
	return "<span class=\"$s\">".status_name($s)."</span>";
}

# make path for host for target browser (or forward to SMB-HTTP gate)
sub make_root_path
{
	my $proto = $_[0];

	if ($proto == 1) {
		return $ftp_user_root;
	}
	if ($proto == 0) {
		return $smb_user_root;
	}
	print 'Internal error: unsupported protocol!';
}

# make relative path for target browser (or forward to SMB-HTTP gate)
sub make_rel_path
{
	my $proto = $_[0];
	my $path = $_[1];
	if ($path eq "") {
		return "";
	}
	my $new_path = $path."/";
	return $new_path;
}

# make host path for displaying
sub make_display_host_path
{
	my $proto = $_[0];
	my $hostname = $_[1];

	if ($proto == 0) {
		return "\\\\$hostname";
	}
	if ($proto == 1) {
		return "ftp://".lc($hostname);
	}
	print 'Internal error: unsupported protocol!';
}

# convert slashes to backslashes
sub win_path
{
	my $path = $_[0];
	$path =~ s#/#\\#g;
	return $path;
}

# make relative path for displaying
sub make_display_rel_path
{
	my $proto = $_[0];
	my $path = $_[1];

	if ($proto == 0) {
		return win_path($path);
	}
	return $path;
}

# return separator used in protocol
sub separator
{
	my $proto = $_[0];

	if ($proto == 0) {
		return '\\';
	}
	if ($proto == 1) {
		return '/';
	}
	print 'Internal error: unsupported protocol!';
}


# make full path for displaying
sub make_display_full_path
{
	my $proto = $_[0];
	my $hostname = $_[1];
	my $path = $_[2];

	if ($proto == 0) {
		return "\\\\$hostname\\".win_path($path);
	}
	if ($proto == 1) {
		return "ftp://".lc($hostname)."/$path";
	}
	print 'Internal error: unsupported protocol!';
}

# open connection to fsearch server
sub fsearch_connect
{
	my $fsearch_socket = $_[0];
	if (not socket(SOCK, PF_UNIX, SOCK_STREAM, 0)) {
		print 'Fatal error: cannot open create unix socket';
		exit;
	}
	if (not connect(SOCK, sockaddr_un($fsearch_socket))) {
		write_error(<<__ERR_TEXT_NO_FSEARCH);
Cannot open connection to fsearch server</p>
<p>This usually means that fsearch server is under upgrade. Please come back later after 5 minutes.
If you continue to receive this error, please contact author.</p>
<p>See more details in <a href="faq.html#lorien-bug">FAQ</a>.
__ERR_TEXT_NO_FSEARCH
		exit;
	}
}
															
# send request to server and return first line of output
sub fsearch_request
{
	my $req_string = $_[0]."\n";
	send SOCK, $req_string, 0;
	my $result = <SOCK>;
	chomp $result;
	return $result;
}

# return one line from string
# may block (wait)
sub fsearch_get_line
{
	my $result = <SOCK>;
	chomp $result;
	return $result;
}

# close connection to fsearch
sub fsearch_close
{
	close SOCK;
}

# return 1 if argument is a valid number
sub is_number
{
	return 1 if $_[0] =~ m/^[0-9]+$/;
	return 0;
}

# print simple page footer
sub print_footer
{
	my $date = localtime();
	print qq(<p class="smalltext">
The report was generated at $date<br />
This page has been visited <b>$counter</b> times.</p>
<p class="smalltext">$about_info</p>
</body></html>);
}

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

END { }

# Currently no initialization is done

# return 1 as success
1;

