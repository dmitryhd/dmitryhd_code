/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: test.h,v 1.4 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __TEST_H__
#define __TEST_H__

#include <vector>
#include "record.h"

// abstract test
template <typename CObject, typename CObjectSet> class CTest
{
public:	
	virtual ~CTest() { }

	virtual bool test(const CObject&) { return false; }
	virtual bool is_valid() const { return true; }
	// perform setup for given set of tested objects
	virtual void reset(const CObjectSet&) { }
};

// test, that always return false
template <typename CObject, typename CObjectSet> class CFalseTest : public CTest<CObject, CObjectSet>
{
public:	
	virtual bool test(const CObject&) { return false; }
};

// test, that always return true
template <typename CObject, typename CObjectSet> class CTrueTest : public CTest<CObject, CObjectSet>
{
public:
	virtual bool test(const CObject&) { return true; }
};

// abstract hinted test
template <typename Hint, typename Data> class CHinter
{
public:
	// real initialization, may be long
	// don't do any non-trivial actions in constructor
	virtual void init() { }
	// estimated probability of test match
	virtual double level() const { return 1.0; }
	
	virtual Hint hint(const Data&) const { return Hint(); }

	bool operator < (const CHinter& r) { return level() < r.level(); }

	virtual ~CHinter() { }
};

// record test hint class
class CHint
{
public:
	CHint() { }
	CHint(const std::vector<CRecordPtr>& data) : m_data(data) { }
	
	inline const std::vector<CRecordPtr>& data() const { return m_data; }

	friend inline CHint operator || (const CHint& a, const CHint& b)
	{ 
		CHint result;
		result.m_data.reserve(a.m_data.size() + b.m_data.size());
		result.m_data.insert(result.m_data.end(), a.m_data.begin(), a.m_data.end());
		result.m_data.insert(result.m_data.end(), b.m_data.begin(), b.m_data.end());
		return result;
	}
		
	friend inline CHint operator && (const CHint& a, const CHint& b)
	{
		return (b.m_data.size() > a.m_data.size()) ? b : a;
	}
	
private:
	std::vector<CRecordPtr> m_data;
};

class CFileRecord;
class CHostData;
class CHostSet;
class CFileSet;

typedef CTest<CHostData, CHostSet> CHostTest;
typedef CTest<CFileRecord, CFileSet> CRecordTest;
typedef CHinter<CHint, CFileSet> CFileHinter;

class CHintedTest
{
public:
	// this clas is inside union, so we can't declary normal ctors/dtor
	static CHintedTest ctor(CRecordTest* test, CFileHinter *hinter);
	// simplified ctor, auto-create empty hinter for hinter
	static CHintedTest ctor_no_hint(CRecordTest* test, CFileHinter *hinter);
	
	void clear() { delete m_hinter; m_hinter = 0; delete m_test; m_test = 0; }
	
	// access to members
	CRecordTest* test() { return m_test; }
	CFileHinter* hinter() { return m_hinter; }

	CHintedTest& operator = (CRecordTest* test);
	
	// operators
	friend CHintedTest operator || (CHintedTest& a, CHintedTest& b);
	friend CHintedTest operator && (CHintedTest& a, CHintedTest& b);
	friend CHintedTest operator ! (CHintedTest& a);
		
private:
	CRecordTest *m_test;
	CFileHinter *m_hinter;
};

#endif 
