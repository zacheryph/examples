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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include "network.h"

#define SOCKSIZE sizeof(struct sockaddr_in)

#define MAXCLIENTS 128
#define SERVERIP "127.0.0.1"
#define SERVERPORT 4532

int main (int argc, char **argv)
{
	int serv, ret, cfd, sbuf;
	struct pollfd fds[128];
	int nfds = 1;
	struct sockaddr_in caddr;
	int sinsize;
	char *rbuf;
	
	rbuf = (char *) malloc(1024);

	serv = net_listen(SERVERIP, SERVERPORT);
	if (-1 == serv)
		err(1, "failed to open listening socket.");

	fds[0].fd = serv;
	fds[0].events = POLLIN;
	
	warnx ("entering infiniloop");
	for (;;) {
		ret = poll (fds, nfds, 2*1000);
		if (-1 == ret || (fds[0].revents & (POLLERR|POLLHUP|POLLNVAL)))
			errx(1, "failed poll()ing");

		if (0 == ret) {
			warnx ("(%d) no data for 2 seconds, continue", nfds);
			continue;
		}

		if (fds[0].revents & POLLIN) {
			sinsize = sizeof(struct sockaddr_in);
			fds[nfds].fd = accept(serv, (struct sockaddr *) &caddr, &sinsize);
			if (-1 == fds[nfds].fd)
				warnx("error accepting connection");
			else {
				warnx("accepted connection from: %s:%d", inet_ntoa(caddr.sin_addr), caddr.sin_port);
				fds[nfds++].events = POLLIN;
			}
		}

		for (cfd = 1; cfd < nfds; ++cfd) {
			if (fds[cfd].revents & POLLIN) {
				sbuf = recv(fds[cfd].fd, rbuf, 1023, 0);
				rbuf[sbuf] = '\0';
				if (0 == sbuf)
					fds[cfd].revents |= POLLHUP;
				else
					warnx("recv(%d) [%d[ %*s ]] %d", fds[cfd].fd, sbuf, sbuf, rbuf, fds[cfd].revents);
			}
		}

    for(cfd = 1; cfd < nfds; ++cfd) {
      if (fds[cfd].revents & POLLHUP) {
				warnx ("connection closed on fd: %d", fds[cfd].fd);
        close(fds[cfd].fd);
				fds[cfd].fd = -1;
      }
    }

	}

	return 0;	
}
