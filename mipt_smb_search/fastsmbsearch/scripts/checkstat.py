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

from smbsearch import *

print_time()
dbh = db_connect()
curs = dbh.cursor()

online = scan_all_net()
# Get host list from base
curs.execute("select ip, status from hosts")
count = 0
status = {}
while True:
	row = curs.fetchone()
	if row == None:
		break
	status[row[0]] = row[1]
# Set status
new_status = {}
for ip in status.keys():
	new_status[ip] = "offline"
for ip in online:
	# Skip new hosts
	if not ip in status:
		continue
	# Mark online if it was online and pinging
	if status[ip] == "online":
		new_status[ip] = "online"
		continue
	if status[ip] == "error":
		new_status[ip] = "error"
		continue
	# Otherwise make host unchecked
	new_status[ip] = "unchecked"
# Mark outdated info to force rescan
smb_norm_time = config.get('options','smb_rescan_interval')
smb_error_time = config.get('options','smb_error_rescan_interval')
ftp_norm_time = config.get('options','ftp_rescan_interval')
ftp_error_time = config.get('options','ftp_error_rescan_interval')
# Update tables
curs.execute("lock table hosts write")
for ip in status.keys():
	cur_status = new_status[ip]
	if cur_status != "offline":
		count = count + 1
	curs.execute( "update hosts set status=\"%s\", last_ping=now() where ip=\"%s\"" % (cur_status, ip))
# Update last_* times
curs.execute("update hosts set last_online = last_ping where status = \"online\"")
curs.execute("update hosts set last_offline = last_ping where status = \"offline\"")
# update SMB hosts
curs.execute("update hosts set status = \"unchecked\" where (status = \"online\") and (last_scan < now() - interval %s hour) and (proto = 0)" % smb_norm_time);
curs.execute( "update hosts set status = \"unchecked\" where (status = \"error\") and (last_scan < now() - interval %s hour) and (proto = 0)" % smb_error_time);
# update FTP hosts
curs.execute("update hosts set status = \"unchecked\" where (status = \"online\") and (last_scan < now() - interval %s hour) and (proto = 1)" % ftp_norm_time);
curs.execute( "update hosts set status = \"unchecked\" where (status = \"error\") and (last_scan < now() - interval %s hour) and (proto = 1)" % ftp_error_time);
curs.execute( "unlock tables" );
print "%d computers is up." % count
fsearch_connect()
host_list_sync()
fsearch_close()
print_time()
print "Status check completed."
