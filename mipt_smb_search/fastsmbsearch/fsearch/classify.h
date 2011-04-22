/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: classify.h,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#ifndef __CLASSIFY_H__
#define __CLASSIFY_H__

#include <map>
#include "record.h"
#include "logger.h"

// all possilbe resource types
enum {
	RES_TYPE_UNKNOWN   =  0,
	RES_TYPE_MOVIE     =  1,
	RES_TYPE_MUSIC     =  2,
	RES_TYPE_DOC       =  3,
	RES_TYPE_PICTURE   =  4,
	RES_TYPE_UNIX_FILE =  5,
	RES_TYPE_CLIP      =  6,
	RES_TYPE_CD_IMAGE  =  7,
	RES_TYPE_CD_INDEX  =  8,
	RES_TYPE_ARCHIVE   =  9,
	RES_TYPE_WIN_EXE   = 10
};

// maximal resource type = last + 1
enum {
	RES_TYPE_MAX = RES_TYPE_WIN_EXE + 1
};

class CFileClassifier
{
public:
	// constructor and destructor
	explicit CFileClassifier();
	// main routine
	int classify(const CFileRecord &r) const;
	
private:		
	typedef long long int hash_t;
		
	hash_t hash_from_ext(const char *ext) const;
	void add_extension(int res_type, const char *ext);
	
	std::map<hash_t, int> m_class_map;
};

extern CFileClassifier classifier;

#endif
