/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: fileset.cpp,v 1.9 2005/09/11 06:54:28 alex_vodom Exp $
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

#include <utility>
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "record.h"
#include "strheap.h"
#include "fileread.h"
#include "uppercase.h"
#include "fileset.h"
#include "classify.h"
#include "logger.h"

CFileSet::~CFileSet()
{
	clear();
}

void CFileSet::clear()
{
	clear_index();
	m_nonleaf.unlock();
	m_nonleaf.clear();
	m_leaf.clear();
}

struct CTmpRecord {
	CFileRecord rec;
	char *name;
	int parent;
};

void CFileSet::load_from_file(const char *file_name)
{
	CFileRecord record;
	CFileReader reader(file_name);
	
	if (!empty()) clear();
	// skip empty files
	if (reader.total_lines() == 0) return;
	if (reader.total_lines() > 1000000) {
		std::ostringstream err;
		err << "Very large number of lines in file " << file_name << ": " << reader.total_lines();
		throw std::runtime_error(err.str());
	}
	// two-pass loading: first pass for check data and estimate size needed for strings
	int line_count = 0;
	std::vector<CTmpRecord> data;
	data.reserve(reader.total_lines());
	CStringHeap<CSystemAllocPolicy> sheap;
	while (reader.get_next()) {
		const char *begin = reader.begin();
		const char *end = reader.end();
		unsigned int name_len;
		const char *name;
		int parent_id;
		// skip empty lines
		if (end == begin) continue;
		// check for invalid records
		if (!record.from_string(begin, end, name_len, parent_id, &name)) {
			std::ostringstream err;
			err << "Wrong filelist: invalid record at " << file_name << ", line " << reader.get_line();
			throw std::runtime_error(err.str());
		}
		int id = record.id();
		if (line_count != id) {
			std::ostringstream err;
			err << "Wrong filelist: ids are not sequential: " << id << "instead of " + line_count;
			throw std::runtime_error(err.str());
		}
		if ((id > 0) && (parent_id >= id)) {
			std::ostringstream err;
			err << "Wrong filelist: invalid parent_id: " << parent_id << " (id = " << id << ")";
			throw std::runtime_error(err.str());
		}
		line_count++;
		CTmpRecord tmp;
		tmp.rec = record;
		tmp.name = sheap.append(name_len, name);
		// convert suspicious symbols to spaces
		for (unsigned int i = 0; i < name_len; i++) {
			char ch = tmp.name[i];
			if ((unsigned char)ch < 0x20) ch = 0x20;
			tmp.name[i] = ch;
		}
		tmp.parent = parent_id;
		data.push_back(tmp);
	}
	reader.close();

	// resort according to best word probability
	std::vector<int> words(20);
	std::string s;
	std::vector<std::pair<int, int> > sortindex;
	sortindex.reserve(line_count);
	for (int i = 0; i < line_count; i++) {
		s.assign(data[i].name);
		std::string::iterator p = s.begin();
		while (p != s.end()) {
			*p = upper(*p);
			p++;
		}
		word_splitter.split(s.c_str(), words);
		std::vector<int>::const_iterator j;
		int best_level = 0;
		for (j = words.begin(); j != words.end(); j++) {
			const CWordInfo& winfo = wordhash.value(*j);
			if (winfo.is_common()) continue;
			int substr_id = winfo.m_best_substr;
			if (substr_id) {
				int level = searchhistory.value(substr_id).m_hits;
				if (level > best_level) best_level = level;
			}
		}
		sortindex.push_back(std::pair<int,int>(best_level, i));
	}
	// sort, but don't move root element
	std::sort(sortindex.begin() + 1, sortindex.end());
	CSwapVectorEstimator leaf_est;
	CSwapVectorEstimator nonleaf_est;
	int str_size = 0;
	for (int i = 0; i < line_count; i++) {
		const CTmpRecord& tmp = data[sortindex[i].second];
		const char *name = tmp.name;
		int name_len = strlen(name);
		// name length + trailing zero
		str_size += name_len + 1;
		int rec_size = sizeof(CFileRecord) - 1 + name_len + 1;
		if (tmp.rec.is_dir()) {
			nonleaf_est.add(rec_size);
		} else {
			leaf_est.add(rec_size);
		}
	}
	// allocate memory
	m_leaf.reserve_space(leaf_est.finish());
	m_leaf.lock();
	m_leaf.enable_write();
	m_nonleaf.reserve_space(nonleaf_est.finish());
	m_nonleaf.lock();
	m_nonleaf.enable_write();
	reader.rewind();
	// fixme: reduce memory usage
	std::vector<CRecordPtr> back_index(line_count);
	for (int i = 0; i < line_count; i++) {
		CTmpRecord& tmp = data[sortindex[i].second];
		CFileRecord& record = tmp.rec;
		int id = record.id();
		const char *name = tmp.name;
		unsigned int name_len = strlen(name);
		int rec_size = sizeof(CFileRecord) - 1 + name_len + 1;
		CRecordPtr addr;
		if (record.is_dir()) {
			addr = m_nonleaf.append_item(rec_size);
		} else {
			addr = m_leaf.append_item(rec_size);
		}
		*addr = record;
		strcpy(addr->name(), name);
		back_index[id] = addr;
		// reset parent to 0 to avoid using uninitialized memory
		addr->set_parent(CRecordPtr(0));
		// classify record
		addr->classify();
	}
	// setup proper parents
	for (int i = 1; i < line_count; i++) {
		CRecordPtr addr = back_index[i];
		addr->set_parent(back_index[data[i].parent]);
	}
	logger->log(4, "Loaded %d lines, %d bytes in %lu swap size", line_count, str_size, m_leaf.mem_size() + m_nonleaf.mem_size());
	// calculate size
	if (line_count > 0) {
		for (iterator i = m_nonleaf.begin(); i != m_nonleaf.end(); ++i) {
			i->reset_size();
		}
		for (int i = line_count - 1; i > 0; i--) {
			CRecordPtr item = back_index[i];
			CRecordPtr parent = back_index[item->parent_id()];
			parent->add_size(item->size());
		}
	}
	back_index.clear();
	build_word_index();
	m_leaf.unlock();
	m_leaf.swapout();
	m_leaf.disable_write();
	// important: leave m_nonleaf locked!
}

size_t CFileSet::mem_size() const
{
	return sizeof(CFileSet) + m_word_index.capacity() * sizeof(WordIndexItem);
}

unsigned long long CFileSet::total_size() const
{
	if (count() == 0) return 0;
	return m_nonleaf.begin()->size();
}

void CFileSet::build_word_index()
{
	std::vector<int> words(20);
	std::string s;
	for (const_iterator i = m_leaf.begin(); i != m_leaf.end(); ++i) {
		s.assign(i->name());
		std::string::iterator p = s.begin();
		while (p != s.end()) {
			*p = upper(*p);
			p++;
		}
		word_splitter.split(s.c_str(), words);
		std::vector<int>::const_iterator j;
		for (j = words.begin(); j != words.end(); j++) {
			wordhash.value(*j).inc();			
			m_word_index.push_back(WordIndexItem(*j, i));
		}
	}
	for (const_iterator i = m_nonleaf.begin(); i != m_nonleaf.end(); ++i) {
		s.assign(i->name());
		std::string::iterator p = s.begin();
		while (p != s.end()) {
			*p = upper(*p);
			p++;
		}
		word_splitter.split(s.c_str(), words);
		std::vector<int>::const_iterator j;
		for (j = words.begin(); j != words.end(); j++) {
			wordhash.value(*j).inc();
			m_word_index.push_back(WordIndexItem(*j, i));
		}
	}
	// copy vector to new with exact size to free space
	word_index(m_word_index).swap(m_word_index);
	std::sort(m_word_index.begin(), m_word_index.end());
}

void CFileSet::clear_index()
{
	for_each(m_word_index.begin(), m_word_index.end(), std::mem_fun_ref(&WordIndexItem::dec));
	m_word_index.clear();
}


void CFileSet::res_type_stat(unsigned long long int *res_size) const
{
	int i;
	for (i = 0; i < RES_TYPE_MAX; i++) {
		res_size[i] = 0;
	}
	CFileSet::const_iterator b, e;
	b = m_leaf.begin();
	e = m_leaf.end();
	CFileSet::const_iterator fi;
	for (fi = b; fi != e; ++fi) {
		// don't count directories (they now have non-zero size);
		if (fi->is_dir()) continue;
		int res_type = fi->res_type();
		if (res_type > RES_TYPE_MAX) {
			logger->log(1, "Warning: unknown resource type %d at file %d!", res_type, fi->id());
			res_type = 0;
		}
		long long unsigned int file_size = fi->size();
		res_size[0] += file_size; 
		if (res_type > 0) {
			res_size[res_type] += file_size;
		}
	}
}

struct file_dumper
{
	file_dumper(std::ostream& os_) : os(os_) {}

	void operator()(const CFileRecord& r) {
		os << r.id() << '\t' << r.parent_id() << '\t' << r.name() << '\t' << r.size() << '\t'
		   << int(r.is_dir()) << '\t' << int(r.res_type()) << '\n';
	}
private:
	std::ostream& os;
};

void dump(const CFileSet& fs, const char *file_name)
{
	std::ofstream f(file_name);
	if (!f) {
		throw std::runtime_error(std::string("Cannot open dump file ") + file_name + " for writing");
	}
	file_dumper fd(f);
	fs.for_all_ref(fd);
	f.close();
}

struct index_dumper
{
	index_dumper(std::ostream& os_) : os(os_) {}

	void operator()(const CFileSet::WordIndexItem& r) {
		os << r.word() << '\t' << r.record()->id() << '\n';
	}
private:
	std::ostream& os;
	
};

void dump_index(const CFileSet& fs, const char *file_name)
{
	std::ofstream f(file_name);
	if (!f) {
		throw std::runtime_error(std::string("Cannot open dump file ") + file_name + " for writing");
	}
	index_dumper id(f);
	const CFileSet::word_index& index = fs.get_word_index();
	for_each(index.begin(), index.end(), id); 
	f.close();
}
