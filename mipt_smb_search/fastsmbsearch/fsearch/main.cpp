/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: main.cpp,v 1.8 2005/09/08 18:52:52 alex_vodom Exp $
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

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <locale>
#include <errno.h>
#include <signal.h>
#include "logger.h"
#include "swapalloc.h"
#include "uppercase.h"
#include "classify.h"
#include "server.h"

static char socket_name[] = "/tmp/fastsmb.sock";
static char log_file_name[] = "fsearch.log";
static char swap_file[] = "swapfile";
static long swap_size = 1024*1024*1024;
static long max_rss = 25000; // in pages

extern int caught_sighterm;
int caught_sigterm = 0;

void signal_handler(int signo)
{
	if (signo == SIGTERM) {
		caught_sigterm = 1;
	}
	logger->log(2, "Got signal %d", signo);
}

int main(int, char **)
{
	struct sigaction sig_act;
	sig_act.sa_handler = signal_handler;
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sigaction(SIGHUP, &sig_act, 0);
	sigaction(SIGTERM, &sig_act, 0);
	sigaction(SIGINT, &sig_act, 0);
	sigaction(SIGPIPE, &sig_act, 0);
	sigaction(SIGHUP, &sig_act, 0);
	sigaction(SIGTTIN, &sig_act, 0);
	sigaction(SIGTTOU, &sig_act, 0);
	setlocale(LC_ALL, "");

	uppercase_init();
	try {
		logger = new CLogger(log_file_name);
		swapAllocator.init(swap_file, swap_size, max_rss);

		CServer server;
		server.listen(socket_name);
		server.run();
		printf("Server: %d queries\n", server.request_count());
	}
	catch (std::runtime_error &e) {
		logger->log(0, "%s", e.what());
		logger->log(0, "Run-time error occured, terminating");
	}
	catch (std::exception& e) {
		logger->log(0, "Unknown exception caught: %s", e.what());
	}
	swapAllocator.finish();
	delete logger;
	return 0;
}
