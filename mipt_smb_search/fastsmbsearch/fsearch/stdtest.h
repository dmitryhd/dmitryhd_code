/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: stdtest.h,v 1.7 2005/09/11 12:11:23 alex_vodom Exp $
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

#ifndef __SIZETEST_H__
#define __SIZETEST_H__

#include "record.h"
#include "test.h"

class CFileIdTest : public CRecordTest
{
public:
	CFileIdTest(int id) { m_id = id; };
	
	virtual bool test(const CFileRecord &r);

private:
	int m_id;
};

class CParentIdTest : public CRecordTest
{
public:
	CParentIdTest(int parent_id) { m_parent_id = parent_id; };
	
	virtual bool test(const CFileRecord &r);

private:
	int m_parent_id;
};

class CMinSizeTest : public CRecordTest
{
public:
	CMinSizeTest(const long long unsigned int min_size) { m_min_size = min_size; };
	
	virtual bool test(const CFileRecord &r);

private:
	long long unsigned int m_min_size;
};

class CMaxSizeTest : public CRecordTest
{
public:
	CMaxSizeTest(const long long unsigned int max_size) { m_max_size = max_size; };
	
	virtual bool test(const CFileRecord &r);

private:
	long long unsigned int m_max_size;
};

class CMinDateTest : public CRecordTest
{
public:
	CMinDateTest(int min_date) { m_min_date = min_date; };
	
	virtual bool test(const CFileRecord &r);

private:
	int m_min_date;
};

class CMaxDateTest : public CRecordTest
{
public:
	CMaxDateTest(int max_date) { m_max_date = max_date; };
	
	virtual bool test(const CFileRecord &r);

private:
	int m_max_date;
};

class CDirTest : public CRecordTest
{
public:
	CDirTest(const unsigned char is_dir) { m_is_dir = is_dir; };
	
	virtual bool test(const CFileRecord &r);

private:
	unsigned char m_is_dir;
};

class CResTypeTest : public CRecordTest
{
public:
	CResTypeTest(const unsigned char res_type) { m_res_type = res_type; };
	
	virtual bool test(const CFileRecord &r);

private:
	unsigned char m_res_type;
};

class CNonNullResTypeTest : public CRecordTest
{
public:
	CNonNullResTypeTest() { }
	
	virtual bool test(const CFileRecord &r);
};
	
#endif
