/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: parenttest.h,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __PARENTTEST_H__
#define __PARENTTEST_H__

#include "record.h"
#include "fileset.h"
#include "test.h"

class CParentTest : public CRecordTest
{
public:
	CParentTest(CRecordTest *test) { m_test = test; }
	virtual ~CParentTest() { delete m_test; m_test = 0; }

	virtual bool test(const CFileRecord &r);
	virtual bool is_valid() const { return m_test->is_valid(); }
	virtual void reset(const CFileSet& fs);

private:
	CRecordTest *m_test;
	const CFileSet *fs;
};

class CPathTest : public CRecordTest
{
public:
	CPathTest(CRecordTest *test) { m_test = test; }
	virtual ~CPathTest() { delete m_test; m_test = 0; }

	virtual bool test(const CFileRecord &r);
	virtual bool is_valid() const { return m_test->is_valid(); }
	virtual void reset(const CFileSet& fs);

private:
	CRecordTest *m_test;
	const CFileSet *fs;
};

#endif
