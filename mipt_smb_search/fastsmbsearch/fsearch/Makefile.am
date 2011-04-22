# This file is part of Fast SMB Search
#
# Copyright 2001-2005   Alexander Vodomerov
#
# $Id: Makefile.am,v 1.8 2005/09/08 18:52:52 alex_vodom Exp $
#
# Fast SMB Search is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# Fast SMB Search is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

bin_PROGRAMS = fsearch

fsearch_CXXFLAGS = -O2 -ggdb3 -static -Wall -std=c++98 -pedantic -Wno-long-long -Werror

generated = qlang.tab.hpp qlang.tab.cpp lex.yy.cpp

fsearch_SOURCES = swapfile.h swapfile.cpp				\
		  swapalloc.h swapalloc.cpp				\
		  logger.h logger.cpp					\
		  uppercase.h uppercase.cpp				\
		  memalloc.h memalloc.cpp				\
		  util.h						\
		  hash.h						\
		  strheap.h strheap.cpp					\
		  wordhash.h wordhash.cpp				\
		  wordsplit.h wordsplit.cpp				\
		  record.h record.cpp					\
		  classify.h classify.cpp				\
		  fileread.h fileread.cpp				\
		  test.h test.cpp					\
		  fileset.h fileset.cpp					\
		  hostdata.h hostdata.cpp				\
		  hostset.h hostset.cpp					\
		  stdtest.h stdtest.cpp					\
		  stdhost.h stdhost.cpp					\
		  testoper.h 						\
		  cachingtest.h						\
		  parenttest.h parenttest.cpp				\
		  substr.h substr.cpp					\
		  statefilter.h						\
		  regexp.h regexp.cpp					\
		  cacheitem.h						\
		  stdcmpr.h stdcmpr.cpp					\
		  parser.h parser.cpp					\
		  qcache.h qcache.cpp					\
		  server.h server.cpp					\
		  main.cpp						\
		  $(generated)

lex.yy.cpp: qlang.l qlang.tab.hpp
	        flex -olex.yy.cpp qlang.l

qlang.tab.cpp: qlang.tab.hpp

qlang.tab.hpp: qlang.ypp
	        bison -v -d qlang.ypp -o qlang.tab.cpp

BUILT_SOURCES = $(generated)

EXTRA_DIST = qlang.l qlang.ypp

CLEANFILES = $(generated)
