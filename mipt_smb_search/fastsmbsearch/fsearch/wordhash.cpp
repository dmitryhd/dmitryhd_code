/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: wordhash.cpp,v 1.11 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <fstream>
#include <functional>
#include <algorithm>
#include "util.h"
#include "wordhash.h"
#include "logger.h"

// if words appear more frequently than limit
// it considered to be a common (e.g. 'the')
static const int common_limit = 30000;

void CDictWordIndex::add(int id, const char *str)
{
	const int l = strlen(str);
	if (l < 2) return;
	for (int i = 0; i < l - 1; i++) {
		int m = ((unsigned char)str[i] << 8) + (unsigned char)str[i+1];
		std::vector<int>& idx = m_twoletter[m];
		// manually control expansion
		if (idx.size() == idx.capacity()) {
			idx.reserve(idx.size() + idx.size() / 3);
		}
		idx.push_back(id);
	}
}

const std::vector<int>& CDictWordIndex::search_for(const char *pat) const
{
	const int l = strlen(pat);
	if (l < 2) return *(std::vector<int>*)0;
	int best_m = 0;
	int best_size = 0;
	for (int i = 0; i < l - 1; i++) {
		int m = ((unsigned char)pat[i] << 8) + (unsigned char)pat[i+1];
		int size = m_twoletter[m].size();
		if ((i == 0) || (size < best_size)) {
			best_m = m;
			best_size = size;
		}
	}
	return m_twoletter[best_m];
}

size_t CDictWordIndex::mem_size() const
{
	return sizeof(CDictWordIndex) + 65536 * sizeof(std::vector<int>) +
		sizeof(int) * sum_func(m_twoletter, m_twoletter + 65536, std::mem_fun_ref(&std::vector<int>::capacity));
}

void CWordInfo::inc()
{
	m_count++;
	if (m_count > common_limit) m_common = true;
}

void CWordInfo::dec()
{
	m_count--;
}

CWordDict::CWordDict(int reserve, int n_buckets) : CWordHash(reserve, n_buckets)
{
	// initial string heap size
	m_heap.reserve(10 * reserve);
}

struct wordinfo_is_common_t : public std::unary_function<std::pair<const char*, CWordInfo>, int>
{
	inline bool operator()(const std::pair<const char *, CWordInfo>& p) const {
		return p.second.is_common();
	}
};

int CWordDict::total_words() const
{
	return sum_key_value_func(begin(), end(), std::mem_fun_ref(&CWordInfo::count));
}

int CWordDict::common_count() const
{
	wordinfo_is_common_t c;
	return count_if(begin(), end(), c);
}

size_t CWordDict::mem_size() const
{
	return CWordHash::mem_size() + m_heap.mem_size() + m_dictindex.mem_size();
}

int CWordDict::index(const char *str)
{
	int id = find(str);
	if (id > 0) return id;
	// set id for new word
	CWordInfo info;
	const char *c_str = m_heap.append(strlen(str), str);
	int result = append(c_str, info);
	m_dictindex.add(result, c_str);
	return result;
}
	
void save(const CWordDict& wd, const char *name)
{
	std::ofstream f(name);
	if (!f) {
		logger->log(1, "Cannot open wordhash dump file %s for writing", name);
		return;
	}
	for (CWordDict::const_iterator i = wd.begin(); i != wd.end(); i++) {
		const char *str = i->first;
		const CWordInfo& info = i->second;
		f << str << '\t' << (info.is_common() ? "true" : "false") << '\t' << info.count()
		  << '\t' << info.m_hits << '\t' << info.m_best_substr << '\n';
	}
	f.close();
}

void load(CWordDict& wd, const char *name)
{
	std::ifstream f(name);
	if (!f) {
		logger->log(1, "Cannot open wordhash dump file %s for reading", name);
		return;
	}
	std::string s;
	std::string common;
	std::string beststr;
	int count, hits;
	while (!f.eof()) {
		f >> s >> common >> count >> hits >> beststr;
		if (s.length() > 1) {
			wd.value(wd.index(s.c_str())).m_hits = hits;
		}
	}
}

int get_best_prefix(const CWordDict& wd, const char *s)
{
	int length = strlen(s);
	// this function is called very often
	// avoid memory-allocation here
	static char buf[4096];
	// FIXME
	if (length > 1024) return 0;
	memset(buf, 0, length + 1);
	char *substr = buf;
	const char *p = s;
	int len = 0;
	int best_hits = 0;
	int best = 0;
	while (*p != 0) {
		*substr = *p;
		len++;
		p++;
		substr++;
		if (len < 3) continue;
		int id = wd.find(buf);
		if (id == 0) continue;
		int hits = wd.value(id).m_hits;
		if (hits > best_hits) {
			best_hits = hits;
			best = id;
		}
	}
	return best;
}

int get_best_substr(const CWordDict& wd, const char *str)
{
	const char *p = str;
	int best_hits = 0;
	int best = 0;
	while (*p != 0) {
		int id = get_best_prefix(wd, p);
		p++;
		if (id == 0) continue;
		int hits = wd.value(id).m_hits;
		if (hits > best_hits) {
			best_hits = hits;
			best = id;
		}
	}
	return best;
}

void recalculate_best_substr(CWordDict& wd, const CWordDict& history)
{
	for (CWordDict::iterator i = wd.begin(); i != wd.end(); i++) {
		i->second.m_best_substr = get_best_substr(history, i->first);
	}
}

// number of words to reserve at start
static const int wordhash_reserve = 800000;
static const int history_reserve = 70000;

// number of buckets for hash
// just a large prime number
static const int wordhash_buckets = 1572869;
static const int history_buckets = 98317;

CWordDict wordhash(wordhash_reserve, wordhash_buckets);
CWordDict searchhistory(history_reserve, history_buckets);
