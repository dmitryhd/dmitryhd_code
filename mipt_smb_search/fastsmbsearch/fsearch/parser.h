/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: parser.h,v 1.6 2005/01/20 16:57:16 alex_vodom Exp $
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

#ifndef __PARSER_H__
#define __PARSER_H__

#include <list>
#include "test.h"
#include "cacheitem.h"
#include "memalloc.h"

class CParser
{
public:
	// getters
	const CHintedTest& test() const { return m_test; }
	CHostTest* host_test() const { return m_host_test; }
	std::list<CItemCompareFunc*> sort_list() const { return m_sort_list; }
	// methods
	bool parse(const char *query);

private:
	CSimpleAllocator m_allocator;
	CHintedTest m_test;
	CHostTest *m_host_test;
	std::list<CItemCompareFunc*> m_sort_list;
};

#endif
