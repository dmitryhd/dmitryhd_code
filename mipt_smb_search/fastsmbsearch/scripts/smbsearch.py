#!/usr/bin/env python

import os
import re
import string
import time
import socket
import ConfigParser
import MySQLdb

config_file = 'smbsearch.conf'
config = None
fsearch_sock = None

def read_config():
	global config
	config = ConfigParser.ConfigParser()
	config.readfp(open(config_file))
	config.sections()

# Return's current config
def config():
	global config
	return config

# Print current time to stdout
def print_time():
	print time.ctime()

# Return current GMT time as timestamp
def now_time():
	print time.strftime("%Y%m%d%h%m%s")

# Connect to database
def db_connect():
	global config
	db_host = config.get('database', 'host')
	db_user = config.get('database', 'user')
	db_pass = config.get('database', 'pass')
	db_name = config.get('database', 'name')
	if db_name == "":
		print "Database not defined.\n"
		sys.exit(1);
	return MySQLdb.connect(db_host, db_user, db_pass, db_name)

# Send request to fsearch server, return server results
def fsearch_connect():
	global config
	global fsearch_sock
	sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
	sock.connect((config.get('path', 'fsearch_sock')))
	fsearch_sock = sock.makefile()

# Send request and receive one string from server
def fsearch_request(req_line):
	global fsearch_sock
	fsearch_sock.write(req_line + "\n")
	fsearch_sock.flush()
	return string.rstrip(fsearch_sock.readline(), "\n")

# return one line from string
# may block (wait)
def fsearch_get_line():
	return string.rstrip(fsearch_sock.readline(), "\n")

# Close connection to fsearch server
def fsearch_close():
	global fsearch_sock
	fsearch_sock.close()

# Get host list from SQL server and send it to fsearch server
def host_list_sync():
	global config
	q = "select comp_id, wkg_id, proto, name, ip, status, last_scan from hosts"
	db_name = config.get('database','name')
	db_pass = config.get('database', 'pass')
	pass_opt = ""
	if db_pass:
		pass_opt = "-p" + db_pass
	hostlist_file = "%s/hostlist.tmp" % config.get('path', 'tmp_path')
	db_user = config.get('database', 'user')
	os.system("mysql -u%s %s -NB -e \"%s\" %s >%s" % (db_user, pass_opt, q, db_name, hostlist_file))
	fsearch_request("UPDATEHOSTS %s" % hostlist_file)

# Return 1 if given ip is valid local ip
def is_local_ip(ip):
	global config
	nets = config.get('network', 'local_subnets')
	for net in string.split(nets):
		if len(net) > len(ip):
			continue
		if ip[0:len(net)] == net:
			return True
	return False

# Get a IP list of hosts in workgroup
def nmb_scan_wkg(wkg):
	global config
	nmblookup = config.get('path', 'nmblookup')
	result = []
	out = os.popen("%s %s#0 2>/dev/null" % (nmblookup, wkg))
	for outstr in out.readlines():
		m = re.search("^(\d+\.\d+\.\d+\.\d+)", string.strip(outstr))
		if not m:
			continue
		ip = m.group(1)
		if is_local_ip(ip):
			result.append(ip)
	return result

# Get a IP list of hosts in subnet
def nmap_scan_subnet(ip_range):
	global config
	nmap = config.get('path', 'nmap')
	if (config.get('options', 'ftp_support')):
		cmd = "%s -n -sP -PT139 %s 2>/dev/null; %s -n -sP -PT21 %s 2>/dev/null" % (nmap, ip_range, nmap, ip_range)
	else:
		cmd = "%s -n -sP -PT139 %s 2>/dev/null" % (nmap, ip_range)
	out = os.popen(cmd)
	result = []
	for outstr in out.readlines():
		m = re.search("^Host\s+\(?(\d+\.\d+\.\d+\.\d+)\)?", string.strip(outstr))
		if not m:
			continue
		ip = m.group(1)
		result.append(m.group(1))
	return result

def unique(list):
	result = []
	for item in list:
		if not item in result:
			result.append(item)
	return result
	

# Return list of alive IPs on network
def scan_all_net():
	online_ips = []
	for wkg in string.split(config.get('network', 'scan_workgroups')):
		online_ips = online_ips + nmb_scan_wkg(wkg)
	for subnet in string.split(config.get('network', 'scan_subnets')):
		online_ips = online_ips + nmap_scan_subnet(subnet)
	return unique(online_ips)

read_config()

