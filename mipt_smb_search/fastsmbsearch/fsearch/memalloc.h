/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: memalloc.h,v 1.9 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__

#include <vector>
#include <stdlib.h>

typedef unsigned long memaddr_t;

// one contigous block of virtual memory
struct CMemRange
{
	CMemRange(memaddr_t base, long size) : m_base(base), m_size(size) {}
	memaddr_t m_base;
	long m_size;
};

// system malloc-based allocation
class CSystemAllocPolicy
{
public:
	inline static void *alloc(long bytes) { return malloc(bytes); }
	inline static void free(void *ptr) { free(ptr); }
	inline static void flush(void *addr, long size) {}
};

// simple wrapper for allocator
// remember all allocated regions, frees them at destructor
class CSimpleAllocator
{
public:
	~CSimpleAllocator();

	void *alloc(long size);
	void free(void *ptr);
	long totalSize() const;
	void dump() const;
	void freeAll();

private:
	struct MemBlock {
		void *ptr;
		long size;
	};

	typedef std::vector<MemBlock> mem_list;
	mem_list m_list;
};

// manual allocator over one large memory region
// use classical "best-fit" algorithm
// works good only for large blocks
class CBlockAllocator
{
public:
	explicit CBlockAllocator(CMemRange range);
	~CBlockAllocator();

	memaddr_t alloc(long bytes);
	void free(memaddr_t data);
	long totalSize() const;
	long memUsed() const;
	bool check();
	void debug_dump() const;

private:
	long min_size;
	long m_total_size;
	memaddr_t m_base;
	
	struct MemBlock {
		memaddr_t ptr;
		long size;
		bool free;
	};

	typedef std::vector<MemBlock> mem_list;
	mem_list m_list;
};

#endif // __MEMALLOC_H__
