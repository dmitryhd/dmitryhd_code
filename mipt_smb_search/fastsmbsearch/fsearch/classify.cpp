/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: classify.cpp,v 1.4 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <map>
#include <cctype>
#include "record.h"
#include "classify.h"

#define MIN_MOVIE_SIZE 320000000
#define MIN_CD_IMAGE_SIZE 100000000

CFileClassifier classifier;

CFileClassifier::CFileClassifier()
{
	const char **p;
	const char *ext1[] = { "mpeg", "mpg", "avi", "dat", "asf", "ogm", "vob", "m2v", "wmv", "mkv", 0 };
	const char *ext2[] = { "mp3", "wav", "au", "aif", "aiff", "ogg", "mpc", "flac", "fla", "ape", "wma", "ra", "rm", "vqf", "ac3", 0 };
	const char *ext3[] = { "txt", "rtf", "doc", "htm", "html", "pdf", "ps", "djv", "djvu", "chm", 0 };
	const char *ext4[] = { "bmp", "gif", "jpeg", "jpg", "pbm", "pcx", "png", "pnm", "ppm", "psd", "tiff", "tif", "xbm", "xpm", 0 };
	const char *ext5[] = { "rpm", "deb", "spec", "ebuild", "patch", 0 };
	const char *ext7[] = { "bin", "iso", "mdf", "bwt", "cdi", "nrg", 0 };
	const char *ext8[] = { "mds", "ccd", "cue", 0 };
	const char *ext9[] = { "arj", "zip", "rar", "arc", "lha", "lzh", "ace", "tar", "gz", "gzip", "bz2", "tgz", "tbz2", "zoo", "cab", "jar", "7z", "z", 0 };
	const char *ext10[] = { "exe", "dll", 0 };

	for (p = ext1; *p != 0; p++) {
		add_extension(RES_TYPE_MOVIE, *p);
	}
	for (p = ext2; *p != 0; p++) {
		add_extension(RES_TYPE_MUSIC, *p);
	}
	for (p = ext3; *p != 0; p++) {
		add_extension(RES_TYPE_DOC, *p);
	}
	for (p = ext4; *p != 0; p++) {
		add_extension(RES_TYPE_PICTURE, *p);
	}
	for (p = ext5; *p != 0; p++) {
		add_extension(RES_TYPE_UNIX_FILE, *p);
	}
	for (p = ext7; *p != 0; p++) {
		add_extension(RES_TYPE_CD_IMAGE, *p);
	}
	for (p = ext8; *p != 0; p++) {
		add_extension(RES_TYPE_CD_INDEX, *p);
	}
	for (p = ext9; *p != 0; p++) {
		add_extension(RES_TYPE_ARCHIVE, *p);
	}
	for (p = ext10; *p != 0; p++) {
		add_extension(RES_TYPE_WIN_EXE, *p);
	}
}

// get hash code from extension
CFileClassifier::hash_t CFileClassifier::hash_from_ext(const char *ext) const
{
	hash_t result = 0;
	for (const char *p = ext; *p; p++) {
		int ch = tolower(*p);
		result = result * 65599 + ch;
	}
	return result;
}

void CFileClassifier::add_extension(int res_type, const char *ext)
{
	// get code
	hash_t hash = hash_from_ext(ext);
	// insert into class map
	m_class_map[hash] = res_type;
}

int CFileClassifier::classify(const CFileRecord &r) const
{
	// get extension
	const char *ext = strrchr(r.name(), '.');
	if (ext == 0) {
		return RES_TYPE_UNKNOWN;
	}
	ext++;
	// check for extension length, prevent spurious matches
	if (strlen(ext) > 8) {
		return RES_TYPE_UNKNOWN;
	}
	// get hash
	hash_t hash = hash_from_ext(ext);
	// get resource type from class map
	std::map<hash_t, int>::const_iterator hash_iter = m_class_map.find(hash);
	if (hash_iter == m_class_map.end()) {
		return RES_TYPE_UNKNOWN;
	}
	int res_type = hash_iter->second;
	// check for size
	unsigned long long int size = r.size();
	// for movie
	if (res_type == RES_TYPE_MOVIE) {
		if (size < MIN_MOVIE_SIZE) {
			res_type = RES_TYPE_CLIP;
		}
	}
	// for CD image
	if (res_type == RES_TYPE_CD_IMAGE) {
		if (size < MIN_CD_IMAGE_SIZE) {
			res_type = RES_TYPE_UNKNOWN;
		}
	}
	if (res_type == RES_TYPE_CD_INDEX) {
		res_type = RES_TYPE_CD_IMAGE;
	}
	return res_type;
}
