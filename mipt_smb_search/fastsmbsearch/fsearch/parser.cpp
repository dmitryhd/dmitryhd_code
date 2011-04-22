/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: parser.cpp,v 1.6 2005/01/20 16:57:16 alex_vodom Exp $
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

#include "test.h"
#include "stdtest.h"
#include "substr.h"
#include "parser.h"
#include "logger.h"

extern bool zz_parse(const char *string, CSimpleAllocator* allocator, CHintedTest *test, CHostTest **host_test, std::list<CItemCompareFunc*> *sort_list);

bool CParser::parse(const char *query)
{
	/* try to parse */
	if (!zz_parse(query, &m_allocator, &m_test, &m_host_test, &m_sort_list)) {
		return false;
	}

	/* validity check */
	if (!host_test()->is_valid()) {
		logger->log(1, "Host test is not valid in query %s", query);
		std::list<CItemCompareFunc *>::iterator it;
		for (it = m_sort_list.begin(); it != m_sort_list.end(); it++) {
			delete *it;
			*it = 0;
		}
		delete m_host_test;
		m_host_test = 0;
		m_test.clear();
		return false;
	}
	if (!(m_test.test())->is_valid()) {
		logger->log(1, "File test is not valid in query %s", query);
		std::list<CItemCompareFunc *>::iterator it;
		for (it = m_sort_list.begin(); it != m_sort_list.end(); it++) {
			delete *it;
			*it = 0;
		}
		delete m_host_test;
		m_host_test = 0;
		m_test.clear();
		return false;
	}

	/* all seems to be ok */
	return true;
}
