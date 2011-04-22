#!/usr/bin/env python
#
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

# Global variables

import sys
import os
import string
import re
import time
from smbsearch import *
 
filelist_file = None
id = 0
dbh = db_connect()

class ScanError(Exception):
	def __init__(self, msg):
		self.msg = msg
	
	def __str__(self):
		return self.msg

# Add record for share into file list
def add_share(name):
	global id
	global filelist_file
	f = open(filelist_file, 'a')
	f.write("%d\t0\t%s\t0\t1\n" % (id, name))
	f.close()
	share_id = id
	id = id + 1
	return share_id

# Scan specified SMB share
def smb_scan_share(ip, share, share_id):
	global id
	global filelist_file
	print "Scanning %s" % share;
	psmbclient = config.get('path', 'psmbclient')
	cmd = [psmbclient, share, '-N', '-U', 'guest', '-I', ip, '-c', 'recurse; ls',
		'-p', '139', '-d', '0', '-y', str(share_id), '-f', str(id), '-e', filelist_file]
	print "Running " + string.join(cmd)
	# FIXME: it is very stupid
	pid = os.fork()
	if pid == 0:
		os.close(0)
		os.close(1)
		os.close(2)
		os.execv(psmbclient, cmd)
		sys.exit(1)
	(childpid, retval) = os.waitpid(pid, 0)
	# Don't fail when scanner was aborted it the middle of the process
	# It may have scanned some files, we need to update $id even if it fails
	out = os.popen("tail -1 %s" % filelist_file)
	last_line = out.readline()
	m = re.search("^([0-9]+)", last_line)
	if m:
		next_id = int(m.group(1)) + 1
		if next_id > id:
			id = next_id
	print "Next id after scanning is %s" % id;
	return retval

# Get list of SMB shared on specified computer (old method, via SMB client)
def smb_get_share_list_old(hostname, ip):
	psmbclient = config.get('path', 'psmbclient')
	cmd = "%s -L \"%s\" -N -U guest -I %s -p 139 -d 0 2>/dev/null" % (psmbclient, hostname, ip)
	print "Running " + cmd
	out = os.popen(cmd)
	lines = out.readlines()
	if out.close() != None:
		raise ScanError("smbclient failed at host %s" % hostname)
	shares = []
	while (len(lines) > 0) and not re.match("^\tSharename", lines[0]):
		del lines[0]
	del lines[0]
	del lines[0]
	for str in lines:
		str = string.strip(str)
		if str == "":
			break
		m = re.match("^Error returning browse list: (.*)", str)
		if m:
			raise ScanError(m.group(1))
		m = re.match("^(.{15})Disk", str)
		if m:
			shares.append(string.strip(m.group(1)))
	return shares

# Get list of SMB shared on specified computer (new method, via MS-RPC client)
def smb_get_share_list_rpc(hostname, ip):
	prpclient = config.get('path', 'prpclient')
	cmd = "%s \"%s\" -N -U Guest -I %s -d 0 -c 'netshareenum 1' 2>/dev/null" % (prpclient, ip, ip)
	print "Running " + cmd
	out = os.popen(cmd)
	lines = out.readlines()
	if out.close() != None:
		raise ScanError("prpcclient failed at host %s" % hostname)
	shares = []
	for str in lines:
		str = string.strip(str)
		m = re.match("^netname: (.*)", str)
		if m:
			shares.append(m.group(1))
			continue
	return shares

# Get list of SMB shared on specified computer (wrapper)
def smb_get_share_list(hostname, ip):
	# There are two ways of enumerate share on remote host
	# The old, most used one is smbclient -L but it is buggy
	# and don't shows shares with long names
	#
	# The new one is based on MS-RPC mechanism implemented in Samba 3.0
	#
	# We first good one, and if it fails fall to old.
	try:
		return smb_get_share_list_rpc(hostname, ip)
	except ScanError, (msg):
		try:
			print "RPC NetShareEnum failed: %s. Trying old buggy method" % msg
			return smb_get_share_list_old(hostname, ip)
		except ScanError, (msg):
			print "Got error %s" % msg
			return None

# Scan specified computer via SMB
def smb_scan_computer(hostname, ip):
	print "Scanning SMB host %s" % hostname
	add_share("")
	shares = smb_get_share_list(hostname, ip)
	if shares == None:
		raise ScanError("Cannot get share list at host %s" % hostname)
	share_counter = 0
	for share in shares:
		# Skip admin, IPC shares
		if share == "IPC$":
			continue
		if share == "ADMIN$":
			continue
		if re.match("^[C-Z]\$",share):
			continue
		share_id = add_share(share)
		retval = smb_scan_share(ip, "//%s/%s" % (hostname, share), share_id)
		if retval != 0:
			print "Error scanning share //%s/%s" % (hostname, share)
			continue
		share_counter = share_counter + 1
	return share_counter

# Scan specified computer via FTP
def ftp_scan_computer(hostname, ip, charset, user, password):
	global filelist_file
	global id
	print "Scanning FTP host %s" % hostname
	add_share("")
	ftpspider = config.get('path', 'ftpspider')
	internal_charset = config.get('options', 'internal_charset')
	default_ftp_charset = config.get('options', 'default_ftp_charset')
	default_ftp_user = config.get('options', 'default_ftp_user')
	default_ftp_password = config.get('options', 'default_ftp_password')
	ftp_passive = config.get('options', 'ftp_passive')
	ftp_allow_dtp = config.get('options', 'ftp_allow_dtp')
	ftp_allow_nodtp = config.get('options', 'ftp_allow_nodtp')
	opts="";
	if (internal_charset != None) and ((charset != None) or (default_ftp_charset != None)):
		if charset == None:
			charset = default_ftp_charset
		if charset != internal_charset:
			opts="-I %s -O %s" % (charset,internal_charset)
	if (user == None) and (default_ftp_user != None):
		user=default_ftp_user
		password=default_ftp_password
	if (user != None) and (password == None):
		password=""
	if (user != None) and (password != None):
		opts = opts + (" -u%s -p%s" % (user,password))
	if ftp_passive != None:
		opts = opts + (" -P%s" % ftp_passive)
	if ftp_allow_dtp!= None:
		opts = opts + (" -d%s" % ftp_allow_dtp)
	if ftp_allow_nodtp!= None:
		opts = opts + (" -D%s" % ftp_allow_nodtp)
	cmd = "%s %s %s >>%s" % (ftpspider, ip, opts, filelist_file)
	print "Running " + cmd
	if os.system(cmd) != 0:
		raise ScanError("ftpspider returned unsuccesfully at host %s" % hostname)
	# Don't fail when scanner was aborted it the middle of the process
	# It may have scanned some files, we need to update $id even if it fails
	out = os.popen("tail -1 %s" % filelist_file)
	last_line = out.readline()
	m = re.search("^([0-9]+)", last_line)
	if m:
		next_id = int(m.group(1)) + 1
		if next_id > id:
			id = next_id
	print "Next id after scanning is %s" % id;
	return 1

# Update statistics
def update_stats(host_id):
	global dbh
	# Delete old content
	curs = dbh.cursor()
	curs.execute("delete from stat where comp_id = %d" % host_id)
	stat = fsearch_request("HOSTSTAT %d" % host_id)
	res_size = string.split(stat, "\t")
	for i in range(len(res_size)):
		sum = res_size[i]
		curs.execute("insert into stat (comp_id, res_type, size) values (%d, %d, %s)" % (host_id, i, sum))

# Load info from files into SQL and fsearch server
def load_files(host_id):
	global filelist_file
	files_fsearch = fsearch_request("LOAD %d 0 %s" % (host_id, filelist_file))
	print "%s rows added to fsearch" % files_fsearch

# Rescan computer
def rescan_comp(host_id):
	global id
	global dbh
	global filelist_file
	
	curs = dbh.cursor()
	curs.execute("select name,wkg_id,ip,proto,status,charset,user,password from hosts where comp_id=%d" % host_id)
	r = curs.fetchone()
	hostname = r[0]
	wkg_id = r[1]
	ip = r[2]
	proto = r[3]
	status = r[4]
	charset = r[5]
	user = r[6]
	password = r[7]
	if status == "offline":
		return
	print "Scanning host %d (%s): ip=%s wkg_id=%s" % (host_id, hostname, ip, wkg_id)
	filelist_file = "%s/filelist.%d" % (config.get('path', 'tmp_path'), host_id)
	# Reset id
	id = 0
	try:
		os.unlink(filelist_file)
	except OSError:
		pass
	try:
		if proto == 0:
			share_count = smb_scan_computer(hostname, ip)
		else:
			if proto == 1:
				share_count = ftp_scan_computer(hostname, ip, charset, user, password)
			else:
				print "Unknown protocol	%s at host %d (%s)" % (proto, host_id, host_name);
	except ScanError, (msg):
		print msg
		curs.execute("update hosts set last_scan=now(), status = \"error\" where comp_id = %d" % host_id);
		return 
	curs.execute("lock tables hosts write, stat write")
	load_files(host_id)
	curs.execute("update hosts set last_scan=now(), share_count = %d, status = \"online\" where comp_id = %d" % (share_count, host_id));
	update_stats(host_id)
	curs.execute("unlock tables")

fsearch_connect()
while True:
	query = ""
	ftp_support = config.get('options', 'ftp_support')
	if ftp_support:
		query = '(status = "unchecked")'
	else:
		query = '(status = "unchecked") and (proto = 0)'
	curs = dbh.cursor()
	curs.execute("select comp_id from hosts where %s order by last_scan asc limit 1" % query);
	print_time()
	r = curs.fetchone()
	if r:
		comp_id = int(r[0])
		rescan_comp(comp_id)
		host_list_sync()
	else:
		idle = int(config.get('options', 'rescan_sleep'))
		print "Nothing to do, going to sleep for %d sec" % idle;
		time.sleep(idle);
	os.system("sync")

fsearch_close();
print "Update completed."

