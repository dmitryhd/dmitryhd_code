/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: record.cpp,v 1.6 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <cstdio>
#include <cstring>
#include <string>
#include "record.h"
#include "logger.h"
#include "classify.h"

void CFileRecord::classify()
{
	m_res_type = classifier.classify(*this);
}

// helper function for CFileRecord::record_from_file
static inline unsigned long long read_number(const char *&cur_ptr, const char *const end)
{
	unsigned long long num;
	char ch;
	
	num = 0;
	while (cur_ptr < end) {
		ch = *cur_ptr;
		if (ch == '\t') break;
		if ((ch < '0') || (ch > '9')) {
			logger->log(0, "BUGG: expected digit!");
			return 0;
		}
		num = num*10 + ch - '0';
		cur_ptr++;
	}
	return num;
}

// helper function for CFileRecord::record_from_file
static inline int read_date(const char *&cur_ptr, const char *const end)
{
	if (end - cur_ptr < 14) {
		logger->log(0, "BUGG: timestamp expected");
		return -1;
	}
	cur_ptr += 14;
	return 0;
}

// construct record from string
bool CFileRecord::from_string(const char *const begin, const char *const end, unsigned int& name_len, int& parent_id, const char **name)
{
	const char *p1, *p2;
	const char *cur_ptr;

	m_parent_addr = 0;
	if (begin == end) {
		logger->log(0, "Empty line - ignoring");
		return false;
	}
	cur_ptr = begin;
	m_flags = 0;
	// get id
	m_id = read_number(cur_ptr, end);
	if (*cur_ptr != '\t') {
		logger->log(0, "BUGG2!");
		return false;
	}
	cur_ptr++;
	// get parent_id
	parent_id = read_number(cur_ptr, end);
	if (*cur_ptr != '\t') {
		logger->log(0, "BUGG3");
		return false;
	}
	cur_ptr++;
	// get name
	p1 = cur_ptr;
	p2 = strchr(cur_ptr, '\t');
	if (p2 == 0) {
		logger->log(0, "BUGG4");
		return false;
	}
	cur_ptr = p2+1;
	name_len = p2 - p1;
	// get size
	m_size = read_number(cur_ptr, end);
	if (*cur_ptr != '\t') {
		logger->log(0, "BUGG5");
		return false;
	}
	cur_ptr++;
	// get is_dir
	m_is_dir = read_number(cur_ptr, end);
	// set name
	*name = p1;
	// get res_type
	// m_res_type = read_number(cur_ptr, end);
	m_res_type = 0;
	return true;
}

// get full path to file
const std::string path(const CFileRecord& r)
{
	std::string result(r.name());
	CRecordPtr p = r.parent();
	while (p.notNull()) {
		if (p->id() == 0) return result;
		result = std::string(p->name()) + "/" + result;
		p = p->parent();
	}
	return result;
}

void sprintf(const CFileRecord& r, char *buf)
{
	char *p = buf;
	p += ::sprintf(p, "%d\t%d\t", r.id(), r.parent_id());
	strcpy(p, r.name());
	p += strlen(r.name());
	p += ::sprintf(p, "\t%llu\t%d\t%d", r.size(), r.is_dir(), r.res_type());
}
