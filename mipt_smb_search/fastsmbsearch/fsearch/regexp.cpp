/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: regexp.cpp,v 1.4 2005/01/19 09:02:56 polzun Exp $
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

#include <sys/types.h>
#include <regex.h>
#include "record.h"
#include "test.h"
#include "regexp.h"
#include "logger.h"

CRegexpTest::CRegexpTest(const char *regexp)
{
	m_valid = false;
	if (regcomp(&m_regexp, regexp, REG_ICASE | REG_NOSUB) == 0) {
		m_valid = true;
	} else {
		logger->log(1, "Invalid regular expression %s", regexp);
	}
}

CRegexpTest::~CRegexpTest()
{
	if (is_valid()) {
		regfree(&m_regexp);
	}
}

bool CRegexpTest::test(const CFileRecord& r)
{
	return regexec(&m_regexp, r.name(), 0, 0, 0) == 0;
}
