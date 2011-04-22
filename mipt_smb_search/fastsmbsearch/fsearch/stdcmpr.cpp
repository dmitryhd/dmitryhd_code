/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: stdcmpr.cpp,v 1.4 2005/01/19 09:02:56 polzun Exp $
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

#include "stdcmpr.h"

int CCompareHostPriority::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return a.m_host->priority() - b.m_host->priority();
}

int CCompareHostStatus::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return a.m_host->status_prio() - b.m_host->status_prio();
}

int CCompareHostWkg::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return a.m_host->wkg_id() - b.m_host->wkg_id();
}

int CCompareHostName::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return strcmp(a.m_host->name(), b.m_host->name());
}

int CCompareFileSize::compare(const CFoundItem& a, const CFoundItem &b) const
{
	long long int _tmp = a.m_record->size() - b.m_record->size();
	if (_tmp < 0) return -1;
	if (_tmp > 0) return 1;
	return 0;
}

int CCompareFileDir::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return b.m_record->is_dir() - a.m_record->is_dir();
}

int CCompareFileName::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return strcasecmp(a.m_record->name(), b.m_record->name());
}

int CCompareFileNameCase::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return strcmp(a.m_record->name(), b.m_record->name());
}

int CCompareFilePath::compare(const CFoundItem& a, const CFoundItem &b) const
{
	return a.m_record->id() - b.m_record->id();
}
