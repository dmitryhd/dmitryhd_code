#!/usr/bin/perl
#
# This script was used for tests in my network. Don't use it if you
# cannot understand what may happens!
#
use strict;
use DBI;
use IO::Socket;

sub tryport;
sub trynmb;
sub trydns;
sub getwkg;
sub ucr;
sub lcr;

exit unless @ARGV;
my ($ip)=@ARGV;
open DEBUG,">> debug.log";
print DEBUG "Call $ip\n";

my $dbh=DBI->connect("DBI:mysql:smbsearch","smb");

my $sth=$dbh->prepare("SELECT name,proto FROM hosts WHERE ip='$ip'");
$sth->execute;
my %res=qw();

my ($name,$proto);
while(($name,$proto)=$sth->fetchrow_array)
{
  $res{$proto}=$name;
}
$sth->finish;

#foreach(keys %res)
#{
#  print DEBUG "IP: $ip Proto: $_ Name: $res{$_}\n";
#}

if(scalar(keys %res))
{
  exit;
}
else
{
  print DEBUG "No IP $ip in db!\n";
}

my $nmbname=undef;
my $dnsname=trydns $ip;
my $smbok=0;
my $wkg=getwkg $ip;

if(tryport $ip,139)
{
  $smbok=1;
  $nmbname=trynmb $ip;
  print DEBUG "IP: $ip Port: 139 (smb) NMB: $nmbname\n";
}

$name=$dnsname if defined $dnsname;
$name=$nmbname if defined $nmbname;
$name=$ip unless defined $name;

my $cmd;
if($smbok)
{
  $cmd="insert into hosts (ip,name,proto,status,wkg_id) values ('$ip','$name',0,'unchecked',$wkg)";
  print DEBUG "DO: $cmd\n";
  $dbh->do($cmd);
}

if(tryport $ip,21)
{
  print DEBUG "IP: $ip Port: 21 (ftp)\n";
  $cmd="insert into hosts (ip,name,proto,status,wkg_id) values ('$ip','$name',1,'unchecked',$wkg)";
  print DEBUG "DO: $cmd\n";
  $dbh->do($cmd);
}

print DEBUG "IP: $ip DNS: $dnsname\n" if defined $dnsname;

close DEBUG;
$dbh->disconnect;
exit 0;

sub tryport
{
  my($ip,$port)=@_;
  my $s=new IO::Socket::INET(PeerAddr=>$ip,PeerPort=>$port,Timeout=>.1);
  return 0 unless $s;
  close $s;
  return 1;
}

sub trynmb
{
  my ($ip)=@_;
  open P,"nmblookup -A $ip 2> /dev/null|";
  my $i=0;
  my $rd=undef;
  while($rd=<P>)
  {
    $i++;
    last if $i==2;
  }
  close P;
  return undef if $i < 2;
  return undef unless $rd=~/^\s+([^\s]+)\s/;
  return $1;
}

sub trydns
{
  my ($ip)=@_;
  my $cmd="dig -x $ip 2> /dev/null|grep PTR|grep -v ';'|awk '{ print \$5 }'";
#  print DEBUG "dns cmd: $cmd\n";
  my $nm=`$cmd`;
  chomp $nm;
#  system "$cmd";
#  print DEBUG "dns got: $nm\n";
  $nm=~s/\..*$//;
  return undef unless $nm=~/[\w\W]/;
  return ucr($nm);
}

sub lcr
{
  my ($a)=@_;
  $a=lc($a);
  $a=~tr/áâ÷çäå³öúéêëìíîïğòóôõæèãşûıÿùøüàñ/ÁÂ×ÇÄÅ£ÖÚÉÊËÌÍÎÏĞÒÓÔÕÆÈÃŞÛİßÙØÜÀÑ/;
  return $a;
}

sub ucr
{
  my ($a)=@_;
  $a=uc($a);
  $a=~tr/ÁÂ×ÇÄÅ£ÖÚÉÊËÌÍÎÏĞÒÓÔÕÆÈÃŞÛİßÙØÜÀÑ/áâ÷çäå³öúéêëìíîïğòóôõæèãşûıÿùøüàñ/;
  return $a;
}

sub getwkg
{
  my ($ip)=@_;
  return 0 if $ip=~m#^7\.#;
  return 1 if $ip=~m#^10\.16\.[0-9]\.#;
  return 2 if $ip=~m#^10\.16\.(1[6-9]|2[0-9]|3[0-1])\.#;
  return 3 if $ip=~m#^10\.16\.(3[2-9]|4[0-7])\.#;
  return 4 if $ip=~m#^10\.16\.(4[8-9]|5[0-9]|6[0-3])\.#;
  return 5 if $ip=~m#^10\.16\.(6[4-9]|7[0-9])\.#;
  return 6 if $ip=~m#^213\.85\.#;
  return 7 if $ip=~m#^10\.107\.#;
  return 8 if $ip=~m#^194\.135\.#;
  return 0;
}

