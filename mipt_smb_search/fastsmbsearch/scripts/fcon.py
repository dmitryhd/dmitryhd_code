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
from smbsearch import *

fsearch_connect()
print "Welcome to fsearch console! Enter request, empty line to exit"
while True:
	line = raw_input(">> ")
	line = string.strip(line)
	if line == "":
		break
	repl = fsearch_request(line)
	while repl:
		repl = string.strip(repl)
		print repl
		if repl in ["END", "ERROR"]:
			break
		repl = fsearch_get_line()

fsearch_close()

