/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: testoper.h,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __TESTOPER_H__
#define __TESTOPER_H__

#include "test.h"

template <typename CObject, typename CObjectSet> class CAndTest : public CTest<CObject, CObjectSet> 
{
public:
	typedef CTest<CObject, CObjectSet> test_type;

	CAndTest(test_type *t1, test_type *t2 ) { m_t1 = t1; m_t2 = t2; }
	virtual ~CAndTest() { delete m_t1; delete m_t2; }

	virtual bool test(const CObject &obj) 
	{ return m_t1->test(obj) && m_t2->test(obj); }

	virtual bool is_valid() const
	{ return m_t1->is_valid() && m_t2->is_valid(); }

	virtual void reset(const CObjectSet& set) { m_t1->reset(set); m_t2->reset(set); }

private:
	test_type *m_t1, *m_t2;
};

template <typename CObject, typename CObjectSet> class COrTest : public CTest<CObject, CObjectSet> 
{
public:
	typedef CTest<CObject, CObjectSet> test_type;

	COrTest(test_type *t1, test_type *t2 ) { m_t1 = t1; m_t2 = t2; }
	virtual ~COrTest() { delete m_t1; delete m_t2; }

	virtual bool test(const CObject &obj) 
	{ return m_t1->test(obj) || m_t2->test(obj); }

	virtual bool is_valid() const
	{ return m_t1->is_valid() && m_t2->is_valid(); }

	virtual void reset(const CObjectSet& set) { m_t1->reset(set); m_t2->reset(set); }

private:
	test_type *m_t1, *m_t2;
};

template <typename CObject, typename CObjectSet> class CNotTest : public CTest<CObject, CObjectSet> 
{
public:
	typedef CTest<CObject, CObjectSet> test_type;

	CNotTest(test_type *test ) { m_test = test; }
	virtual ~CNotTest() { delete m_test; }

	virtual bool test(const CObject &obj) 
	{ return !m_test->test(obj); }

	virtual bool is_valid() const
	{ return m_test->is_valid(); }

	virtual void reset(const CObjectSet& set) { m_test->reset(set); }

private:
	test_type *m_test;
};

template <typename Hint, typename Data> class CAndHinter : public CHinter<Hint, Data>
{
public:
	typedef CHinter<Hint, Data> hinter_type;

	CAndHinter(hinter_type *t1, hinter_type *t2) {
		m_t1 = t1;
		m_t2 = t2;
		m_best = 0;
	}
	virtual void init() {
		m_t1->init();
		m_t2->init();
		m_best = (*m_t1 < *m_t2) ? m_t1 : m_t2;
		m_level = m_best->level();
	}

	virtual ~CAndHinter() { m_best = 0; delete m_t1; delete m_t2; m_t1 = m_t2 = 0; }

	virtual double level() const { return m_level; }
	
	virtual Hint hint(const Data& data) const { return m_best->hint(data); }

private:
	double m_level;
	hinter_type *m_t1, *m_t2;
	hinter_type *m_best;
};

template <typename Hint, typename Data> class COrHinter : public CHinter<Hint, Data>
{
public:
	typedef CHinter<Hint, Data> hinter_type;

	COrHinter(hinter_type *t1, hinter_type *t2) {
		m_t1 = t1;
		m_t2 = t2;
	}
	virtual void init() {
		m_t1->init();
		m_t2->init();
		m_level = m_t1->level() + m_t2->level();
		if (m_level > 1.0) m_level = 1.0;
	}
	virtual ~COrHinter() { delete m_t1; delete m_t2; }

	virtual double level() const { return m_level; }
	
	virtual Hint hint(const Data& data) const { return m_t1->hint(data) || m_t2->hint(data); }

private:
	double m_level;
	hinter_type *m_t1, *m_t2;
};

template <typename Hint, typename Data> class CMultiOrHinter : public CHinter<Hint, Data>
{	
public:
	typedef CHinter<Hint, Data> hinter_type;
	
	CMultiOrHinter(const typename std::vector<hinter_type *>& hinters) : m_hinters(hinters) { }

	virtual ~CMultiOrHinter()
	{
		typename std::vector<hinter_type *>::iterator i;
		for (i = m_hinters.begin(); i != m_hinters.end(); i++) {
			delete *i;
			*i = 0;
		}
		m_hinters.clear();
	}

	virtual void init()
	{
		typename std::vector<hinter_type *>::iterator i;
		m_level = 0;
		for (i = m_hinters.begin(); i != m_hinters.end(); i++) {
			(*i)->init();
			m_level += (*i)->level();
			if (m_level >= 1.0) {
				m_level = 1.0;
				return;
			}
		}
	}

	virtual double level() const { return m_level; }
	
	virtual Hint hint(const Data& data) const
	{
		CHint result;
		typename std::vector<hinter_type *>::const_iterator i;
		for (i = m_hinters.begin(); i != m_hinters.end(); i++) {
			result = result || (*i)->hint(data);
		}
		return result;
	}

private:
	double m_level;
	typename std::vector<hinter_type *> m_hinters;
};

// empty hinter, exist only to provide destruction of hinter
template <typename Hint, typename Data> class CEmptyHinter : public CHinter<Hint, Data>
{
public:
	typedef CHinter<Hint, Data> hinter_type;

	CEmptyHinter(hinter_type *t) { m_t = t; }
	virtual ~CEmptyHinter() { delete m_t; m_t = 0; }

	virtual double level() const { return 1.0; }
	
	virtual Hint hint(const Data&) const { return Hint(); }

private:
	hinter_type *m_t;
};

#endif
