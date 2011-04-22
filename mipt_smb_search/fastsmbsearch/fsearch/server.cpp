/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: server.cpp,v 1.12 2005/09/08 18:52:52 alex_vodom Exp $
 *
 * Fast SMB Search is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Fast SMB Search is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "record.h"
#include "classify.h"
#include "hostdata.h"
#include "hostset.h"
#include "parser.h"
#include "qcache.h"
#include "server.h"

#ifdef LINUX
#define NO_SIG_OPTION MSG_NOSIGNAL
#elif FREEBSD
#define NO_SIG_OPTION SO_NOSIGPIPE
#else
#define NO_SIG_OPTION 0
#endif

extern int caught_sigterm;

inline std::string num_to_str(int num)
{
	char buf[20];
	sprintf(buf, "%d", num);
	return std::string(buf);
}

inline std::string double_to_str(const double num)
{
	char buf[30];
	sprintf(buf, "%.3f", num);
	return std::string(buf);
}


CServer::CServer() : m_host_set(), m_cache(m_host_set)
{
	logger->log(0, "Server initializing...");
	m_request_count = 0;
	m_sock_buf_size = 200;
	m_sock_buf = new char[m_sock_buf_size];
}

CServer::~CServer()
{
	delete[] m_sock_buf;
	close(m_listen_socket);
	if (!m_socket_name.empty()) {
		unlink(m_socket_name.c_str());
	}
	logger->log(0, "Shutting down server...");
}

void CServer::send_string(const std::string& str)
{
	m_current_reply += str;
	m_current_reply += '\n';
}

void CServer::send_string_fmt(const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	while (vsnprintf(m_sock_buf, m_sock_buf_size, format, args) >= m_sock_buf_size) {
		delete[] m_sock_buf;
		m_sock_buf_size *= 2;
		m_sock_buf = new char[m_sock_buf_size];
	}
	va_end(args);
	send_string(m_sock_buf);
}

size_t CServer::mem_size() const
{
	return m_host_set.mem_size() + wordhash.mem_size();
}

// Syntax: SEARCH num1 num2 [short|full] hostfilter query
void CServer::search_request(const char *request)
{
	int num_begin, num_end;
	bool is_full = true;

	if (strlen(request) == 0) throw std::runtime_error("Empty request");
	const char *p = request;
	p = strchr(request, ' ');
	if (p == 0) {
		throw std::runtime_error("Invalid search request");
	}
	while (*p == ' ') p++;
	p = strchr(p, ' ');
	if (p == 0) {
		throw std::runtime_error("Invalid search request");	
	}
	while (*p == ' ') p++;
	if (strncmp(p, "SHORT", 5) == 0) {
		p += 5;
		is_full = false;
		while (*p == ' ') p++;
	}
	const char *query = p;
	if (strlen(query) == 0) {
		throw std::runtime_error("Empty search query");
	}
	if (sscanf(request, "%d %d", &num_begin, &num_end) != 2) {
		throw std::runtime_error("Minimal or maximal number is invalid");
	}
	if ((num_begin < 0) || (num_end < num_begin)) {
		throw std::runtime_error("Minimal or maximal number is invalid");
	}
	if (num_end - num_begin > 10000) {
		num_end = num_begin + 10000;
	}

	int error;
	CCachedQuery *cached = m_cache.do_query(p, error);
	if (error == CQueryCache::SyntaxError) {
		throw std::runtime_error("Syntax error");
	}
	if (error == CQueryCache::NoHintFound) {
		send_string("FULLSCANREQUIRED");
		return;
	}
	char query_info[100];
	snprintf(query_info, 100, "QUERY_ID %d HINT %.4f%%", cached->id(), cached->hint_level() * 100.0);
	send_string(query_info);
	int matched = cached->count();
	const std::vector<CFoundItem>& result = cached->get_result();
	if (num_begin >= matched) {
		send_string("COUNT " + num_to_str(matched) + " 0 " +
				double_to_str(cached->search_time()));
		return;
	}
	if (num_end > matched) {
		num_end = matched;
	}
	int returned  = num_end - num_begin;
	send_string("COUNT " + num_to_str(matched) + ' ' + 
			num_to_str(returned) + ' ' + double_to_str(cached->search_time()));
	char record_buf[FILE_RECORD_STRING_LEN];
	for (int i = num_begin; i < num_end; i++) {
		CRecordPtr ptr = result[i].m_record;
		const CFileRecord& record = *ptr;
		const CHostData *host = result[i].m_host;
		std::ostringstream buf;
		sprintf(record, record_buf);
		buf << record_buf;
		if (is_full) {
			buf << '\t';
			if (record.parent().notNull()) buf << path(*record.parent());
			buf << '\t' << host->wkg_id();
			buf << '\t' << host->host_id();
			buf << '\t' << host->proto();
			buf << '\t' << host->name();
			buf << '\t' << host->ip();
			buf << '\t' << host->status();
		}
		send_string(buf.str());
	}
	// Disable exhaustive logging
	logger->log(2, "search time: %5.3lf sec", cached->search_time());
}

void CServer::count_request()
{
	send_string(num_to_str(m_host_set.file_count()));
}

void CServer::host_info_request(const char *id_string)
{
	int host_id;

	if (sscanf(id_string, "%d", &host_id) != 1) {
		throw std::runtime_error(std::string("Cannot extract host id from host request: ") + id_string);
	}
	const CHostData *host = m_host_set.find_host(host_id);
	if (host == 0) {
		throw std::runtime_error("Cannot write info about unknown host " + num_to_str(host_id));
	}
	send_string(sprintf(*host));
}

void CServer::host_stat_request(const char *id_string)
{
	int host_id;

	if (sscanf(id_string, "%d", &host_id) != 1) {
		throw std::runtime_error(std::string("Cannot extract host id from host request: ") + id_string);
	}
	const CHostData *host = m_host_set.find_host(host_id);
	if (host == 0) {
		throw std::runtime_error(std::string("Cannot write stats about unknown host ") + num_to_str(host_id));
	}
	long long unsigned int res_size[RES_TYPE_MAX];
	const CFileSet& fs = host->fileset();
	fs.res_type_stat(res_size);
	char buf[200];
	char *ptr;
	ptr = buf;
	for (int i = 0; i < RES_TYPE_MAX; i++) {
		ptr += sprintf(ptr, "%llu\t", res_size[i]);
	}
	// cut trailing \t
	ptr--;
	*ptr = 0;
	send_string(std::string(buf));
}

void CServer::delete_request(const char *id_string)
{
	int host_id;
	CHostData *host;
	
	if (sscanf(id_string, "%d", &host_id) != 1) {
		throw std::runtime_error(std::string("Cannot extract id from id request: ") + id_string);
	}
	host = m_host_set.find_host(host_id);
	if (host == 0) {
		throw std::runtime_error(std::string("Request to delete unexisting host ") + num_to_str(host_id));
	}
	logger->log(2, "Invalidating cache");
	m_cache.invalidate_host(host_id);
	logger->log(2, "Deleting host %d (%s): %d files", host_id, host->name(), host->fileset().count());
	int deleted = host->fileset().count();
	m_host_set.delete_host(host_id);
	send_string(num_to_str(deleted));
}

void CServer::load_request(const char *request)
{
	if (strlen(request) == 0) {
		throw std::runtime_error("Empty load request");
	}
	const char *p = request;
	p = strchr(request, ' ');
	if (p == 0) {
		throw std::runtime_error("Invalid load request");
	}
	while (*p == ' ') p++;
	p = strchr(p, ' ');
	if (p == 0) {
		throw std::runtime_error("Invalid load request");
	}
	while (*p == ' ') p++;
	const char *file_name = p;
	if (strlen(file_name) == 0) {
		throw std::runtime_error("Empty filename in load request");
	}
	int host_id = 0;
	int estimated = 0;
	if (sscanf(request, "%d %d", &host_id, &estimated) != 2) {
		throw std::runtime_error("Host id or estimated count is invalid");
	}
	CHostData *host = m_host_set.find_host(host_id);
	if (host == 0) {
		throw std::runtime_error("Cannot load file list for unknown host " + num_to_str(host_id));
	}
	logger->log(2, "Invalidating cache");
	m_cache.invalidate_host(host_id);
	logger->log(2, "Loading file for host %d from file %s", host_id, file_name);
	host->fileset().load_from_file(file_name);
	int new_count = host->fileset().count();
	send_string(num_to_str(new_count));
	logger->log(2, "%d new records inserted", new_count);
	if (new_count > 0) {
		logger->log(2, "Flushing cache");
		m_cache.invalidate_all();
	}
}

void CServer::dump_files_request(const char *request)
{
	const char *p = request;
	p = strchr(request, ' ');
	if (p == 0) {
		throw std::runtime_error("Invalid dump request");
	}
	while (*p == ' ') p++;
	const char *file_name = p;
	if (strlen(file_name) == 0) {
		throw std::runtime_error("Empty filename in dump request");
	}
	int host_id = 0;
	if (sscanf(request, "%d", &host_id) != 1) {
		throw std::runtime_error("Invalid host id");
	}
	const CHostData *host = m_host_set.find_host(host_id);
	if (host == 0) {
		throw std::runtime_error("Cannot dump files for unknown host " + num_to_str(host_id));
	}
	logger->log(2, "Dumping files for host %d to file %s", host_id, file_name);
	dump(host->fileset(), file_name);
	send_string("OK");
}

void CServer::dump_index_request(const char *request)
{
	const char *p = request;
	p = strchr(request, ' ');
	if (p == 0) {
		throw std::runtime_error("Invalid dump request");
	}
	while (*p == ' ') p++;
	const char *file_name = p;
	if (strlen(file_name) == 0) {
		throw std::runtime_error("Empty filename in dump request");
	}
	int host_id = 0;
	if (sscanf(request, "%d", &host_id) != 1) {
		throw std::runtime_error("Invalid host id");
	}
	const CHostData *host = m_host_set.find_host(host_id);
	if (host == 0) {
		throw std::runtime_error("Cannot dump index for unknown host " + num_to_str(host_id));
	}
	logger->log(2, "Dumping word index for host %d to file %s", host_id, file_name);
	dump_index(host->fileset(), file_name);
	send_string("OK");
}

void CServer::status_request()
{
	std::ostringstream buf;
	buf << m_clients.size() << " clients active, ";
	buf << m_host_set.host_count() << " hosts, ";
	buf << m_host_set.file_count() << " files, ";
	buf << m_cache.count() << " items in cache, ";
	buf << m_host_set.mem_size()/(1024*1024) << " Mb memory used";
	send_string(buf.str());
}

void CServer::cache_info_request()
{
	std::ostringstream buf;
	buf << m_cache.count() << " entires, ";
	buf << "total size: " << m_cache.mem_size() << ", ";
	buf << "hits: " << m_cache.hits() << " (" << m_cache.total() << ")";
	send_string(buf.str());
}

void CServer::update_hosts_request(const char *file_name)
{
	m_host_set.update_hosts(file_name);
	send_string("OK");
	m_cache.invalidate_all();
}

void CServer::debug_request(const char *level_string)
{
	int level;

	if (sscanf(level_string, "%d", &level) != 1) {
		throw std::runtime_error(std::string("Cannot extract debug level from request: ") + level_string);
	}
	logger->set_level(level);
	send_string(std::string("Debug level ") + num_to_str(logger->get_level()));
}

void CServer::dump_state_request()
{
	swapAllocator->debug_dump();
	save_state();
	send_string("OK");
}

void CServer::word_order_request()
{
	recalculate_best_substr(wordhash, searchhistory);
	send_string("OK");
}

void CServer::clear_querycache_request()
{
	m_cache.invalidate_all();
	send_string("OK");
}

void CServer::clear_pagecache_request()
{
	clear_page_cache();
	send_string("OK");
}

void CServer::save_state()
{
	save(wordhash, "wordhash.txt");
	save(searchhistory, "history");
}

void CServer::load_state()
{
	load(wordhash, "wordhash.txt");
	load(searchhistory, "history");
}

void CServer::server_stat_request()
{
	send_string("FSearch server build on " __DATE__ " " __TIME__);
	send_string("FSearch server version " VERSION);
	send_string_fmt("Active clients: %u", m_clients.size());
	send_string_fmt("Debug level: %d", logger->get_level());
	send_string_fmt("Allocated in swap: %u Mb", swapAllocator->allocated()/(1024*1024));
	send_string_fmt("Total swap space used: %u Mb", swapAllocator->memUsed()/(1024*1024));
	send_string_fmt("Total hosts: %u", m_host_set.host_count());
	send_string_fmt("Total files: %u", m_host_set.file_count());
	send_string_fmt("Total files size: %d Gb", m_host_set.total_size()/(1024*1024*1024));
	if (m_host_set.host_count() > 0) {
		send_string_fmt("Average files per host: %d", m_host_set.file_count()/m_host_set.host_count());
	}
	if (m_host_set.file_count() > 0) {
		send_string_fmt("Average file size: %d", m_host_set.total_size()/m_host_set.file_count());
	}
	send_string_fmt("Total server memory usage: %u Mb", mem_size()/(1024*1024));
	send_string_fmt("Number of swap pages: %u Mb", page_max*page_size/(1024*1024));
	send_string_fmt("Present swap pages: %u Mb", page_present*page_size/(1024*1024));
	send_string_fmt("Locked swap pages: %u Mb", page_locked*page_size/(1024*1024));
	send_string_fmt("Number of page loads: %lu", page_read_count);
	send_string_fmt("Number of page swaps: %lu", page_swap_count);
	send_string_fmt("Total queries: %u", m_cache.total());
	send_string_fmt("Total uncached queries: %u", m_cache.query_made());
	send_string_fmt("Average query time: %.3lf sec", m_cache.avg_search_time());
	send_string_fmt("Maximal query time: %.3lf sec", m_cache.max_search_time());
	send_string_fmt("Queries in cache: %u", m_cache.count());
	send_string_fmt("Query cache size: %u Kb", m_cache.mem_size()/1024);
	send_string_fmt("Query cache hits: %u", m_cache.hits());
	if (m_cache.total() > 0) {
		send_string_fmt("Query cache hit ratio: %5.2lf%%", 100.0*m_cache.hits()/m_cache.total());
		send_string_fmt("Average query matches: %d", m_cache.total_matched()/m_cache.total());
	}
	send_string_fmt("Word hash size: %u", wordhash.size());
	send_string_fmt("Dictionary index size: %u Kb", wordhash.get_dictindex().mem_size()/1024);
	send_string_fmt("Max collisions: %u", wordhash.max_collisions());
	send_string_fmt("Total words: %u", wordhash.total_words());
	send_string_fmt("Common words: %u", wordhash.common_count());
	send_string("END");
}

void CServer::enable_full_request(const char *opt_string)
{
	int enable;
	
	if (sscanf(opt_string, "%d", &enable) != 1) {
		throw std::runtime_error(std::string("Cannot extract interge value from request: ") + opt_string);
	}
	m_cache.enable_full_scan(enable != 0);
	send_string(std::string("Full scan ") + (m_cache.full_scan_enabled() ? "enabled" : "disabled"));
}

void CServer::handle_request(const char *request)
{
	const char *p;
	int cmd_len;
	int ok;

	logger->log(3, "Got request: %s", request);
	p = strchr(request,' ');
	if (p == 0) {
		p = request + strlen(request);
	}
	cmd_len = p - request;
	while (*p == ' ') {
		p++;
	}
	ok = 0;
	if (cmd_len == 0) {
		logger->log(2, "Empty request!");
		return;
	}
	char *command = new char[cmd_len+1];
	memcpy(command, request, cmd_len);
	command[cmd_len] = 0;
	try {
		if (strcmp(command, "SEARCH") == 0) {
			ok = 1;
			search_request(p);
		}
		if (strcmp(command, "COUNT") == 0) {
			ok = 1;
			count_request();
		}
		if (strcmp(command, "DELETEHOST") == 0) {
			ok = 1;
			delete_request(p);
		}
		if (strcmp(command, "LOAD") == 0) {
			ok = 1;
			load_request(p);
		}
		if (strcmp(command, "STATUS") == 0) {
			ok = 1;
			status_request();
		}
		if (strcmp(command, "HOSTINFO") == 0) {
			ok = 1;
			host_info_request(p);
		}
		if (strcmp(command, "HOSTSTAT") == 0) {
			ok = 1;
			host_stat_request(p);
		}
		if (strcmp(command, "CACHEINFO") == 0) {
			ok = 1;
			cache_info_request();
		}
		if (strcmp(command, "UPDATEHOSTS") == 0) {
			ok = 1;
			update_hosts_request(p);
		}
		if (strcmp(command, "SERVERSTAT") == 0) {
			ok = 1;
			server_stat_request();
		}
		if (strcmp(command, "DEBUG") == 0) {
			ok = 1;
			debug_request(p);
		}
		if (strcmp(command, "DUMP") == 0) {
			ok = 1;
			dump_state_request();
		}
		if (strcmp(command, "FULLSCAN") == 0) {
			ok = 1;
			enable_full_request(p);
		}
		if (strcmp(command, "WORDORDER") == 0) {
			ok = 1;
			word_order_request();
		}
		if (strcmp(command, "CLEARPAGECACHE") == 0) {
			ok = 1;
			clear_pagecache_request();
		}
		if (strcmp(command, "CLEARQUERYCACHE") == 0) {
			ok = 1;
			clear_querycache_request();
		}
		if (strcmp(command, "DUMPFILES") == 0) {
			ok = 1;
			dump_files_request(p);
		}
		if (strcmp(command, "DUMPINDEX") == 0) {
			ok = 1;
			dump_index_request(p);
		}
		if (!ok) {
			throw std::runtime_error(std::string("Unrecognized request: ") + request);
		}
	}
	catch (std::runtime_error &e) {
		logger->log(1, "Error: %s", e.what());
		send_string("ERROR");
	}
	delete[] command;	
	m_request_count++;
}

void CServer::listen(const std::string& socket_name)
{
	struct sockaddr_un sock_addr;
	int error;
	
	m_listen_socket = socket(PF_UNIX, SOCK_STREAM, 0);
	memset(&sock_addr, 0, sizeof(struct sockaddr_un));
	sock_addr.sun_family = AF_UNIX;
	strcpy(sock_addr.sun_path, socket_name.c_str());
	error = bind(m_listen_socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));	
	if (error) {
		std::ostringstream err;
		err << "Cannot bind socket: " << strerror(errno);
		throw std::runtime_error(err.str());
	}
	m_socket_name = socket_name;
	::listen(m_listen_socket, 10);
}
 
void CServer::run()
{
	char req[2048];
	int req_len;
	fd_set read_fds;
	bool go_shutdown = false;

	load_state();
	while (!go_shutdown) {
		// if received sigterm, quit
		if (caught_sigterm) {
			go_shutdown = true;
			break;
		}
		// fill structure for select
		FD_ZERO(&read_fds);
		int max_fd = m_listen_socket;
		FD_SET(m_listen_socket, &read_fds);
		std::vector<int>::iterator i;
		for (i = m_clients.begin(); i < m_clients.end(); i++) {
			if ((*i) > max_fd) {
				max_fd = *i;
			}
			FD_SET(*i, &read_fds);
		}
		int retval = select(max_fd + 1, &read_fds, 0, 0, 0);
		if (retval < 0) {
			logger->log(0, "Error: select() returned error %s.", strerror(errno));
			continue;
		}
		if (FD_ISSET(m_listen_socket, &read_fds)) {
			int client_socket = accept(m_listen_socket, 0, 0);
			if (client_socket < 0) {
				logger->log(0, "Error: cannot accept connection.");
				continue;
			}
			m_clients.push_back(client_socket);
		}
		i = m_clients.begin();
		while (i != m_clients.end()) {
			if (FD_ISSET(*i, &read_fds)) {
				m_current_client = *i;
				req_len = recv(m_current_client, req, 1024, NO_SIG_OPTION);
				// Data ended or error
				if (req_len <= 0) {
					if (req_len < 0) {
						logger->log(0, "recv() returned error: %s", strerror(errno));
					}
					shutdown(m_current_client, SHUT_RDWR);
					close(m_current_client);
					i = m_clients.erase(i);
					continue;
				}
				req[req_len] = 0;
				req_len = strlen(req);
				if (req_len > 0) {
					if (req[req_len-1] == '\n') req_len--;
					req[req_len] = 0;
				}
				if (strcmp(req, "QUIT") == 0) {
					go_shutdown = true;
					break;
				}
				m_current_reply = "";
				handle_request(req);
				int length = m_current_reply.size();
				int sent = send(m_current_client, m_current_reply.c_str(), length, NO_SIG_OPTION);
				if (sent != length) {
					logger->log(1, "Failed sending data to client: %d bytes of %d total", sent, length);
				}
			}
			i++;
		}
	}
	std::vector<int>::iterator i;
	for (i = m_clients.begin(); i < m_clients.end(); i++) {
		shutdown(*i, SHUT_RDWR);
		close(*i);
	}
	save_state();
}

