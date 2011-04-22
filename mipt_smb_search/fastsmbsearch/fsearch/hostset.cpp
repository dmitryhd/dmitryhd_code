/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: hostset.cpp,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <functional>
#include "util.h"
#include "record.h"
#include "fileread.h"
#include "logger.h"
#include "hostset.h"

int CHostSet::file_count() const
{
	return sum_key_value_func(m_hosts.begin(), m_hosts.end(), std::mem_fun(&CHostData::count));
}

size_t CHostSet::mem_size() const
{
	return sum_key_value_func(m_hosts.begin(), m_hosts.end(), std::mem_fun(&CHostData::mem_size));
}

void CHostSet::clear()
{
	storage::iterator i;

	for (i = m_hosts.begin(); i != m_hosts.end(); i++)
	{
		delete i->second;
		i->second = 0;
	}
	m_hosts.clear();
}

CHostSet::~CHostSet()
{
	clear();
}

CHostData* CHostSet::find_host(int host_id)
{
	storage::iterator i;
	i = m_hosts.find(host_id);
	if (i == m_hosts.end()) return 0;
	return i->second;
}

void CHostSet::delete_host(int host_id)
{
	storage::iterator i;
	i = m_hosts.find(host_id);
	if (i == m_hosts.end()) return;
	delete i->second;
	i->second = 0;
	m_hosts.erase(i);
}

struct host_name_cmp
{
	bool operator()(const CHostData* p1, const CHostData* p2) const
	{
		return strcmp(p1->name(), p2->name()) < 0;
	}
};

struct host_status_cmp
{
	bool operator()(const CHostData* p1, const CHostData* p2) const
	{
		return p1->status_prio() < p2->status_prio();
	}
};

void CHostSet::calc_priorities()
{
	std::vector<CHostData*> tmp;
	tmp.reserve(m_hosts.size());

	storage::iterator i;
	for (i = m_hosts.begin(); i != m_hosts.end(); i++) {
		tmp.push_back(i->second);
	}
	std::sort(tmp.begin(), tmp.end(), host_name_cmp());
	std::stable_sort(tmp.begin(), tmp.end(), host_status_cmp());

	int num = 0;
	std::vector<CHostData*>::iterator vi;
	for (vi = tmp.begin(); vi != tmp.end(); vi++)
	{
		m_hosts[(*vi)->host_id()]->set_priority(num);
		num++;
	}
}

void CHostSet::update_hosts(const char *file_name)
{
	int count = 0;
	CFileReader reader(file_name);
	int host_id;
	
	while (reader.get_next()) {
		const char *begin = reader.begin();
		const char *end = reader.end();
		// skip empty lines
		if (end == begin) continue;
		CHostData t_host(begin, end);
		// get host id
		host_id = t_host.host_id();
		// find host
		CHostData *host = m_hosts[host_id];
		// std::map automatically creates new entry if key is not found
		if (host == 0) {
			// add new if not exist
			logger->log(2, "Adding new host %d", host_id);
			host = new CHostData(begin, end);
			m_hosts[host_id] = host;
		}
		// update values
		host->from_string(begin, end);
		count++;
	}
	reader.close();
	calc_priorities();
}
	
unsigned long long CHostSet::total_size() const
{
	return sum_key_value_func(m_hosts.begin(), m_hosts.end(), std::mem_fun(&CHostData::total_size));
}
