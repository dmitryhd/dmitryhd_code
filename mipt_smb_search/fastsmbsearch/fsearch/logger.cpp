/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: logger.cpp,v 1.5 2005/09/08 18:52:52 alex_vodom Exp $
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "logger.h"

CLogger* logger;

CLogger::CLogger(const char *file_name)
{
	m_pid = getpid();
	m_level = 100;
	m_file = 0;
	m_buf_size = 200;
	m_buf = new char[m_buf_size];
	m_file = fopen(file_name, "a");
	//m_file = fdopen(1, "a");
	if (m_file == 0) {
		throw std::runtime_error(std::string("Cannot open log file ") + file_name + " for writing");
	}
}

CLogger::~CLogger()
{
	if (m_file) {
		fclose(m_file);
		m_file = 0;
	}
	delete[] m_buf;
	m_buf = 0;
	m_pid = 0;
}

void CLogger::log(int level, const char *format, ...)
{
	va_list args;
	struct tm *newtime;
	time_t aclock;
	char *t;
	
	if (level > m_level) return;
	va_start(args, format);
	while (vsnprintf(m_buf, m_buf_size, format, args) >= m_buf_size) {
		delete[] m_buf;
		m_buf_size *= 2;
		m_buf = new char[m_buf_size];
	}
	va_end(args);
	
	time( &aclock );
	newtime = localtime( &aclock );
	t = asctime( newtime );
	t[19] = 0;
	fprintf(m_file, "%s: [%d] %s\n", t, m_pid, m_buf);
	// flush
	fflush(m_file);
}

void CLogger::set_level(int level)
{
	if ((level < 0) || (level > 255)) {
		log(0, "attempt to set invalid log level %d", level);
		return;
	}
	m_level = level;
}

int CLogger::get_level()
{
	return m_level;
}
