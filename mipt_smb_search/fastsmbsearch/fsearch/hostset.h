/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: hostset.h,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __NETDATA_H__
#define __NETDATA_H__

#include <map>
#include "record.h"
#include "hostdata.h"

// collection of hosts
class CHostSet
{
public:
	// constructor and destructor
	~CHostSet();

	// types
	typedef std::map<int, CHostData*> storage;
	typedef storage::iterator iterator;
	typedef storage::const_iterator const_iterator;

	int host_count() const { return m_hosts.size(); }
	int file_count() const;
	CHostData* find_host(int host_id);
	void delete_host(int host_id);
	int max_id() const;
	void clear();
	size_t mem_size() const;
	unsigned long long total_size() const;
	void update_hosts(const char *file_name);
	const_iterator begin() const { return m_hosts.begin(); }
	const_iterator end() const { return m_hosts.end(); }

private:
	void calc_priorities();
	
	storage m_hosts;
};

#endif
