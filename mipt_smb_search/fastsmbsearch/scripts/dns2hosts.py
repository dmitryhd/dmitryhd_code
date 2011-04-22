#!/usr/bin/env python
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

import sys
import string
import re
import os
from smbsearch import *

invalid_hosts = string.split(config.get('network', 'invalid_hosts'))

# zone_sync - sync given workgroups and DNS zone
#
# Parameters:
#   $domain - name of the DNS domain (e.g. "local")
#   $server - IP of the DNS server (e.g. "172.16.0.11")
#   $wkg    - the ID of workgroup (e.g. 1)
#   $wkg_regexp - the regexp that match all hosts IPs (e.g "172\.16\.1[2367]\.[0-9]+")
#   $special_name_regexp - the regexp that matches special nodes (e.g. "virtual")

def zone_sync(domain, server, wkg, wkg_regexp, special_name_regexp):
	global dbh
	print "Syncing domain %s from server %s to workgroup %s" % (domain, server, wkg);
	seen_ip = {}
	out = os.popen("host -l %s %s" % (domain, server))
	for line in out.readlines():
		file_line = string.strip(line)

		# Get only A type records
		m = re.match("^(.*) has address\s+([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)$", file_line)
		if not m:
			continue
		dns_name = m.group(1)
		ip = m.group(2)
		# Check that IP match workgroup regexp
		if not re.match(wkg_regexp, ip):
			continue
		# Check that FQDN matches domain name
		if dns_name[-len(domain):] != domain:
			continue
		local_name = dns_name[0:len(dns_name)-len(domain)-1]
		if string.find(local_name, ".") != -1:
			continue
		# Don't match special names
		if special_name_regexp:
			if re.match(special_name_regexp, local_name):
				continue
		name = string.upper(local_name)
		if name in invalid_hosts:
			print "%s is invalid hostname" % name
			continue
		# Check for duplicate IP
		if ip in seen_ip:
			print "IP %s (%s) has been already listed as %s, skipping" % (ip, name, seen_ip[ip])
			continue
		seen_ip[ip] = name

		# Check for host with the same IP, but different name
		curs = dbh.cursor()
		curs.execute("select name from hosts where ip = \"%s\"" % ip)
		row = curs.fetchone()
		if row:
			# If already exists
			old_name = row[0]
			if old_name == name:
				continue
			print "IP %s (%s) is taken by another host %s: deleting" % (ip, name, old_name)
			curs.execute("delete from hosts where ip = \"%s\" and name = \"%s\"" % (ip, old_name))
		print "Adding host %s: %s" % (name, ip)
		curs.execute("insert into hosts (wkg_id, name, ip, status) values (%s, \"%s\", \"%s\", \"offline\")" % (wkg, name, ip))
	out.close()

dbh = db_connect()
zone_sync("local", "172.16.0.11", 1, "^172\.16\.([89]|1[2345678]|2[012])\..*", None)
zone_sync("v.ru", "172.16.0.11", 4, "^10\.0\.[0123456789]\..*", "^node[0-9]+-[0-9]+")
zone_sync("hackers", "172.16.0.11", 3, "^172\.16\.[34][0123456789]\..*", None)
zone_sync("gznet.ru", "10.3.0.1", 5, "^10\.[23]\..*", None)

