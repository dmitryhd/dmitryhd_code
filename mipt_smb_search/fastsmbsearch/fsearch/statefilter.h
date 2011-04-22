/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: statefilter.h,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __STATEFILTER_H__
#define __STATEFILTER_H__

#include <vector>
#include "record.h"
#include "test.h"

template <typename State> class CStateCacheItem
{
public:
	inline CStateCacheItem() { }
	inline CStateCacheItem(State st, int id) { m_state = st; m_id = id; }
	
	inline State state() const { return m_state; }
	inline int id() const { return m_id; }
	inline void set_id(int id) { m_id = id; }

private:
	State m_state;
	int m_id;
};

template <typename State> class CStateCache
{
public:
	inline CStateCache() { clear(); }
	
	inline CStateCacheItem<State> last() const { return m_last; }
	inline bool empty() const { return m_cache.empty(); }
	inline void clear() { m_cache.clear(); m_last.set_id(-1); }
	inline void add(const CStateCacheItem<State> &item) { m_cache.push_back(item); m_last = item; }
	void remove_bigger(int id);

private:
	std::vector<CStateCacheItem<State> > m_cache;
	CStateCacheItem<State> m_last;
};


template <typename CStateFilter> class CPathMatchFilter : public CRecordTest
{
public:
	typedef typename CStateFilter::State State;

	CPathMatchFilter(CStateFilter *filter) { m_filter = filter; }
	virtual ~CPathMatchFilter();
	
	virtual bool test(const CFileRecord &r);
	virtual bool is_valid() const { return m_filter->is_valid(); }
	virtual void reset(const CFileSet& fs_) { m_cache.clear(); m_filter->reset(fs_); fs = &fs_; }

private:

	State search_with_update(const CFileRecord& r);

	const CFileSet* fs;
	CStateFilter *m_filter;
	CStateCache<State> m_cache;
};

template <typename State> void CStateCache<State>::remove_bigger(int id)
{
	// check for empty
	if (m_last.id() < 0) return;
	while (m_last.id() > id) {
		// the first item is record with zero id
		// we need not check for i to go before begin
		m_cache.pop_back();
		m_last = m_cache.back();
	}
}

template <typename StateFilter> CPathMatchFilter<StateFilter>::~CPathMatchFilter()
{
	delete m_filter;
	m_filter = 0;
}

template <typename StateFilter> typename StateFilter::State CPathMatchFilter<StateFilter>::search_with_update(const CFileRecord& r)
{
	int id = r.id();
	if (m_cache.last().id() > id) {
		m_cache.remove_bigger(id);
	}
	if (m_cache.last().id() == id) return m_cache.last().state();

	State state;
	CRecordPtr p = r.parent();
	if (p.notNull()) {
		state = search_with_update(*p);
	} else {
		state = m_filter->empty_state();
	}
	state = m_filter->add_item(state, r);
	m_cache.add(CStateCacheItem<State>(state, id));
	return state;
}

template <typename State> bool CPathMatchFilter<State>::test(const CFileRecord& r)
{
	State state;
	if (r.id() != 0) {
		// most frequently used path: can avoid func call
		CRecordPtr p = r.parent();
		if (m_cache.last().id() == p->id()) {
			state = m_cache.last().state();
		} else {
			state = search_with_update(*p);
		}
		if (m_filter->is_full(state)) return false;
	} else {
		state = m_filter->empty_state();
	}
	return m_filter->is_match(state, r);
}

class CBitAndStateFilter
{	
public:
	typedef unsigned int State;

	explicit CBitAndStateFilter(const std::vector<CRecordTest*>& tests);
	~CBitAndStateFilter();
	
	inline State empty_state() const { return 0; }
	inline bool is_full(State state) const { return state == m_full_state; }
	inline State add_item(State state, const CFileRecord& r);
	inline bool is_match(State state, const CFileRecord& r); // can be optimized version of previous
	bool is_valid() const;
	void reset(const CFileSet& fs);

private:
	std::vector<CRecordTest*> m_tests;
	State m_full_state;
};

CBitAndStateFilter::CBitAndStateFilter(const std::vector<CRecordTest*>& tests) : m_tests(tests)
{
	int test_count = tests.size();
	m_full_state = 0;
	if ((test_count < 1) || (test_count > 30)) return;
	m_full_state = (1 << test_count) - 1;
}

bool CBitAndStateFilter::is_valid() const
{
	// check count of tests
	if (m_full_state == 0) return false;
	std::vector<CRecordTest *>::const_iterator i;
	for (i = m_tests.begin(); i != m_tests.end(); i++) {
		if (!(*i)->is_valid()) return false;
	}
	return true;
}

CBitAndStateFilter::~CBitAndStateFilter()
{
	std::vector<CRecordTest *>::iterator i;
	for (i = m_tests.begin(); i != m_tests.end(); i++) {
		delete *i;
		*i = 0;
	}
	m_tests.clear();
}

void CBitAndStateFilter::reset(const CFileSet& fs)
{
	std::vector<CRecordTest *>::iterator i;
	for (i = m_tests.begin(); i != m_tests.end(); i++) {
		(*i)->reset(fs);
	}
}

inline CBitAndStateFilter::State CBitAndStateFilter::add_item(State state, const CFileRecord& r)
{
	State mask = 1;
	// optimization: check only test that we hadn't already satisfied
	std::vector<CRecordTest *>::iterator i;
	std::vector<CRecordTest *>::iterator e = m_tests.end();
	for (i = m_tests.begin(); i != e; i++) {
		if (!(state & mask)) {
			if ((*i)->test(r)) state |= mask;
		}
		mask <<= 1;
	}
	return state;
}

inline bool CBitAndStateFilter::is_match(State state, const CFileRecord& r)
{
	State mask = 1;
	// optimization: check only test that we hadn't already satisfied
	std::vector<CRecordTest *>::iterator i;
	std::vector<CRecordTest *>::iterator e = m_tests.end();
	for (i = m_tests.begin(); i != e; i++) {
		if (!(state & mask)) {
			if (!(*i)->test(r)) return false;
		}
		mask <<= 1;
	}
	return true;
}

#endif
