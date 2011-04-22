/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: record.h,v 1.6 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __RECORD_H__
#define __RECORD_H__

#include <cstdio>
#include <string>
#include "swapalloc.h"

class CFileRecord;

typedef swap_ptr<CFileRecord> CRecordPtr;

// record about one file
class CFileRecord
{
public:
	// getters
	int id() const { return m_id; };
	CRecordPtr parent() const { return CRecordPtr(m_parent_addr); };
	int parent_id() const { return (m_parent_addr == 0) ? 0 : parent()->id(); }
	char *name() { return m_name; };
	const char *name() const { return m_name; };
	unsigned char is_dir() const { return m_is_dir; };
	long long unsigned int size() const { return m_size; };
	unsigned char res_type() const { return m_res_type; };
	inline bool is_last() const { return m_flags; }
	// modifiers
	inline int mem_size() const { return sizeof(CFileRecord) - 1 + strlen(m_name) + 1; }
	inline void set_last(bool flag) { m_flags = flag ? 1 : 0; }
	inline void set_parent(const CRecordPtr a_parent) { m_parent_addr = a_parent.get_mem_addr(); };
	inline void reset_size() { m_size = 0; }
	inline void add_size(long long unsigned int size) { m_size += size; };
	// parser string and fill fields accordingly
	bool from_string(const char *begin, const char *end, unsigned int& name_len, int& parent_id, const char **name);
	// comparison
	inline bool operator < (const CFileRecord& b) const { return m_id < b.m_id; };
	inline bool operator < (int b) const { return m_id < b; };
	void classify();
	
private:
	int m_id;
	memaddr_t m_parent_addr;
	long long unsigned int m_size;
	unsigned char m_is_dir : 1;
	unsigned char m_res_type : 5;
	unsigned char m_flags : 1;
	char m_name[1];
} __attribute__ ((packed));

// any buffer that are used for storing string representation
// of a file buffer must have at least FILE_RECORD_STRING_LEN chars
// otherwise a buffer overflow can occur!
#define FILE_RECORD_STRING_LEN 400

const std::string path(const CFileRecord& r);
// print content to buffer (which must have at least FILE_RECORD_STRING_LEN space)
void sprintf(const CFileRecord& r, char *buf);

#endif
