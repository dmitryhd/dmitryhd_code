/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: fileset.h,v 1.9 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __FILESET_H__
#define __FILESET_H__

#include <vector>
#include <ext/hash_map>
#include "record.h"
#include "memalloc.h"
#include "swapalloc.h"
#include "strheap.h"
#include "wordhash.h"
#include "wordsplit.h"

// contiguous file records set
class CFileSet
{
public:
	// constructor & destructor
	~CFileSet();

	// types
	struct WordIndexItem
	{
		int m_word;
		CRecordPtr m_record;

		inline WordIndexItem() { }
		inline WordIndexItem(int word, const swap_ptr<CFileRecord>& record) : m_word(word), m_record(record) { }

		inline int word() const { return m_word; }
		inline const CRecordPtr& record() const { return m_record; }

		inline void dec() const { wordhash.value(m_word).dec(); }

		inline bool operator< (const WordIndexItem& rhs) const { return m_word < rhs.m_word; }
		inline bool operator< (int rhs) const { return m_word < rhs; }
	};

	typedef CSwapVector<CFileRecord> record_vector;
	typedef record_vector::iterator iterator;
	typedef record_vector::const_iterator const_iterator;
	typedef std::vector<WordIndexItem> word_index;
	
	// getters
	inline int count() const { return m_leaf.size() + m_nonleaf.size(); }
	inline bool empty() const { return count() == 0; }
	// methods
	void clear();
	void load_from_file(const char *file_name);
	unsigned long long total_size() const;
	size_t mem_size() const;
	// word index
	const word_index& get_word_index() const { return m_word_index; }
	void res_type_stat(unsigned long long int *res_size) const;

	// compile-time polymorphis 
	template <typename func_type> void for_all_ptr(func_type func) const {
		const_iterator b, e;
		b = m_nonleaf.begin();
		e = m_nonleaf.end();
		const_iterator fi;
		for (fi = b; fi != e; ++fi) {
			func(fi);
		}
		m_leaf.lock();
		b = m_leaf.begin();
		e = m_leaf.end();
		for (fi = b; fi != e; ++fi) {
			func(fi);
		}
		m_leaf.unlock();
	}

	template <typename func_type> void for_all_ref(func_type func) const {
		const_iterator b, e;
		b = m_nonleaf.begin();
		e = m_nonleaf.end();
		const_iterator fi;
		for (fi = b; fi != e; ++fi) {
			func(*fi);
		}
		m_leaf.lock();
		b = m_leaf.begin();
		e = m_leaf.end();
		for (fi = b; fi != e; ++fi) {
			func(*fi);
		}
		m_leaf.unlock();
	}


private:
	void build_word_index();
	void clear_index();
	
	record_vector m_leaf;
	record_vector m_nonleaf;
	word_index m_word_index;
};

void dump(const CFileSet& fs, const char *file_name);
void dump_index(const CFileSet& fs, const char *file_name);

#endif
