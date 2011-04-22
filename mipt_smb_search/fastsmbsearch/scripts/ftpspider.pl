#!/usr/bin/perl

# Simple FTP spider for Fast SMB Search
# (sorry, now it is bugged, but support more effective scanning method)
# by Shurik <shurikz@users.sourceforge.net>

use POSIX;
use Time::HiRes qw(gettimeofday);
use Net::FTP;
use Text::Iconv;
use Getopt::Long qw(:config no_ignore_case bundling);
use strict;

my $icharset="";
my $ocharset="";
my ($user,$password)=('anonymous','-ftp-scanner@poiskovik');
my $addr;
my $raise=0;

my $allow_dtp=0;
my $allow_nodtp=1;
my $passive=0;

GetOptions (
  "h"=>\&usage,
  "P=i"=>\$passive,
  "d=i"=>\$allow_dtp,
  "D=i"=>\$allow_nodtp,
  "u=s"=>\$user,
  "p=s"=>\$password,
  "I=s"=>\$icharset,
  "O=s"=>\$ocharset,
  "<>"=>sub { $raise=1 if defined $addr; ($addr)=@_; } ) or die "Error in params!\n";

die "To few/many params" if $raise or !defined $addr;

my $converter;

if($icharset && $ocharset) {
  $converter=new Text::Iconv($icharset,$ocharset);
}
  
sub ftplist;

my $max_depth=12;
my $prio=18;

my $timestart=gettimeofday();
my $timeint=60;
my $timeiter=1;

my $nodtp=0;
$nodtp=1 if $allow_nodtp; # $nodtp will be updated to 0 if unavailable

# Force DTP if noDTP is off
$allow_dtp=1 unless $allow_nodtp;

setpriority 0,0,$prio;

# Open connection
my $ftp = Net::FTP->new($addr, Debug => 0, Passive => $passive);
die "Cannot open FTP connection to $addr" unless defined $ftp;

# Login
$ftp->login($user,$password) or die "Cannot login to $addr: ".$ftp->message."\n";

# Scan
my $id=1;
ftplist("/",0,$max_depth);
$ftp->quit;
exit;

sub ftpdir { # (void)

  # First try noDTP
  if($nodtp) {
    # Periodically use DTP to prevent timeout disconnect
    if (gettimeofday() > $timestart + $timeint * $timeiter) {
      $timeiter++;
      return $ftp->dir;
    }

    # Try to get filelist via STAT
    my $resp=$ftp->quot("STAT .");
    my $code=$ftp->code;
    my $msg=$ftp->message;

    if($resp==2) {
      # Check output to verify if it is RFC-compliant
      my @tmp=split "\n",$msg;
      unless($tmp[0]=~m#^status\s+for\s+user#i||     # serv-u
             $tmp[0]=~m#^\s*server\s+status\s+for#i) # bulletproof
      {
        # If we're here, noDTP seems to be working correctly
        map chomp,@tmp;
	@tmp=grep { !/STAT/i } @tmp;
        @tmp=grep { !/Status/i } @tmp;
	@tmp=grep { !/(2\d\d)?\s*End/i } @tmp;
	map { s/^\s*// } @tmp;
	return @tmp;
      }
    }
    # If STAT returns error or STAT output is not like to right filelist,
    # disable noDTP
    $nodtp=0;
  }

  # Use DTP if allowed
  return $ftp->dir if $allow_dtp;

  # Finnaly, don't return anything
  return undef;
}

sub ftplist # ($dir,$dir_id,$depth)
{
  my ($cwd,$ud,$depth)=@_;

  # Go into directory
  if($ftp->cwd($cwd)<1) {
    # If CWD failed, dir will be listed as empty
    return;
  }

  my @dir=ftpdir; # Directory listing
  my @sub=qw();   # Subdirectories for recurse calls

  if((!$nodtp && !$allow_dtp)||($depth==$max_depth && @dir==0)) {
    # If noDTP failed and DTP is not allowed, return dummy filelist
    print "1\t0\tThis server seems to be non-RFC959 so it unsupported\t0\t0\n";
    return;
  }

  my ($type,$size,$name);
  foreach(@dir) {
    # Parse line
    if(/^\d/) {
      # Microsoft FTP Server style
      m#^[^\s]+\s+[^\s]+\s+([^\s]+)\s+([^\s].*)$#;
      ($type,$size,$name)=('-',$1,$2);
      ($type,$size)=('d',0) unless($size=~m#^\d+$#);
    }
    else {
      # Canonical FTP style
      m#^(.).........\s+\d+\s[^\s]+\s+[^\s]+\s+(\d+)\s.{12}\s(.+)$#;
      ($type,$size,$name)=($1,$2,$3);
    }

    # Only regular files and directories must be listed!
    if(!defined $name) { warn "BUG: parse error for directory listing line, is that FTP server software untested? [$_]"; next; }
    next if $name eq ".";
    next if $name eq "..";
    next unless ($type eq '-') or ($type eq 'd');

    # Fix $type and $size
    ($size,$type)=(0,1) if $type eq 'd';
    $type=0 if $type eq '-';

    # Iconv name
    my $oname=$name;
    if($converter) {
      $oname=$converter->convert($name);
      unless($oname) {
        warn "I cannot iconv follow string: $name in $cwd on $addr\n";
        $oname="EILSEQ-$name";
      }
    } 

    # Print resulting line
    print "$id\t$ud\t$oname\t$size\t$type\n";

    # Put directory to @sub for recurse
    push @sub,[$name,$id] if $type==1;
    $id++;
  }

  # Recurse into subdirs
  if($depth>0) {
    foreach(@sub) {
      ftplist($cwd.${$_}[0]."/",${$_}[1],$depth-1);
    }
  }
}

sub usage {
  print <<EOF
Usage: $0 [options] {IP|host}
Options:
  -h            This help
  -d{0|1}       Allow DTP (default=0)
  -D{0|1}       Allow NoDTP (default=1)
  -P{0|1}       Use passive mode (default=0)
  -u<user>      Use custom user to login
  -p<password>  Use custom password to login
  -I<charset>   Use custom input charset 
  -O<charset>   Use custom output charset
EOF
;
  exit(0);
}
