/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: hash.h,v 1.1 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __HASH_H__
#define __HASH_H__

#include <vector>
#include <utility>

template <typename key_type, typename data_type, typename hash_fun, typename eq_key> class CHash
{
private:
	// FIXME: cannot use const key_type here due to std::vector constraints
	typedef std::pair<key_type, data_type> value_type;
	struct CHashItem : public value_type {
		int m_next;		// linked list for hash table
	};

	eq_key equal_obj;
	hash_fun hash_obj;
	const int buckets;

public:
	typedef unsigned long hash_t;
	typedef std::vector<CHashItem> itemarray;
	typedef typename itemarray::iterator iterator; 
	typedef typename itemarray::const_iterator const_iterator;

	CHash(int reserve, int n_buckets) : buckets(n_buckets), m_count(0) {
		m_id_index.reserve(reserve);
		m_id_index.push_back(CHashItem());
		m_hash_entry.resize(buckets);
	}
	inline int size() const { return m_count; }
	const key_type& key(int id) const { return m_id_index[id].first; }
	const data_type& value(int id) const { return m_id_index[id].second; }
	data_type& value(int id) { return m_id_index[id].second; }
	int find(const key_type& key) const {
		// get existing record or create new
		int id = m_hash_entry[hash_obj(key) % buckets];
		// walk through list
		while (id != 0) {
			const CHashItem& item = m_id_index[id];
			if (equal_obj(item.first, key)) return id;
			id = item.m_next;
		}
		return 0;
	}
	int append(const key_type& key, const data_type& value) {
		const hash_t hash = hash_obj(key) % buckets;
		// set id for new word
		m_count++;
		CHashItem item;
		item.first = key;
		item.second = value;
		item.m_next = m_hash_entry[hash];
		m_id_index.push_back(item);
		m_hash_entry[hash] = m_count;
		return m_count;
	}
	const_iterator begin() const { const_iterator p = m_id_index.begin(); p++; return p; }
	const_iterator end() const { return m_id_index.end(); }
	iterator begin() { iterator p = m_id_index.begin(); p++; return p; }
	iterator end() { return m_id_index.end(); }
	int max_collisions() const {
		int max = 0;
		for (int start = 0; start < m_count; start++) {
			int id = start;
			int len = 0;
			while (id != 0) {
				id = m_id_index[id].m_next;
				len++;
			}
			if (len > max) max = len;
		}
		return max;
	}
	size_t mem_size() const {
		return sizeof(CHash) + m_id_index.capacity() * sizeof(value_type) +
			m_hash_entry.capacity() * sizeof(int);
	}

private:
	itemarray m_id_index;
	std::vector<int> m_hash_entry;
	int m_count;
};

#endif
