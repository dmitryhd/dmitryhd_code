/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: memalloc.cpp,v 1.10 2005/09/08 18:52:52 alex_vodom Exp $
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

/* Definiton of simple memory allocator.
 * Needed to prevent memory leaks
 */

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "memalloc.h"
#include "logger.h"

#undef MEMALLOC_DEBUG

CSimpleAllocator::~CSimpleAllocator()
{
#ifdef MEMALLOC_DEBUG
	fprintf(stderr, "Destroying mem allocator: %ld bytes used\n", totalSize());
#endif
	freeAll();
}
	
void* CSimpleAllocator::alloc(long size)
{
	void *ptr = ::malloc(size);
	MemBlock block;
	block.ptr = ptr;
	block.size = size;
	m_list.push_back(block);
#ifdef MEMALLOC_DEBUG
	fprintf(stderr, "Allocation of %ld bytes: %p\n", size, ptr);
#endif
	return ptr;
}

void CSimpleAllocator::free(void *ptr)
{
	mem_list::iterator it;

#ifdef MEMALLOC_DEBUG
	fprintf(stderr, "Freeing of %p\n", ptr);
#endif
	for (it = m_list.begin(); it != m_list.end(); it++) {
		if (it->ptr == ptr) {
			::free(ptr);
			m_list.erase(it);
			return;
		}
	}
#ifdef MEMALLOC_DEBUG
	fprintf(stderr, "Trying to free non-allocated memory %p\n", ptr);
#endif
}

long CSimpleAllocator::totalSize() const
{
	mem_list::const_iterator it;

	long size = 0;
	for (it = m_list.begin(); it != m_list.end(); it++) {
		size += it->size;
	}
	return size;
}

void CSimpleAllocator::dump() const
{
	mem_list::const_iterator it;

	for (it = m_list.begin(); it != m_list.end(); it++) {
		fprintf(stderr, "Mem at ptr = %p, size = %ld\n", it->ptr, it->size);
	}
}

void CSimpleAllocator::freeAll()
{
	mem_list::iterator it;

	for (it = m_list.begin(); it != m_list.end(); it++) {
#ifdef MEMALLOC_DEBUG
		fprintf(stderr, "Freeing mem at ptr = %p, size = %ld\n", it->ptr, it->size);
#endif
		::free(it->ptr);
	}
	m_list.clear();
}


CBlockAllocator::CBlockAllocator(CMemRange range)
{
	m_total_size = range.m_size;
	m_base = range.m_base;
	min_size = 4096;
	m_list.clear();
	MemBlock block;
	block.free = true;
	block.size = m_total_size;
	block.ptr = m_base;
	m_list.push_back(block);
}

CBlockAllocator::~CBlockAllocator()
{
#ifdef MEMALLOC_DEBUG
	fprintf(stderr, "Destroying");
#endif
}

memaddr_t CBlockAllocator::alloc(long size)
{
	if (size <= 0) return 0;
	int real_size = (size / min_size) * min_size;
	if (real_size < size) real_size += min_size;
	if (real_size >= m_total_size) return 0;

	mem_list::iterator it, best_it;
	best_it = m_list.end();
	long best = m_total_size + 1;
	for (it = m_list.begin(); it != m_list.end(); it++) {
		if (!it->free) continue;
	        if (it->size < real_size) continue;
		if (it->size < best) {
			best = it->size;
			best_it = it;
		}
	}
	// do we have good block?
	if (best_it == m_list.end()) {
		logger->log(0, "Out of memory when allocating %ld bytes\n", size);
		throw std::runtime_error("Block allocator: out of memory");
	}
	// found a block to split
	it = best_it;
	MemBlock block;
	block.size = real_size;
	block.ptr = it->ptr;
	block.free = false;
	if (it->size > real_size) {
		it->size -= real_size;
		it->ptr = it->ptr + real_size;
	} else {
		m_list.erase(it);
	}
	m_list.insert(it, block);
	it++;
	return block.ptr;
}

void CBlockAllocator::free(memaddr_t ptr)
{
        mem_list::iterator it;
	for (it = m_list.begin(); it != m_list.end(); it++) {
		if (it->free) continue;
		if (it->ptr >= ptr) break;
	}
	if ((it == m_list.end()) || (it->ptr != ptr)) {
		logger->log(0, "Trying to free non-allocated memory %p\n", ptr);
		throw std::runtime_error("Block allocator: invalid free");
	}
	it->free = true;
	if (it != m_list.begin()) {
		mem_list::iterator prev = it;
		prev--;
		if (prev->free) {
			prev->size += it->size;
			m_list.erase(it);
			it = prev;
		}
	}
	if (it != m_list.end()) {
		mem_list::iterator next = it;
		next++;
		if (next->free) {
			it->size += next->size;
			m_list.erase(next);
			it = next;
		}
	}
}

long CBlockAllocator::totalSize() const
{
	mem_list::const_iterator it;

	long size = 0;
	for (it = m_list.begin(); it != m_list.end(); it++) {
		if (it->free) continue;
		size += it->size;
	}
	return size;
}

long CBlockAllocator::memUsed() const
{
	mem_list::const_iterator it;
	it = m_list.end();
	it--;
	if (it == m_list.begin()) return 0;
	if (it->free) it--;
	return (unsigned long)it->ptr + it->size - (unsigned long)m_base;
}

bool CBlockAllocator::check()
{
	if (m_list.empty()) {
		fprintf(stderr, "Empty list!\n");
		return false;
	}
	mem_list::iterator it = m_list.begin();
	mem_list::iterator next = it;
	next++;
	while (next != m_list.end()) {
		if (it->size <= 0) {
			fprintf(stderr, "Error: invalid size in block (0x%lx, %ld)\n", it->ptr, it->size);
			return false;
		}
		if ((char*)next->ptr != ((char*)it->ptr + it->size)) {
			fprintf(stderr, "Error: block (0x%lx, %ld) with next block starting at 0x%lx\n", it->ptr, it->size, next->ptr);
			return false;
		}
		if (next->free && it->free) {
			fprintf(stderr, "Error: two adjacent free blocks at 0x%lx and 0x%lx\n", it->ptr, next->ptr);
			return false;
		}
		it++;
		next++;
	}
	return true;
}

void CBlockAllocator::debug_dump() const
{
	std::ofstream dump("memdump.txt");
	if (!dump) {
		logger->log(0, "Cannot open file for writing debug memory dump\n");
		return;
	}
	mem_list::const_iterator it;
	for (it = m_list.begin(); it != m_list.end(); it++) {
		dump << (it->free ? "free" : "used") << "mem at ptr = " << it->ptr << ", size = " << it->size << '\n';
	}
}
