#!/usr/bin/env python
# This file is part of Fast SMB Search
#
# Copyright 2001 - 2005   Alexander Vodomerov
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
import os
import re
import random
from smbsearch import *

testfile = "allreq.txt"
passed = "passed.txt"
failed = "failed.txt"

max_time = 0.4	# maximal allowed time for query
num_hosts = 4000	 # fixme!
# run LOAD command about one time per 100 searches
load_prob = 0.01

tests_file = open(testfile, "r")
passed_file = open(passed, "w")
failed_file = open(failed, "w")

fsearch_connect()
for query in tests_file.readlines():
	query = string.strip(query)
	time_start = time.clock()
	reply = fsearch_request(query)
	time_query = time.clock() - time_start
	if time_query > max_time:
		time = "%.3f" % time_query
		print "Warning! Too long (%.3f sec) query: %s" % (time_query, query)
	m = re.match("^QUERY_ID ([0-9]+) HINT .+$", reply)
	if not m:
		print "Error in query: %s, got reply: %s" % (query, reply);
		failed_file.write(query + "\n")
		continue
	reply = fsearch_get_line()
	m = re.match("^COUNT ([0-9]+) ([0-9]+) ([0-9]+(\.[0-9]*)?)", reply)
	if not m:
		print "Wrong response from fsearch server"
		failed_file.write(query + "\n")
		continue
	matched = int(m.group(1))
	returned = int(m.group(2))
	for j in range(returned):
		fsearch_get_line()
	passed_file.write(query + "\n")
	# Load hosts data with given probability
	if random.random() < load_prob:
		host = random.randint(0, num_hosts)
		tmp_path = config.get('path', 'tmp_path')
		fsearch_request("LOAD %d 0 %s/filelist.%d" % (host, tmp_path, host))

fsearch_close()
