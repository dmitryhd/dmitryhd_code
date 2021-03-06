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

import os
from smbsearch import *

tmp_path = config.get('path', 'tmp_path')
filelist_file = "%s/filelist" % tmp_path;

# Load info about specified host into fsmb server
def load_fsmb(host_id):
	filename = "%s.%d" % (filelist_file, host_id)
	if not os.path.isfile(filename):
		print "Filelist for host %d not found, skipping" % host_id
		return
	fsearch_request("LOAD %d 0 %s" % (host_id, filename))

print_time()
fsearch_connect()
fsearch_request("DEBUG 0")
print "Sorting words..."
fsearch_request("WORDORDER")
host_list_sync()
dbh = db_connect()
c = dbh.cursor()
c.execute("select comp_id from hosts")
while True:
	r = c.fetchone()
	if r == None:
		break
	host_id = r[0]
	load_fsmb(host_id)
fsearch_close()
os.system("sync")
print "Update completed."
