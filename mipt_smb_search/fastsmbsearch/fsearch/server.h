/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: server.h,v 1.6 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <vector>
#include "fileset.h"
#include "hostset.h"
#include "qcache.h"
#include "logger.h"

#define ANSWER_SIZE 10240

class CServer
{
public:
	explicit CServer();
	~CServer();

	void listen(const std::string& str);
	void run();
	int request_count() const { return m_request_count; }
	size_t mem_size() const;
	
private:
	void handle_request(const char *request);
	void search_request(const char *query);
	void count_request();
	void load_request(const char *file_name);
	void dump_files_request(const char *request);
	void dump_index_request(const char *request);	
	void host_info_request(const char *id_string);
	void host_stat_request(const char *id_string);
	void delete_request(const char *id_string);
	void debug_request(const char *level_string);
	void dump_state_request();
	void clear_pagecache_request();
	void clear_querycache_request();
        void check_request();
	void status_request();
	void cache_info_request();
	void word_order_request();
	void update_hosts_request(const char *file_name);
	void server_stat_request();
	void enable_full_request(const char *opt_string);
	void load_state();
	void save_state();
	void send_string(const std::string& str);
	void send_string_fmt(const char *format, ...);
	
	CHostSet m_host_set;
	CQueryCache m_cache;
	int m_listen_socket;
	std::vector<int> m_clients;
	int m_current_client;
	std::string m_current_reply;
	std::string m_socket_name;
	char *m_sock_buf;
	int m_sock_buf_size;
	int m_request_count;
};

#endif
