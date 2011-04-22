/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: parenttest.cpp,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#include "parenttest.h"

bool CParentTest::test(const CFileRecord &r)
{
	if (r.id() == 0) return false;
	return m_test->test(*r.parent());
}

void CParentTest::reset(const CFileSet& fs_)
{
	fs = &fs_;
	m_test->reset(fs_);
}

bool CPathTest::test(const CFileRecord &r)
{
	const CFileRecord *p = &r;
	while (p->id() != 0) {
		if (m_test->test(*p)) return true;
		p = &*(p->parent());
	}
	return false;
}

void CPathTest::reset(const CFileSet& fs_)
{
	fs = &fs_;
	m_test->reset(fs_);
}
