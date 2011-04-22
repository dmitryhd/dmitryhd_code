/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: wordhash.h,v 1.11 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __WORDHASH_H__
#define __WORDHASH_H__

#include <vector>
#include <string>
#include "../includes.h"
#include "memalloc.h"
#include "strheap.h"
#include "hash.h"

// an index for index
class CDictWordIndex
{
public:	
	void add(int id, const char *str);
	const std::vector<int>& search_for(const char *pat) const;
	size_t mem_size() const;

private:
	std::vector<int> m_twoletter[65536];
};

class CWordInfo
{
public:
	explicit CWordInfo() : m_hits(0), m_best_substr(0), m_count(0), m_common(false) {}

	inline int count() const { return m_count; }
	inline bool is_common() const { return m_common; }
	void inc();
	void dec();

	int m_hits;
	int m_best_substr;
	
private:
	int m_count;
	bool m_common;
};

struct str_hash
{
	inline size_t operator()(const char *str) const {
		// I don't know if it is good or bad hash algorithm
		// I've just followed first link while searching 
		// "string hash function" in Google
		size_t hash = 0;
		for (const char *p = str ; *p != 0; p++) {
			hash = (hash << 6) + (hash << 16) - hash + *p;
		}
		return hash;
	}
};

struct str_eq
{
	inline bool operator()(const char *str1, const char* str2) const {
		return strcmp(str1, str2) == 0;
	}
};

typedef CHash<const char *, CWordInfo, str_hash, str_eq> CWordHash;

class CWordDict : public CWordHash
{
public:
	CWordDict(int reserve, int n_buckets);

	int index(const char* str);
	int total_words() const;
	int common_count() const;
	const CDictWordIndex& get_dictindex() const { return m_dictindex; }
	size_t mem_size() const;
	
private:
	CStringHeap<CSystemAllocPolicy> m_heap;
	CDictWordIndex m_dictindex;
};

int get_best_prefix(const CWordDict& wd, const char *str);
int get_best_substr(const CWordDict& wd, const char *str);
void recalculate_best_substr(CWordDict& wd, const CWordDict& history);

void save(const CWordDict& wd, const char *name);
void load(CWordDict& wd, const char *name);


extern CWordDict wordhash;
extern CWordDict searchhistory;

#endif
