/* This file is part of Fast SMB Search
 *
 * Copyright 2001, 02, 03, 04   Alexander Vodomerov
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

#ifndef __SWAPALLOC_H__
#define __SWAPALLOC_H__

#include "../includes.h"
#include "memalloc.h"
#include "swapfile.h"

// swap-based allocator
class CSwapAllocator
{
public:
	explicit CSwapAllocator(const std::string& name, long size);
	~CSwapAllocator();

	inline memaddr_t alloc(long bytes) { return m_blockAllocator.alloc(bytes); }
	inline void free(memaddr_t ptr) { m_blockAllocator.free(ptr); }
	inline void read(void *buf, long offset, long size) { m_swapFile.read(buf, offset, size); }
	inline void write(void *buf, long offset, long size) { m_swapFile.write(buf, offset, size); }
	bool check() { return m_blockAllocator.check(); }
	long allocated() { return m_blockAllocator.totalSize(); }
	long memUsed() { return m_blockAllocator.memUsed(); }
	void debug_dump() const { m_blockAllocator.debug_dump(); }
private:
	CSwapFile m_swapFile;
	CBlockAllocator m_blockAllocator;
};

// singleton class
class CSingleSwapAllocator
{
public:
	inline CSwapAllocator* operator -> () { return m_instance; }

	void init(const std::string& name, long size, long max_rss);
	void finish();

private:
	CSwapAllocator* m_instance;
};

// global variable
extern CSingleSwapAllocator swapAllocator;

const int page_size = 4096;
const int page_shift = 12;

struct page_desc {
	void *addr;
	int usage;
	int flags;
};

extern unsigned int page_count;
extern long access_count;
extern long miss_count;
extern long page_read_count;
extern long page_swap_count;
extern unsigned int page_present;
extern unsigned int page_max;
extern unsigned int page_locked;

extern void* load_page(unsigned int pageno);
extern void clear_pages(unsigned int pagebegin, unsigned int pageend);
extern void lock_pages(unsigned int pagebegin, unsigned int pageend);
extern void unlock_pages(unsigned int pagebegin, unsigned int pageend);
extern void swapout_pages(unsigned int pagebegin, unsigned int pageend);
extern void enable_write_pages(unsigned int pagebegin, unsigned int pageend);
extern void disable_write_pages(unsigned int pagebegin, unsigned int pageend);
extern void clear_page_cache();
extern page_desc *page_table;

extern void do_page_error(unsigned int pageno);

inline void* get_page_addr(unsigned int pageno)
{
        if (pageno >= page_count) {
		do_page_error(pageno);
	}
	page_table[pageno].usage++;
	if (page_table[pageno].addr) return page_table[pageno].addr;
	return load_page(pageno);
}

template <typename T> class swap_ptr
{
public:
	explicit swap_ptr() : m_offset(0) { }
	explicit swap_ptr(memaddr_t offset) : m_offset(offset) { }

	swap_ptr(const swap_ptr& p) : m_offset(p.m_offset) { }

        swap_ptr& operator = (const swap_ptr& p) {
		m_offset = p.m_offset;
		return *this;
	}

	void reset() { m_offset = 0; }
	memaddr_t get_mem_addr() const { return m_offset; }
	int get_page() const { return m_offset >> page_shift; }
	int get_page_offset() const { return m_offset & (page_size - 1); }

	bool notNull() const { return m_offset != 0; }
	inline bool operator == (const swap_ptr& rhs) const { return m_offset == rhs.m_offset; }
	inline bool operator != (const swap_ptr& rhs) const { return m_offset != rhs.m_offset; }

	inline bool operator < (const swap_ptr& rhs) const { return m_offset < rhs.m_offset; }
	inline bool operator <= (const swap_ptr& rhs) const { return m_offset <= rhs.m_offset; }
	size_t operator - (const swap_ptr& rhs) const { return m_offset - rhs.m_offset; }
	inline void advance(int bytes) { m_offset = m_offset + bytes; }

	inline swap_ptr& operator ++ () {
		T* val = deref();
		advance(val->mem_size());
		if (val->is_last()) {
			m_offset = (m_offset & ~(page_size-1)) + page_size;
		}
		return *this;
	}

	T* deref() const {
		return reinterpret_cast<T*>((char*)get_page_addr(m_offset >> page_shift) + (m_offset & (page_size - 1)));
	};
	T* operator -> () const { return deref(); }
	T& operator * () const { return *deref(); }	
	
private:
	memaddr_t m_offset;
};

class CSwapVectorEstimator
{
public:
	explicit CSwapVectorEstimator() : m_count(0), m_used(0) { }

	void add(int rec_size) {
		m_used += rec_size;
		if (m_used > page_size) {
			m_used = rec_size;
			m_count++;
		}
	}

	int finish() {
		m_count++;
		int result = m_count;
		m_used = 0;
		m_count = 0;
		return result;
	}
	
private:
	int m_count;
	int m_used;
};

template <typename T> class CSwapVector
{
public:
	typedef swap_ptr<T> iterator;
	typedef swap_ptr<T> const_iterator;

	explicit CSwapVector() : m_size(0), m_begin(0), m_end(0), m_alloc_end(0) { }
	~CSwapVector() { clear(); }
	inline iterator begin() { return iterator(m_begin); }
	inline const_iterator begin() const { return const_iterator(m_begin); }
	inline iterator end() { return iterator(m_end); }
	inline const_iterator end() const { return const_iterator(m_end); }
	
	bool empty() const { return m_size == 0; }
	size_t size() const { return m_size; }
	size_t mem_size() const { return m_reserved * page_size; }

	void clear() {
		if (m_begin.notNull()) {
			// memset(m_begin.deref(), 0xeeeeeeee, m_alloc_end - m_end);
			clear_pages(m_begin.get_page(), m_alloc_end.get_page());			
			swapAllocator->free(m_begin.get_mem_addr());
		}
		m_begin.reset();
		m_end.reset();
		m_alloc_end.reset();
		m_size = 0;
		m_reserved = 0;
	}
	void reserve_space(int blocks) {
		if (m_begin.notNull()) clear();
		if (blocks == 0) return;
		m_reserved = blocks;
		int alloc_size = blocks*page_size;
		m_begin = swap_ptr<T>(swapAllocator->alloc(alloc_size));
		// memset(m_begin.deref(), 0xbbbbbbbb, alloc_size);
		m_end = swap_ptr<T>(m_begin);
		m_alloc_end = m_begin;
		m_alloc_end.advance(alloc_size);
	}
	inline swap_ptr<T> append_item(int len) {
		int used = m_end.get_page_offset();
		assert(len < page_size);
		if (used + len > page_size) {
			if (m_size > 0) m_last->set_last(true);
			m_end.advance(page_size - used);
		}
		m_size++;
		assert(m_end < m_alloc_end);
		m_last = m_end;
		assert(m_end <= m_alloc_end);
		m_end.advance(len);
		return m_last;
	}
	void lock() const { lock_pages(m_begin.get_page(), m_alloc_end.get_page()); }
	void unlock() const { unlock_pages(m_begin.get_page(), m_alloc_end.get_page()); }
	void swapout() const { swapout_pages(m_begin.get_page(), m_alloc_end.get_page()); }
	void enable_write() { enable_write_pages(m_begin.get_page(), m_alloc_end.get_page()); }
	void disable_write() { disable_write_pages(m_begin.get_page(), m_alloc_end.get_page()); }

private:
	int m_size;
	int m_reserved;
	swap_ptr<T> m_begin;
	swap_ptr<T> m_last;
	swap_ptr<T> m_end;
	swap_ptr<T> m_alloc_end;

	// deny some operaions
	CSwapVector(const CSwapVector&);
	CSwapVector& operator = (const CSwapVector&);
};

#endif // __SWAPALLOC_H__
