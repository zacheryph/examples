/* multiserv.c: multiple-service/one-thread example
 *
 * Copyright (C) 2005 Zachery Hostens <zacheryph@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "network.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 4532

int main (int argc, char **argv)
{
	int sock, i, rnum, ret, port;
	pid_t pid = getpid();
	char *msg = (char *) malloc(128);

	srand(rand()*pid);

	if (argc > 1)
		port = atoi(argv[1]);
	else
		port = SERVERPORT;

	sock = net_connect (SERVERIP, port);
	if (-1 == sock)
		err (1, "error during connection");

	warnx ("successfully connected to: %s:%d", SERVERIP, port);
	// do 10 rounds
	for (i = 0; i < 5; ++i) {
		rnum = (rand() % 10) + 1;
		warnx ("(%d) sleeping for %d", pid, rnum);
		sleep(rnum);
		ret = snprintf (msg, 128, "pid(%d) slept %dsec", pid, rnum);
		ret = send(sock, msg, strlen(msg), 0);
		if (-1 == ret)
			err(1, "failed to send message");
	}
	close(sock);
	return 0;
}
