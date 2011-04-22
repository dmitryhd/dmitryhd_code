/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: qcache.h,v 1.7 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __QCACHE_H__
#define __QCACHE_H__

#include <vector>
#include <list>
#include <string>
#include "record.h"
#include "hostdata.h"
#include "hostset.h"
#include "parser.h"
#include "cacheitem.h"

class CCachedQuery
{
public:
	explicit CCachedQuery(const std::string& a_query, const unsigned int id);
	~CCachedQuery();

	const std::string& query() const { return m_query; };
	int count() const { return m_result.size(); };
	unsigned int id() const { return m_id; }
	double search_time() const { return m_search_time; }
	double hint_level() const { return m_hint_level; }
	const std::vector<CFoundItem> &get_result() { return m_result; };
	int hits() const { return m_hits; };
	bool is_same(const std::string& query) const { return m_query == query; }
	void append(const CFoundItem& r) { m_result.push_back(r); };
	void finish_query(double search_time) { m_search_time = search_time; }
	void sort();
	void sort(const CItemCompareFunc *compare);
	void add_hit() { m_hits++; };
	void set_hint_level(const double hint_level) { m_hint_level = hint_level; }
	bool contain_host(int host_id) const;
	size_t mem_size() const;

private:
	unsigned int m_id;
	int m_hits;
	double m_search_time;
	double m_hint_level;
	const std::string m_query;
	std::vector<CFoundItem> m_result;
};

class CQueryCache
{
public:
	enum {
		Success = 0,
		SyntaxError = 1,
		NoHintFound = 2
	};
	
	explicit CQueryCache(const CHostSet& files);
	~CQueryCache();

	int count() const { return m_cache.size(); };
	int hits() const { return m_hits; };
	int total() const { return m_total; };
	int query_made() const { return m_query_made; };
	CCachedQuery* do_query(const char *query, int& error);
	void invalidate_host(int host_id);
	void invalidate_all();
	void clean_up();
	size_t mem_size() const;
	double avg_search_time() const;
	double max_search_time() const { return m_max_query_time; }
	long long total_matched() const { return m_total_matched; }
	void enable_full_scan(bool enable) { m_allow_full_scan = enable; }
	bool full_scan_enabled() const { return m_allow_full_scan; }

private:
	// for using in for-each -like methods
	struct Tester {
		CRecordTest *test;
		const CHostData* hostdata;
		CCachedQuery *query;

		void operator ()(const CRecordPtr& p) {
			if (test->test(*p)) {
				query->append(CFoundItem(p, hostdata));
			}
		}
	};
	
	CCachedQuery* search(const char *query);
	CCachedQuery* do_new_query(const char *query, int& error);

	std::list<CCachedQuery*> m_cache;
	const CHostSet& m_host_set;
	int m_hits;
	int m_total;
	int m_query_made;
	long long m_total_matched;
	double m_total_query_time;
	double m_max_query_time;
	unsigned int m_counter;
	bool m_allow_full_scan;
};

#endif
