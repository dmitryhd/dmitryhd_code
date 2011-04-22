/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: cachingtest.h,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __CACHINGTEST_H__
#define __CACHINGTEST_H__

#include "test.h"

template <typename CObject, typename CObjectSet> class CCachingTest : public CTest<CObject, CObjectSet> 
{
public:
	typedef CTest<CObject, CObjectSet> test_type;

	CCachingTest(test_type *test ) { m_test = test; m_cache = 0; m_result = false; }
	virtual ~CCachingTest() { delete m_test; m_test = 0; m_cache = 0; m_result = false; }

	virtual bool test(const CObject &obj) 
	{
		if (m_cache) {
			if (&obj == m_cache) {
				return m_result;
			}
		}
		m_cache = &obj;
		m_result = m_test->test(obj);
		return m_result;
	}

	virtual bool is_valid() const { return m_test->is_valid(); }
	
	virtual void reset(const CFileSet& fs) { m_cache = 0; m_result = false; m_test->reset(fs); }

private:
	test_type *m_test;
	const CObject *m_cache;
	bool m_result;
};

#endif
