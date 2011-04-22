/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: wordsplit.cpp,v 1.10 2005/09/11 06:54:28 alex_vodom Exp $
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

#include "wordsplit.h"

const std::string CWordSplitter::m_default = " ()[]{}|+-=,.:;'`!_~#@%";

CWordSplitter word_splitter;

void CWordSplitter::init()
{
	// default is normal letter
	for (int i = 0; i < 256; i++) 
		m_separator[i] = false;
	// cut off all non-printable characters
	for (int i = 0; i < ' '; i++) 
		m_separator[i] = true;
	// trailing zero is separator!
	m_separator[0] = true;
	// set default separators
	for (std::string::const_iterator p = m_default.begin(); p != m_default.end(); p++) {
		const char c = *p;
		m_separator[(unsigned char)c] = true;
	}
}

std::vector<std::string> CWordSplitter::split(const char *str)
{
	std::vector<std::string> result;
	const char *p = str;
	while (1) {
		while ((*p) && is_separator(*p)) p++;
		if (*p == 0) return result;
		const char *q = p + 1;
		while (*q) {
			if (is_separator(*q)) break;
			q++;
		}
		// index only >= 2 words
		if ((q - p) < 2) { p = q; continue; }
		std::string s(p, q);
		p = q;

		result.push_back(s);
	}
	return result;
}

void CWordSplitter::split(const char *str, std::vector<int>& result)
{
	result.clear();
	std::vector<std::string> words;
	words = split(str);
	std::vector<std::string>::iterator it;
	for (it = words.begin(); it != words.end(); it++) {
		int wid = wordhash.index(it->c_str());
		// don't index common words
		if (wordhash.value(wid).is_common()) continue;
		result.push_back(wid);
	}
}
