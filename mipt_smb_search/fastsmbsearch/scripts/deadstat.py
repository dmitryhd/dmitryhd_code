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

dbh = db_connect()
fsearch_connect()
curs = dbh.cursor()
curs.execute("select stat.comp_id from stat left join hosts on stat.comp_id = hosts.comp_id where stat.res_type = 0 and hosts.comp_id is NULL");

dead = []
while True:
	r = curs.fetchone()
	if r == None:
		break
	id = int(r[0])
	dead.append(id)

for id in dead:
	print "Deleting statistics for non-existent comp %d" % id
	curs.execute("delete from stat where comp_id = %d" % id)
	fsearch_request("DELETEHOST %d" % id)

fsearch_close()
