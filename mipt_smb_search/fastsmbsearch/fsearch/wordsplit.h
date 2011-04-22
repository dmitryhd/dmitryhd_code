/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: wordsplit.h,v 1.6 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __WORDSPLIT_H__
#define __WORDSPLIT_H__

#include <vector>
#include <string>
#include "wordhash.h"

class CWordSplitter
{
public:
	explicit CWordSplitter() { init(); }
	
	std::vector<std::string> split(const char *str);
	void split(const char *str, std::vector<int>& result);
	inline bool is_separator(const char c) const { return m_separator[(unsigned char)c]; }

private:
	void init();

	static const std::string m_default;
	bool m_separator[256];
};

extern CWordSplitter word_splitter;

#endif
