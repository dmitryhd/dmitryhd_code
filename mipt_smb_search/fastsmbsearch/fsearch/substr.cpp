/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: substr.cpp,v 1.7 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <cstdlib>
#include <cstring>
#include <cctype>
#include "logger.h"
#include "substr.h"

const int maxerr = 8;

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

bool CBoyerMooreTest::test(const CFileRecord &r)
{
	return bm_test(r.name());
}

bool CBoyerMooreTest::test(const std::string& s)
{
	return bm_test(s.c_str());
}

inline bool CBoyerMooreTest::bm_test(const char *str) const
{
	const char *x = bm_substring;
	const int m = bm_substring_len;
	const char *y = str;
	const int n = strlen(y);
	const char *end = y + n - m;
	char uch;
		
	int i;
	
	/* Searching */
	while (y <= end) {
		for (i = m - 1; i >= 0 && x[i] == (uch = upper(y[i])); --i);
		if (i < 0) {
			/* output */
			return true;
			//j += bmGs[0];
		}
		else {
			y += MAX(bmGs[i], bmBc[(unsigned char)uch] + i);
		}
	}
	return false;
}


void CBoyerMooreTest::preBmBc()
{
	char *x = bm_substring;
	int m = bm_substring_len;
	int i;
	
	for (i = 0; i < 256; ++i)
		bmBc[i] = m;
	for (i = 0; i < m - 1; ++i)
		bmBc[(unsigned char )x[i]] = m - i - 1;

	for (i = 0; i < 256; i++)
		bmBc[i] = bmBc[i] - m + 1;
}
 
void CBoyerMooreTest::suffixes(int *suff)
{
	char *x = bm_substring;
	int m = bm_substring_len;
	int f = 0, g, i;
	
	suff[m - 1] = m;
	g = m - 1;
	for (i = m - 2; i >= 0; --i) {
		if (i > g && suff[i + m - 1 - f] < i - g)
			suff[i] = suff[i + m - 1 - f];
		else {
			if (i < g)
				g = i;
			f = i;
			while (g >= 0 && x[g] == x[g + m - 1 - f])
				--g;
			suff[i] = f - g;
		}
	}
}
 
void CBoyerMooreTest::preBmGs()
{
	int m = bm_substring_len;

	int i, j, *suff;
	
	suff = new int[m];
	suffixes(suff);
	
	for (i = 0; i < m; ++i)
		bmGs[i] = m;
	j = 0;
	for (i = m - 1; i >= -1; --i)
		if (i == -1 || suff[i] == i + 1)
			for (; j < m - 1 - i; ++j)
				if (bmGs[j] == m) bmGs[j] = m - 1 - i;
	for (i = 0; i <= m - 2; ++i)
		bmGs[m - 1 - suff[i]] = m - 1 - i;

	delete[] suff;
}

CBoyerMooreTest::CBoyerMooreTest(const char *substring)
{
	int i;
	
	/* default is error */
	m_valid = false;
	
	bm_substring_len = strlen(substring);
	/* sanity check for string length */
	if (bm_substring_len < 1) {
		logger->log(1, "Attempting to construct substring test with short string %s", substring);
		return;
	}
	bm_substring = new char[bm_substring_len + 1];
	
	for (i=0; i < bm_substring_len; i++) {
		bm_substring[i] = upper(substring[i]);
	}
	bm_substring[bm_substring_len] = 0;
	
	/* Preprocessing */
	bmGs = new int[bm_substring_len];
	bmBc = new int[256];
	preBmGs();
	preBmBc();
	m_valid = true;
}

CBoyerMooreTest::~CBoyerMooreTest()
{
	if (is_valid()) {
		delete[] bmBc;
		bmBc = 0;
		delete[] bmGs;
		bmGs = 0;
		delete[] bm_substring;
		bm_substring = 0;
	}
}
				 
bool CShiftAndTest::test(const CFileRecord &r)
{
	return sa_test(r.name());
}

bool CShiftAndTest::test(const std::string& s)
{
	return sa_test(s.c_str());
}

inline bool CShiftAndTest::sa_test(const char *str) const
{
	  const char *p = str;
	  unsigned m[maxerr];
	  unsigned mprev[maxerr];
	  for (int i = 0; i < m_errs; i++) {
		  m[i] = (unsigned(1) << i) - 1;
	  }
	  while (*p != 0) {
		  unsigned u = m_table[(unsigned char)upper(*p)];
		  mprev[0] = m[0];
		  m[0] = ((m[0] << 1) | 1) & u;
		  if (m[0] & m_complete) return true;
		  for (int i = 1; i < m_errs; i++) {
			  mprev[i] = m[i];
			  m[i] = 1 | ((mprev[i-1] | m[i-1]) << 1) | ((m[i] << 1) & u) | mprev[i-1];
			  if (m[i] & m_complete) {
				  return true;
			  }
		  }
		  p++;
	  }
	  return false;
}

void CShiftAndTest::prepare()
{
	m_table = new unsigned[256];
	for (int i = 0; i < 256; i++) {
		m_table[i] = 0;
	}
	for (int i = 0; i < m_substring_len; i++) {
		m_table[(unsigned char)m_substring[i]] |= (unsigned(1) << i);
	}
	m_complete = unsigned(1) << (m_substring_len - 1);
}

CShiftAndTest::CShiftAndTest(const char *substring, int errors)
{
	/* default is error */
	m_valid = false;
	
	m_substring_len = strlen(substring);
	/* sanity check for string length */
	if (m_substring_len < 1) {
		logger->log(1, "Attempting to construct substring test with short string %s", substring);
		return;
	}
	/* avoid very long string */
	if (m_substring_len > 32) {
		logger->log(1, "Attempting to construct substring test with long string %s", substring);
		return;
	}
	if ((errors < 0) || (errors >= maxerr)) {
		logger->log(1, "Invalid number of errors in shift-and test %d", errors);
		return;
	}
	m_errs = errors + 1;
	m_substring = new char[m_substring_len + 1];

	for (int i = 0; i < m_substring_len; i++) {
	  m_substring[i] = upper(substring[i]);
	}

	prepare();
	m_valid = true;
}

CShiftAndTest::~CShiftAndTest()
{
	if (is_valid()) {
		delete[] m_table;
		m_table = 0;
		delete[] m_substring;
		m_substring = 0;
	}
}

CSubstringHinter::CSubstringHinter(const char *substring)
{
	int len = strlen(substring);
	m_str = 0;
	/* sanity check for string length */
	if (len < 1) {
		logger->log(1, "Attempting to construct substring test with short string %s", substring);
		return;
	}
	m_str = new char[len + 1];
	for (int i = 0; i < len; i++) {
		m_str[i] = upper(substring[i]);
	}
	m_str[len] = 0;
}

CSubstringHinter::~CSubstringHinter()
{
	delete[] m_str;
	m_str = 0;
}

CBMSubstringHinter::CBMSubstringHinter(const char *substring) : CSubstringHinter(substring) { }

CSASubstringHinter::CSASubstringHinter(const char *substring, int errors) :
	CSubstringHinter(substring), m_errors(errors)
{
}

// hmm, it doesn't look good...
static void make_word_hint(CFileSet::word_index::const_iterator idx_begin,
			CFileSet::word_index::const_iterator idx_end,
			std::vector<int>::const_iterator begin,
			std::vector<int>::const_iterator end,
			std::vector<CRecordPtr>& result)
{
	if (begin == end) return;
	if (idx_begin == idx_end) return;
	std::vector<int>::const_iterator middle = begin + (end - begin)/2;
	CFileSet::WordIndexItem sample(*middle, swap_ptr<CFileRecord>(0));
	CFileSet::word_index::const_iterator idx_mid = lower_bound(idx_begin, idx_end, sample);
	make_word_hint(idx_begin, idx_mid, begin, middle, result);
	while ((idx_mid != idx_end) && (idx_mid->word() == *middle)) {
		result.push_back(idx_mid->record());
		idx_mid++;
	}
	if (middle != end) {
		middle++;
		make_word_hint(idx_mid, idx_end, middle, end, result);
	}
}

CHint CSubstringHinter::hint(const CFileSet& fileset) const
{
	std::vector<CRecordPtr> result;
	make_word_hint(fileset.get_word_index().begin(), fileset.get_word_index().end(),
		    m_words.begin(), m_words.end(), result);
	return CHint(result);
}

void CBMSubstringHinter::init()
{
	// set default hint level to 1.0 (no hint)
	m_hint_level = 1.0;
	const std::vector<std::string> words = word_splitter.split(m_str);
	if (words.empty()) {
		return;
	}
	std::vector<std::string>::const_iterator i;
	std::vector<int> best;
	for (i = words.begin(); i != words.end(); i++) {
		const std::string s = *i;
		std::vector<int> sub_words;
		if (s.length() < 2) {
			return;
		}
		CBoyerMooreTest t(s.c_str());
		const std::vector<int>& wset = wordhash.get_dictindex().search_for(s.c_str());
		double level = 0.0;
		std::vector<int>::const_iterator it;
		for (it = wset.begin(); it != wset.end(); it++) {
			if (!t.test(wordhash.key(*it))) continue;
			CWordInfo& wi = wordhash.value(*it);
			wi.m_hits++;
			if (wi.is_common()) {
				level = 1.0;
				break;
			}
			sub_words.push_back(*it);
			level += wi.count() / 20000000.0;
		}
		if (level < 0.05) {
			searchhistory.value(searchhistory.index(i->c_str())).m_hits++;
		}
		// it should be better than previous
		if (level > m_hint_level) continue;
		m_hint_level = level;
		m_words = sub_words;
	}
}

void CSASubstringHinter::init()
{
	// set default hint level to 1.0 (no hint)
	m_hint_level = 1.0;
	const std::vector<std::string> words = word_splitter.split(m_str);
	if (words.empty()) {
		return;
	}
	std::vector<std::string>::const_iterator i;
	std::vector<int> best;
	for (i = words.begin(); i != words.end(); i++) {
		const std::string s = *i;
		std::vector<int> sub_words;
		if (s.length() < 2) {
			return;
		}
		CShiftAndTest t(s.c_str(), m_errors);
		double level = 0.0;
		for (unsigned m = 0; m < s.length() - 1; m++) {
			const std::string ls(s, m, 2);
			const std::vector<int>& wset = wordhash.get_dictindex().search_for(ls.c_str());
			std::vector<int>::const_iterator it;
			for (it = wset.begin(); it != wset.end(); it++) {
				if (!t.test(wordhash.key(*it))) continue;
				CWordInfo& wi = wordhash.value(*it);
				wi.m_hits++;
				if (wi.is_common()) {
					level = 1.0;
					break;
				}
				sub_words.push_back(*it);
				level += wi.count() / 20000000.0;
			}
			if (level == 1.0) break;
		}
		// it should be better than previous
		if (level > m_hint_level) continue;
		std::sort(sub_words.begin(), sub_words.end());
		std::vector<int>::iterator new_end = std::unique(sub_words.begin(), sub_words.end());
		sub_words.erase(new_end, sub_words.end());
		m_hint_level = level;
		m_words = sub_words;
	}
}

