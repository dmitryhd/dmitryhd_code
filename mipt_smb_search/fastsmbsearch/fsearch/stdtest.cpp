/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: stdtest.cpp,v 1.6 2005/09/11 12:11:23 alex_vodom Exp $
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

#include "stdtest.h"

bool CFileIdTest::test(const CFileRecord &r)
{
	return r.id() == m_id;
}

bool CParentIdTest::test(const CFileRecord &r)
{
	return r.parent_id() == m_parent_id;
}

bool CMinSizeTest::test(const CFileRecord &r)
{
	return r.size() >= m_min_size;
}

bool CMaxSizeTest::test(const CFileRecord &r)
{
	return r.size() <= m_max_size;
}

bool CDirTest::test(const CFileRecord &r)
{
	return r.is_dir() == m_is_dir;
}

bool CResTypeTest::test(const CFileRecord &r)
{
	return r.res_type() == m_res_type;
}

bool CNonNullResTypeTest::test(const CFileRecord &r)
{
	return r.res_type() != 0;
}