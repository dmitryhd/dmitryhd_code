/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: qcache.cpp,v 1.10 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <vector>
#include <algorithm>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include "util.h"
#include "qcache.h"
#include "hostdata.h"
#include "hostset.h"
#include "logger.h"

#define MAX_POSSIBLE_SEARCH 100000

struct CBoolItemCompareFunc : public std::binary_function<CFoundItem,CFoundItem, bool> 
{
public:
	CBoolItemCompareFunc(const CItemCompareFunc *const function) : m_function(function) {};
	
	bool operator ()(const CFoundItem& x, const CFoundItem& y) {
		return m_function->compare(x, y) < 0;
	}

private:
	const CItemCompareFunc *const m_function;
};

struct CInverseBoolItemCompareFunc : public std::binary_function<CFoundItem,CFoundItem, bool> 
{
public:
	CInverseBoolItemCompareFunc(const CItemCompareFunc *const function) : m_function(function) {};
	
	bool operator ()(const CFoundItem& x, const CFoundItem& y) {
		return m_function->compare(x, y) > 0;
	}

private:
	const CItemCompareFunc *const m_function;
};


CCachedQuery::CCachedQuery(const std::string& a_query, const unsigned int id) : m_id(id), m_hits(0), m_query(a_query)
{
}

CCachedQuery::~CCachedQuery()
{
	// paranoia
	m_hits = 0;
	m_search_time = 0;
	m_hint_level = 1.0;
	m_id = 0;
}

size_t CCachedQuery::mem_size() const
{
	return m_result.capacity()*sizeof(CFoundItem) + m_query.length() + sizeof(CCachedQuery);
}

bool CCachedQuery::contain_host(int host_id) const
{
	std::vector<CFoundItem>::const_iterator i;
	i = m_result.begin();
	
	for (i = m_result.begin(); i != m_result.end(); i++) {
		if (i->m_host->host_id() == host_id) return true;
	}
	return false;
}

void CCachedQuery::sort()
{
	std::sort(m_result.begin(), m_result.end());
}

void CCachedQuery::sort(const CItemCompareFunc *function)
{
	if (!function->reverse) {
		std::stable_sort(m_result.begin(), m_result.end(), CBoolItemCompareFunc(function));
	} else {
		std::stable_sort(m_result.begin(), m_result.end(), CInverseBoolItemCompareFunc(function));
	}
}

CCachedQuery* CQueryCache::search(const char *query) 
{
	std::list<CCachedQuery*>::iterator i;

	for (i = m_cache.begin(); i != m_cache.end(); i++) {
		if ((*i)->is_same(query)) {
			(*i)->add_hit();
			return *i;
		}
	}
	return 0;
}

CQueryCache::CQueryCache(const CHostSet& files) : m_host_set(files)
{
	m_hits = 0;
	m_total = 0;
	m_query_made = 0;
	m_total_matched = 0;
	m_total_query_time = 0.0;
	m_max_query_time = 0;
	m_counter = 0;
	m_allow_full_scan = false;
}

CQueryCache::~CQueryCache()
{
	std::list<CCachedQuery*>::iterator i;

	for (i = m_cache.begin(); i != m_cache.end(); i++) {
		delete *i;
	}
	m_cache.clear();
}

CCachedQuery* CQueryCache::do_new_query(const char *query, int& error)
{
	error = Success;
	// parser request
	CParser *parser = new CParser();
	if (!parser->parse(query)) {
		error = SyntaxError;
		delete parser;
		return 0;
	}
	CHintedTest hinttest = parser->test();
	CRecordTest *test = hinttest.test();
	CFileHinter *hinter = hinttest.hinter();
	CHostTest *host_test = parser->host_test();
	std::list<CItemCompareFunc*> sort_list = parser->sort_list();
	delete parser;
	struct timeval t1, t2;
	long page_reads = page_read_count;
	gettimeofday(&t1, NULL);
	// generate a new unique id for the new query
	m_counter++;
	// create cached query item and fill it 
	CCachedQuery *new_query = new CCachedQuery(query, m_counter);
	// initialize hinter
	hinter->init();
 	gettimeofday(&t2, NULL);
	double hint_time = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1000000.0;
	if (hint_time > 0.05) logger->log(2, "Hint building time %.3f sec (%s)", hint_time, query);
	new_query->set_hint_level(hinter->level());
	// don't use hinter if it has too large level
	if (hinter->level() > 0.005) {
		delete hinter;
		hinter = 0;
	}
	// if we cannot find hint :(
	if (hinter == 0) {
		logger->log(2, "Query without hint: %s", query);
		CHostSet::const_iterator hi;
		int host_count = 0;
		for (hi = m_host_set.begin(); hi != m_host_set.end(); hi++) {
			if (host_test->test(*(hi->second))) host_count++;
		}
		if ((host_count > m_host_set.host_count() / 10) && !m_allow_full_scan) {
			error = NoHintFound;
			delete test;
			delete host_test;
			return 0;
		}
	}

	// get hosts
	CHostSet::const_iterator hi;
	for (hi = m_host_set.begin(); hi != m_host_set.end(); hi++) {
		const CHostData* hp = hi->second;
		if (!host_test->test(*hp)) continue;
		const CFileSet& fs = hp->fileset();
		test->reset(fs);
		if (!hinter) { 
			// query without hint
			Tester t;
			t.test = test;
			t.query = new_query;
			t.hostdata = hp;

			fs.for_all_ptr(t);
		} else {
			// query with hint
			CHint hint = hinter->hint(fs);
			std::vector<CRecordPtr> hvec = hint.data();
			// sort and remove duplicates from hint
			std::sort(hvec.begin(), hvec.end());
			std::vector<CRecordPtr>::iterator new_end = std::unique(hvec.begin(), hvec.end());
			hvec.erase(new_end, hvec.end());
			std::vector<CRecordPtr>::const_iterator b, e;
			std::vector<CRecordPtr>::const_iterator ri;
			b = hvec.begin();
			e = hvec.end();
			for (ri = b; ri != e; ri++) {
				const CFileRecord& r = **ri;
				if (test->test(r)) {
					new_query->append(CFoundItem(*ri, hp));
				}
			}
		}
	}
	if (hinter) delete hinter;
	delete test;
	delete host_test;
	gettimeofday(&t2, NULL);
	page_reads = page_read_count - page_reads;
	double search_time = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1000000.0;
	logger->log(3, "Search time %.3f sec, cause %lu page reads (%s)", search_time, page_reads, query);
	new_query->finish_query(search_time);
	if (sort_list.empty()) {
		// sort results
		new_query->sort();
	} else {
		// do sorting in reverse order
		std::list<CItemCompareFunc*>::reverse_iterator cmp;
		for (cmp = sort_list.rbegin(); cmp != sort_list.rend(); cmp++) {
			new_query->sort(*cmp);
		}
		// clean up
		std::list<CItemCompareFunc*>::iterator it;
		for (it = sort_list.begin(); it != sort_list.end(); it++) {
			delete *it;
			*it = 0;
		}
		sort_list.clear();
	}
	return new_query;
}

CCachedQuery* CQueryCache::do_query(const char *query, int& error)
{
	error = Success;
	m_total++;
	CCachedQuery* cached = search(query);
	if (cached) {
		m_hits++;
		m_total_matched += cached->count();
		return cached;
	}
	CCachedQuery *new_query = do_new_query(query, error);
	if (new_query == 0) {
		logger->log(1, "Invalid search query");
		return 0;
	}
	m_cache.push_front(new_query);
	m_query_made++;
	if (new_query->search_time() > m_max_query_time) {
		m_max_query_time = new_query->search_time();
	}
	m_total_query_time += new_query->search_time();
	m_total_matched += new_query->count();
	return new_query;
}

void CQueryCache::invalidate_host(int host_id)
{
	std::list<CCachedQuery*>::iterator i, j;

	i = m_cache.begin();
	while (i != m_cache.end()) {
		j = i;
		j++;
		if ((*i)->contain_host(host_id)) {
			delete *i;
			m_cache.erase(i);
		}
		i = j;
	} 
}

void CQueryCache::invalidate_all()
{
	std::list<CCachedQuery*>::iterator i;

	for (i = m_cache.begin(); i != m_cache.end(); i++) {
		delete *i;
	}
	m_cache.clear();
}

size_t CQueryCache::mem_size() const
{
	return sum_func(m_cache.begin(), m_cache.end(), std::mem_fun(&CCachedQuery::mem_size));
}

double CQueryCache::avg_search_time() const 
{
	if (m_query_made == 0) return 0.0;
	return m_total_query_time / m_query_made;
}

void CQueryCache::clean_up()
{
	// FIXME : write cache cleaner here!!!
}
