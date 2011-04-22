/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: cacheitem.h,v 1.6 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __CACHEITEM_H__
#define __CACHEITEM_H__

#include <vector>
#include <list>
#include "record.h"
#include "fileset.h"
#include "hostdata.h"

class CFoundItem
{
public:
	CFoundItem() {};
	CFoundItem(const CRecordPtr& a_record, const CHostData *a_host) :
		m_record(a_record), m_host(a_host) {};
		
	inline bool operator < (const CFoundItem& b) const;
		
	CRecordPtr m_record;
	const CHostData *m_host;
};

inline bool CFoundItem::operator < (const CFoundItem& b) const
{
	if (m_host->priority() < b.m_host->priority()) return true;
	if ((m_host->priority() == b.m_host->priority()) && (m_record < b.m_record)) return true;
	return false;
}

/* compare function: 
 *    compare(a,b) return 0 if a==b , < 0 if a < b, and > 0 if a > b */
class CItemCompareFunc
{
public:
	inline CItemCompareFunc() : reverse(false) {};
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const = 0;
	virtual ~CItemCompareFunc() {}
	bool reverse;
};

#endif
