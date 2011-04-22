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
import signal
from smbsearch import *

logfile = "netserver.log"

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_port = int(config.get('netserver', 'port'))
server.bind(('', server_port))
server.listen(2)

log = open(logfile, "a")

# ignore SIGPIPE and SIGCHLD
signal.signal(signal.SIGCHLD, signal.SIG_IGN)
signal.signal(signal.SIGPIPE, signal.SIG_IGN)

while True:
	(client, address) = server.accept()
	pid = os.fork() # fork
	if pid != 0:
		# wait for next client if child
		client.close()
		client = None
		continue
	server.close()
	server = None
	other_end = client.getpeername()
	(client_port, client_ip) = other_end
	query = client.recv(4096)
	p = string.find(query, "\n")
	if p != -1:
		query = query[0:p]
	date = time.ctime()
	log.write("%s %s %s\n" % (date, client_ip, query))
	response = "ERROR Invalid query"
	if query.startswith("SEARCH "):
		fsearch_connect()
		reply = fsearch_request(query)
		response = reply
		if re.match("^QUERY_ID ([0-9]+) HINT .+$", reply):
			reply = fsearch_get_line()
			m = re.match("^COUNT ([0-9]+) ([0-9]+) ([0-9]+(\.[0-9]*)?)", reply)
			response = response + "\n" + reply
			matched = int(m.group(1))
			returned = int(m.group(2))
			for j in range(returned):
				line = fsearch_get_line()
				(id, parent_id, name, size, is_dir, res_type,
						path, wkg_id, host_id, proto, host_name, host_ip, status) = string.split(line, "\t")
				newline = "%s/%s/%s\t%s\t%s\t%s\t%s\t%s\t%s" % (host_ip, path, name, size, is_dir, res_type, host_name, status, proto)
				response = response + "\n" + newline
			response = response + "\n"
			fsearch_close()
	client.send(response)
	client.shutdown(2)
	client.close()
	sys.exit(0)

