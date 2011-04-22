/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: hostdata.h,v 1.6 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __HOSTDATA_H__
#define __HOSTDATA_H__

#include <string>
#include <vector>
#include "record.h"
#include "fileset.h"

#define STATUS_ONLINE		0
#define STATUS_UNCHECKED	1
#define STATUS_ERROR		2
#define STATUS_OFFLINE		3

// information about one host
class CHostData
{
public:
	// constructor & destructor
	explicit CHostData(const char *const begin, const char *const end) { from_string(begin, end); }

	// types
	typedef CFileSet::iterator iterator;
	typedef CFileSet::const_iterator const_iterator;
	// getters
	int count() const { return fileset().count(); }
	int host_id() const { return m_host_id; }
	int proto() const { return m_proto; }
	const char* status() const { return m_status; }
	int status_prio() const { return m_status_prio; }
	bool is_online() const { return m_status_prio != STATUS_OFFLINE; }
	int priority() const { return m_priority; }
	int wkg_id() const { return m_wkg; }
	int share_count() const { return m_share_count; }
	const char *name() const { return m_name; }
	const char *ip() const { return m_ip; }
	CFileSet& fileset() { return m_files; }
	const CFileSet& fileset() const { return m_files; }
	// modifiers
	void set_status(const char *a_status);
	void set_priority(int a_priority) { m_priority = a_priority; }
	void from_string(const char *const begin, const char *const end); 
	// methods
	unsigned long long total_size() const { return m_files.total_size(); }
	// memory size
	size_t mem_size() const;

private:
	int m_host_id;
	int m_proto;
	int m_priority;
	char m_status[32];
	int m_status_prio;
	int m_wkg;
	int m_share_count;
	char m_name[32];
	char m_ip[20];
	CFileSet m_files;
	CHostData(const CHostData&);
};

// print content 
std::string sprintf(const CHostData&);

#endif
