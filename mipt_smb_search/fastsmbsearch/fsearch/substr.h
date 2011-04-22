/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: substr.h,v 1.6 2005/04/04 17:18:30 alex_vodom Exp $
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

#ifndef __SUBSTR_H__
#define __SUBSTR_H__

#include "record.h"
#include "test.h"
#include "uppercase.h"
#include "wordhash.h"
#include "wordsplit.h"
#include "fileset.h"

class CBoyerMooreTest : public CRecordTest
{
public:
	CBoyerMooreTest(const char *substring);
	virtual ~CBoyerMooreTest();
	
	virtual bool test(const CFileRecord &r);
	virtual bool is_valid() const { return m_valid; }
	
	bool test(const std::string& s);

private:
	void init_upper_case();
	void preBmBc();
	void suffixes(int *suff);
	void preBmGs();
	inline bool bm_test(const char *str) const;

	bool m_valid;
	int *bmGs;
	int *bmBc;
	char *bm_substring;
	int bm_substring_len;
};

class CShiftAndTest : public CRecordTest
{
public:
	CShiftAndTest(const char *substring, int errors);
	virtual ~CShiftAndTest();
	
	virtual bool test(const CFileRecord &r);
	virtual bool is_valid() const { return m_valid; }
	
	bool test(const std::string& s);

private:
	void prepare();
	inline bool sa_test(const char *str) const;

	bool m_valid;
	int m_errs; // allowed number of mistakes
	char *m_substring;
	int m_substring_len;
	unsigned *m_table;
	unsigned m_complete;
};

class CSubstringHinter : public CFileHinter
{
public:
	CSubstringHinter(const char *substring);
	virtual ~CSubstringHinter();

	virtual void init() = 0; // should be overriden in ancestors
	virtual double level() const { return m_hint_level; }
	virtual CHint hint(const CFileSet& fileset) const;

protected:

	char *m_str;
	double m_hint_level;
	std::vector<int> m_words;
};

class CBMSubstringHinter : public CSubstringHinter
{
public:
	CBMSubstringHinter(const char *substring);

	virtual void init();
};

class CSASubstringHinter : public CSubstringHinter
{
public:
	CSASubstringHinter(const char *substring, int errors);

	virtual void init();

private:
	int m_errors;
};

#endif
