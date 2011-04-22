/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: hostdata.cpp,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <algorithm>
#include "record.h"
#include "fileset.h"
#include "hostdata.h"
#include "logger.h"

void CHostData::set_status(const char *a_status)
{
	strncpy(m_status, a_status, 32);
	m_status_prio = STATUS_ONLINE;
	if (strcmp(m_status, "unchecked") == 0) {
		m_status_prio = STATUS_UNCHECKED;
	} else {
		if (strcmp(m_status, "error") == 0) {
			m_status_prio = STATUS_ERROR;
		} else {
			if (strcmp(m_status, "offline") == 0) {
				m_status_prio = STATUS_OFFLINE;
			}
		}
	}
}

std::string sprintf(const CHostData& rec)
{
	std::ostringstream buf;
	buf << rec.host_id() << '\t';
	buf << rec.proto() << '\t';
	buf << rec.name() << '\t';
	buf << rec.ip() << '\t';
	buf << rec.status() << '\t';
	buf << rec.wkg_id() << '\t';
	buf << 0 << '\t';
	buf << rec.fileset().count();
	return buf.str();
}

// helper function for CFileRecord::record_from_file
static inline int read_number(const char *&cur_ptr, const char *const end)
{
	int num;
	char ch;
	
	num = 0;
	while (cur_ptr < end) {
		ch = *cur_ptr;
		if (ch == '\t') break;
		if ((ch < '0') || (ch > '9')) {
			logger->log(0, "BUGG: expected digit!");
			return -1;
		}
		num = num*10 + ch - '0';
		cur_ptr++;
	}
	return num;
}

// construct from string
void CHostData::from_string(const char *const begin, const char *const end)
{
	const char *cur_ptr;

	cur_ptr = begin;
	// get id
	m_host_id = read_number(cur_ptr, end);
	if (*cur_ptr != '\t') {
		logger->log(0, "BUGG2!");
		return;
	}
	cur_ptr++;
	// get wkg_id
	m_wkg = read_number(cur_ptr, end);
	if (*cur_ptr != '\t') {
		logger->log(0, "BUGG3");
		return;
	}
	cur_ptr++;
	// get proto
	m_proto = read_number(cur_ptr, end);
	if (*cur_ptr != '\t') {
		logger->log(0, "BUGG3.5");
		return;
	}
	cur_ptr++;
	// get name
	const char *p1 = cur_ptr;
	const char *p2 = strchr(cur_ptr, '\t');
	if (p2 == 0) {
		logger->log(0, "BUGG4");
		return;
	}
	int name_len = p2 - p1;
	if (name_len > 24) {
		logger->log(0, "BUGG: Invalid hostname!");
		return;
	}
	memcpy(m_name, p1, name_len);
	m_name[name_len] = 0;
	cur_ptr = p2+1;
	// get ip
	p1 = cur_ptr;
	p2 = strchr(cur_ptr, '\t');
	if (p2 == 0) {
		logger->log(0, "BUGG5");
		return;
	}
	int ip_len = p2 - p1;
	if (ip_len > 15) {
		logger->log(0, "BUGG: Invalid hostname!");
		return;
	}
	memcpy(m_ip, p1, ip_len);
	m_ip[ip_len] = 0;
	cur_ptr = p2 + 1;
	// get status
	p1 = cur_ptr;
	p2 = p1;
	while ((p2 < end) && (*p2 != 0) && (*p2 != '\t') && (*p2 != '\n')) {
		p2++;
	}
	int stat_len = p2 - p1;
	if (stat_len > 32) {
		logger->log(0, "BUGG7: Invalid status!");
		return;
	}
	char status_buf[32];
	memcpy(status_buf, p1, stat_len);
	status_buf[stat_len] = 0;
	set_status(status_buf);
}

size_t CHostData::mem_size() const
{
	return sizeof(CHostData) + m_files.mem_size();
}
