/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: fileread.cpp,v 1.6 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "fileread.h"
#include "logger.h"

CFileReader::CFileReader(const char *file_name)
{
	m_buf = 0;
	m_end = 0;
	m_line_begin = 0;
	m_line_end = 0;
	m_current_line = 0;
	m_lines_count = 0;

	int fd;
	struct stat file_stat;
	
	// get info about file (size & etc)
	if (stat(file_name, &file_stat) != 0) {
		throw std::runtime_error(std::string("File not found: ") + file_name + strerror(errno));
	}
	// get file size
	m_file_size = file_stat.st_size;
	if (m_file_size == 0) {
		throw std::runtime_error("Empty file, ignored");
	}
	// open file
	fd = ::open(file_name, O_RDONLY);
	if (fd < 0) {
		throw std::runtime_error(std::string("Cannot open file ") + file_name + strerror(errno));
	}
	// mmap file to memory
	m_buf = new char[m_file_size];
	if (!m_buf) {
		::close(fd);
		fd = 0;
		throw std::runtime_error(std::string("Cannot allocate memory to read file ") + file_name);
	}
	if (read(fd, m_buf, m_file_size) != m_file_size) {
		delete[] m_buf;
		m_buf = 0;
		::close(fd);
		fd = 0;
		throw std::runtime_error(std::string("Cannot read file ") + file_name);
	}
	// close file
	::close(fd);
	// set up counters and pointer
	m_lines_count = 0;
	m_end = m_buf + m_file_size;
	count_lines();
}

CFileReader::~CFileReader()
{
	close();
}

void CFileReader::rewind()
{
	m_cur_ptr = m_buf;
	m_current_line = 0;
	m_line_begin = 0;
	m_line_end = 0;
}

void CFileReader::count_lines()
{
	rewind();
	m_lines_count = 0;
	while (get_next()) {
		m_lines_count++;
	}
	rewind();
}

// close file
void CFileReader::close()
{
	// reset pointers to zero
	m_line_begin = 0;
	m_line_end = 0;
	if (m_buf) delete[] m_buf;
	m_buf = 0;
	m_end = 0;
}

// get next line from file
bool CFileReader::get_next()
{
	// if we at end of file, return eof
	if (m_cur_ptr >= m_end) {
		return false;
	}
	// set beginning
	m_line_begin = m_cur_ptr;
	// search for LF or eof
	while (m_cur_ptr < m_end && (*m_cur_ptr != '\n')) {
		m_cur_ptr++;
	}
	// set end
	m_line_end = m_cur_ptr;
	// go further
	m_cur_ptr++;
	m_current_line++;
	return true;
}
