/* scotty.c: play with scotty's toys
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
/* TODO
 * ~ use callbacks on net_connect/net_listen
 */
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

/* sock_open("ip.ad.dr.ess", port)
 * returns: -1:fail +:socket fd (success)
 * note: close the fd when your done with it!
 */
int net_connect (const char *ipaddr, unsigned int port)
{
	int sockfd;
	struct sockaddr_in raddr;

	if (-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0)))
		return -1;

	if (0 == inet_aton(ipaddr, &(raddr.sin_addr))) {
		close (sockfd);
		return -1;
	}

	memset(&(raddr.sin_zero), '\0', 8);
	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(port);
	
	if (-1 == connect(sockfd, (struct sockaddr *) &raddr, sizeof (struct sockaddr))) {
		close (sockfd);
		return -1;
	}

	// yey we connected
	return sockfd;
}

int net_listen (const char *ipaddr, unsigned int port)
{
	int sockfd;
	struct sockaddr_in laddr;

	memset(&(laddr.sin_zero), '\0', 8);
	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(port);

	if (NULL == ipaddr || INADDR_NONE == (laddr.sin_addr.s_addr = inet_addr(ipaddr)))
		laddr.sin_addr.s_addr = INADDR_ANY;

	if (-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0)))
		return -1;

	if (-1 == bind(sockfd, (struct sockaddr *) &laddr, sizeof(struct sockaddr))
			|| -1 == listen(sockfd, 3) ) {
		close(sockfd);
		return -1;
	}

	return sockfd;
}
