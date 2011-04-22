/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: stdcmpr.h,v 1.4 2005/01/19 09:02:56 polzun Exp $
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

#ifndef __STDCMPR_H__
#define __STDCMPR_H__

#include "record.h"
#include "hostdata.h"
#include "cacheitem.h"

class CCompareHostPriority : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareHostStatus : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareHostWkg : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareHostName : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareFileSize : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareFileDir : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareFileName : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareFileNameCase : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareFileDate : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

class CCompareFilePath : public CItemCompareFunc
{
public:
	virtual int compare(const CFoundItem& a, const CFoundItem &b) const;
};

#endif
