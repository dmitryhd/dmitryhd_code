/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: strheap.h,v 1.8 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __STRHEAP_H__
#define __STRHEAP_H__

#include <list>
#include <vector>
#include <cassert>
#include "util.h"
#include "logger.h"

#define STRING_BLOCK_SIZE 4096

template <typename Allocator> class CDataBlock
{
public:
	explicit CDataBlock(int base_size) {
		int alloc_size = (base_size / STRING_BLOCK_SIZE) * STRING_BLOCK_SIZE;
		if (alloc_size < base_size) alloc_size += STRING_BLOCK_SIZE;
		m_begin = (char*)Allocator::alloc(alloc_size);
		m_end = m_begin;
		m_alloc_end = m_end + alloc_size;
	}
	~CDataBlock() {
		Allocator::free(m_begin);
		m_begin = 0;
		m_end = 0;
	}
		
	// getters
	inline bool empty() const { return m_end == m_begin; };
	inline int size() const { return m_end - m_begin; };
	inline int free_space() const { return m_alloc_end - m_end; };
	inline int capacity() const { return m_alloc_end - m_begin; };
	inline size_t mem_size() const { return capacity() + sizeof(CDataBlock); };
	// methods
	void *add(int len, const void *data) {
		char *ptr = m_end;
		m_end += len;
		assert(m_end <= m_alloc_end);
		memcpy(ptr, data, len);
		return ptr;
	}
	void flush() {
		Allocator::flush(m_begin, m_end - m_begin);
	}
	
private:
	char *m_begin;
	char *m_end;
	char *m_alloc_end;

	// disable copy constructor
	CDataBlock(const CDataBlock& block);
};

template <typename Allocator> class CStringBlock : public CDataBlock<Allocator>
{
public:
	CStringBlock(int base_size = STRING_BLOCK_SIZE) : CDataBlock<Allocator>(base_size) { }
	// methods
	char *append(int name_len, const char *str) {
		char null = 0;
		char *ptr = static_cast<char*>(this->add(name_len, str));
		this->add(1, &null);
		return ptr;
	}
};
	
template <typename Allocator> class CStringHeap
{
public:
	// constructors & destructors
	explicit CStringHeap() {	m_last_block = 0; }
	~CStringHeap() { clear(); }

	// getters
	inline bool empty() const { return m_data.empty(); };
	inline int num_blocks() const { return m_data.size(); }
	// methods
	char *append(int name_len, const char *string) {
		if (m_last_block == 0) add_block();
		int space = name_len + 1;
		if (space > m_last_block->free_space()) {
			add_block();
			if (space > m_last_block->free_space()) {
				logger->log(0, "Very long string!");
				return 0;
			}
		}
		return m_last_block->append(name_len, string);
	}
	void flush() {
		typename std::list<CStringBlock<Allocator> *>::iterator i;
		for (i = m_data.begin(); i != m_data.end(); i++) {
			(*i)->flush();
		}
	}
	void clear() {
		typename std::list<CStringBlock<Allocator> *>::iterator i;
		for (i = m_data.begin(); i != m_data.end(); i++) {
			delete *i;
		}
		m_data.clear();
	}
	void reserve(int size) {
		m_last_block = new CStringBlock<Allocator>(size);
		m_data.push_back(m_last_block);
	}
	size_t mem_size() const {
		return sum_func(m_data.begin(), m_data.end(), std::mem_fun(&CStringBlock<Allocator>::mem_size));
	}
	
private:
	CStringHeap(const CStringHeap& heap);
	void add_block() {
		m_last_block = new CStringBlock<Allocator>();
		m_data.push_back(m_last_block);
	}
	std::list<CStringBlock<Allocator> *> m_data;
	CStringBlock<Allocator> *m_last_block;
};

#endif
