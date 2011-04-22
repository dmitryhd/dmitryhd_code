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

#include <stdexcept>
#include <cstdlib>
#include "swapfile.h"
#include "swapalloc.h"

#define PAGE_ZERO   1
#define PAGE_LOCKED 2
#define PAGE_WRITE  4

// global static variable
CSingleSwapAllocator swapAllocator;

// maximal number of pages read at one time
static const unsigned int max_pages_read_at_once = 256;
// whether to do individual reads or group one?
static const unsigned int max_pages_read_by_one = 32;

static memaddr_t first_block;
static void* read_buf = 0;

void init_slots(long max_rss);
void done_slots();

CSwapAllocator::CSwapAllocator(const std::string& name, long size) :
	m_swapFile(name, size),
	m_blockAllocator(m_swapFile.range())
{
	first_block = alloc(page_size);
}

CSwapAllocator::~CSwapAllocator()
{
	free(first_block);
	first_block = 0;
}

// singleton object creation/destuction
void CSingleSwapAllocator::init(const std::string& name, long size, long max_rss)
{
	m_instance = new CSwapAllocator(name, size);
	init_slots(max_rss);
	page_count = size/page_size;
	page_table = new page_desc[page_count];
	memset(page_table, 0, sizeof(page_desc)*page_count);
	for (unsigned int i = 0; i < page_count; ++i) {
		page_table[i].flags = PAGE_ZERO;
	}
	read_buf = malloc(max_pages_read_at_once * page_size);
}

void CSingleSwapAllocator::finish()
{
	free(read_buf);
	read_buf = 0;
	delete[] page_table;
	page_table = 0;
	done_slots();
	delete m_instance;
	m_instance = 0;
}

unsigned int page_count = 0;
long access_count = 0;
long miss_count = 0;
long page_read_count = 0;
long page_swap_count = 0;

page_desc *page_table;
unsigned int page_present = 0;
unsigned int page_max = 0;
unsigned int page_locked = 0;
unsigned int page_to_scan = 10;

void *slot_start;
int *free_slots;
unsigned int free_slot_pos;
unsigned int *slot_pages;

void init_slots(long max_rss)
{
	page_max = max_rss;
	slot_start = malloc(page_max * page_size);
	memset(slot_start, 0, page_max * page_size);
	free_slots = new int[page_max];
	for (unsigned int i = 0; i < page_max; i++) {
		free_slots[i] = page_max - 1 - i;
	}
	free_slot_pos = page_max;
	slot_pages = new unsigned int[page_max];
}

void done_slots()
{
	delete[] free_slots;
	free_slots = 0;
	free(slot_start);
	slot_start = 0;
}

int get_free_slot()
{
	assert(free_slot_pos > 0);
	// return the head of the free list
	free_slot_pos--;
	int slot = free_slots[free_slot_pos];
	free_slots[free_slot_pos] = 0;
	return slot;
}

void free_slot(int slot)
{
	assert(free_slot_pos < page_max);
	free_slots[free_slot_pos] = slot;
	free_slot_pos++;
}

void swap_out_page(unsigned int page)
{
	//	fprintf(stderr, "Swapping out page %d\n", swap_out);
	void *addr = page_table[page].addr;
	int slot = ((char *)addr - (char*)slot_start) / page_size;
	assert(addr);
	if (page_table[page].flags & PAGE_WRITE) {
		swapAllocator->write(addr, page * page_size, page_size);
	}
	page_table[page].addr = 0;
	page_present--;
	slot_pages[slot] = 0;
	free_slot(slot);
}

void swap_page()
{
	int optimal = 0x7fffffff;
	unsigned int swap_out = 0;
	unsigned int scanned = 0;
	while (scanned < page_to_scan) {
		unsigned int page = slot_pages[rand() % page_max];
		if (!page_table[page].addr) continue;
		if (page_table[page].flags & PAGE_LOCKED) continue;
		scanned++;
		if (page_table[page].usage < optimal) {
			optimal = page_table[page].usage;
			swap_out = page;
		}
	}
	page_swap_count++;
	swap_out_page(swap_out);
}

void* load_page(unsigned int pageno)
{
	while (page_present >= page_max) {
		swap_page();
	}
	int slot = get_free_slot();
	slot_pages[slot] = pageno;
	void *buf = (char*)slot_start + slot * page_size;
	page_table[pageno].addr = buf;
	if (page_table[pageno].flags & PAGE_ZERO) {
		memset(buf, 0, page_size);
		page_table[pageno].flags &= ~PAGE_ZERO;
	} else {
		swapAllocator->read(buf, pageno * page_size, page_size);
		page_read_count++;
	}
	page_present++;
	return buf;
}

static void lock_pages_group(unsigned int pagebegin, unsigned int pageend)
{
	unsigned int need_read = 0;
	for (unsigned i = pagebegin; i < pageend; i++) {
		if (page_table[i].addr) continue;
		assert(!(page_table[i].flags & PAGE_LOCKED));
		if (!(page_table[i].flags & PAGE_ZERO)) {
			need_read++;
		}
	}
	if (need_read < max_pages_read_by_one) {
		for (unsigned i = pagebegin; i < pageend; i++) {
			if (page_table[i].addr) continue;
			load_page(i);
			page_table[i].flags |= PAGE_LOCKED;
			page_locked++;
		}
		return;
	}
	int count = pageend - pagebegin;
	swapAllocator->read(read_buf, pagebegin * page_size, count * page_size);
	page_read_count++;
	for (unsigned i = pagebegin; i < pageend; i++) {
		if (page_table[i].addr) continue;
		int slot = get_free_slot();
		slot_pages[slot] = i;
		void *pagebuf = (char*)slot_start + slot * page_size;
		page_table[i].addr = pagebuf;
		if (page_table[i].flags & PAGE_ZERO) {
			memset(pagebuf, 0, page_size);
			page_table[i].flags &= ~PAGE_ZERO;
		} else {
			memcpy(pagebuf, (char*)read_buf + (i - pagebegin)*page_size, page_size);
		}
		page_present++;
		page_table[i].flags |= PAGE_LOCKED;
		page_locked++;
	}
}

void lock_pages(unsigned int pagebegin, unsigned int pageend)
{
	// first, free memory
	unsigned int need_lock = 0;
	for (unsigned i = pagebegin; i < pageend; i++) {
		assert(!(page_table[i].flags & PAGE_LOCKED));
		if (page_table[i].addr) {
			page_table[i].flags |= PAGE_LOCKED;
			page_locked++;
		} else {
			need_lock++;
		}
	}
	while (page_present > page_max - need_lock) {
		swap_page();
	}
	unsigned int start = pagebegin;
	// read by groups
	while (start < pageend) {
		unsigned int count = pageend - start;
		if (count > max_pages_read_at_once) count = max_pages_read_at_once;
		lock_pages_group(start, start + count);
		start += count;
	}
}

void unlock_pages(unsigned int pagebegin, unsigned int pageend)
{
	for (unsigned i = pagebegin; i < pageend; i++) {
		assert(page_table[i].flags & PAGE_LOCKED);
		page_table[i].flags &= ~PAGE_LOCKED;
		page_locked--;
	}
}

void swapout_pages(unsigned int pagebegin, unsigned int pageend)
{
	for (unsigned i = pagebegin; i < pageend; i++) {
		if (!page_table[i].addr) continue;
		assert(!(page_table[i].flags & PAGE_LOCKED));
		swap_out_page(i);
	}
}

void enable_write_pages(unsigned int pagebegin, unsigned int pageend)
{
	for (unsigned i = pagebegin; i < pageend; i++) {
		page_table[i].flags |= PAGE_WRITE;
	}
}

void disable_write_pages(unsigned int pagebegin, unsigned int pageend)
{
	for (unsigned i = pagebegin; i < pageend; i++) {
		page_table[i].flags &= ~PAGE_WRITE;
	}
}

void clear_pages(unsigned int pagebegin, unsigned int pageend)
{
	for (unsigned i = pagebegin; i < pageend; i++) {
		assert(!(page_table[i].flags & PAGE_LOCKED));
		page_table[i].flags &= ~PAGE_WRITE;
		if (page_table[i].addr) swap_out_page(i);
		page_table[i].flags |= PAGE_ZERO;
		page_table[i].usage = 0;
	}
}

void clear_page_cache()
{
	for (unsigned int i = 0; i < page_count; ++i) {
		if (!page_table[i].addr) continue;
		if (page_table[i].flags & PAGE_LOCKED) continue;
		swap_out_page(i);
	}
}

void do_page_error(unsigned int pageno)
{
	static char errbuf[100];
	sprintf(errbuf, "Attempt to access page %u which exceeds maximum possible %u", pageno, page_count);
	throw std::logic_error(errbuf);
}
