/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: stdhost.h,v 1.6 2005/09/11 12:36:36 shurikz Exp $
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

#ifndef __STDHOST_H__
#define __STDHOST_H__

#include "hostdata.h"
#include "test.h"

class CHostTrueTest : public CHostTest
{
public:
	virtual bool test(const CHostData &h);
};

class CHostIDTest : public CHostTest
{
public:
	CHostIDTest(int host_id) : m_host_id(host_id) { }
	
	virtual bool test(const CHostData &h);

private:
	int m_host_id;
};

class CHostOnlineTest : public CHostTest
{
public:
	virtual bool test(const CHostData &h);

private:
};

class CHostWkgTest : public CHostTest
{
public:
	CHostWkgTest(int wkg_id) : m_wkg_id(wkg_id) { }
	
	virtual bool test(const CHostData &h);

private:
	int m_wkg_id;
};

class CHostProtoTest : public CHostTest
{
public:
	CHostProtoTest(int proto) : m_proto(proto) { }
	
	virtual bool test(const CHostData &h);

private:
	int m_proto;
};
#endif
