/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: swapfile.cpp,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "logger.h"
#include "memalloc.h"
#include "swapfile.h"

static const long page_size = 4096;

CSwapFile::CSwapFile(const std::string& name, unsigned long size)
{
	m_fd = -1;
	m_base = 0;
	m_size = 0;

	m_fd = ::open(name.c_str(), O_RDWR|O_CREAT, 0600);
	if (m_fd < 0) {
		std::ostringstream err;
		err << "Cannot open swap file " + name;
		throw std::runtime_error(err.str());
	}
	if (ftruncate(m_fd, size) != 0) {
		std::ostringstream err;
		err << "Cannot truncate file " << name << " to length " << size;
		throw std::runtime_error(err.str());
	}
	m_size = size;
}

CSwapFile::~CSwapFile()
{
	m_base = 0;
	m_size = 0;
	::close(m_fd);
	m_fd = 0;
}

void CSwapFile::read(void *buf, long offset, long size)
{
	::lseek(m_fd, offset, SEEK_SET);
	::read(m_fd, buf, size);
}

void CSwapFile::write(void *buf, long offset, long size)
{
	::lseek(m_fd, offset, SEEK_SET);
	::write(m_fd, buf, size);
}
