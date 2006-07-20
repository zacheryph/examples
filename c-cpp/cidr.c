/* cidr.c: print ip cidr
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
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BROADCAST 0xffffffff

int printbin(const char *msg, long num) {
	long counter;

	printf ("%s: ", msg);
	
	for (counter=31; counter>=0; counter--)
		printf("%d", (num >> counter) & 1);
	putchar('\n');
}

int main (int argc, char **argv)
{
	char *mask;
	int maskbits;
	struct in_addr addr;
	long hostaddr;
	struct in_addr network;
	struct in_addr netmask;
	struct in_addr broadcast;

	if (argc < 2 || NULL == (mask = strchr(argv[1], '/'))) {
		printf ("usage: %s ip.ad.dr.ip/bitmask\n", argv[0]);
		_exit (0);
	}

	*mask++ = '\0';
	maskbits = atoi(mask);
	if (!inet_aton(argv[1], &addr) || maskbits > 30) {
		printf ("cidr: error in your input\n");
		_exit (0);
	}
	hostaddr = ntohl(addr.s_addr);

	netmask.s_addr = ntohl(BROADCAST << (32-maskbits));
	//printbin("netmask:   ", netmask.s_addr);
	network.s_addr = addr.s_addr & netmask.s_addr;
	//printbin("network:   ", network.s_addr);
	broadcast.s_addr = network.s_addr | ntohl(BROADCAST >> maskbits);
	//printbin("broadcast: ", broadcast.s_addr);
	
	printf (" cidr: %s/%d\n", inet_ntoa(addr), maskbits);
	printf (" conf: %s ", inet_ntoa(addr));
	printf ("%s\n", inet_ntoa(netmask));
	printf ("range: %s - ", inet_ntoa(network));
	printf ("%s (%d usable)\n", inet_ntoa(broadcast), (BROADCAST >> maskbits)-1);
/*	
	printf ("cidr: ip(%s)\n", inet_ntoa(addr));
	printf ("cidr: netmask(%s)\n", inet_ntoa(netmask));
	printf ("cidr: network(%s)\n", inet_ntoa(network));
	printf ("cidr: broadcast(%s)\n", inet_ntoa(broadcast));
	printf ("cidr: usable hosts: %d\n", (BROADCAST >> maskbits)-1);
*/
	return 0;
}

