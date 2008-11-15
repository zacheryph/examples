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

#include "eggnet.h"

static int serv1_port = 4395;
static int serv2_port = 4396;
static int serv3_port = 5000;

typedef struct multidata_s {
	char *last;
} multidata_t;


// service 1
static int servone_on_connect (void *client_data, int idx, int newidx, const char *peer_ip, int peer_port);
static int servone_on_eof (void *client_data, int idx, int err, const char *errmsg);
static int servone_on_read (void *client_data, int idx, char *data, int len);

static sockbuf_handler_t servone_handler = {
        "server",
        NULL, servone_on_eof, servone_on_connect,
        servone_on_read, NULL, NULL
};

static int servone_on_connect (void *client_data, int idx, int newidx, const char *peer_ip, int peer_port) {
	warnx ("ONE: connection accepted (%d): %s:%d", sockbuf_get_sock(newidx), peer_ip, peer_port);
	sockbuf_set_handler(newidx, &servone_handler, client_data);
	return 0;
}
static int servone_on_eof (void *client_data, int idx, int err, const char *errmsg) {
	warnx ("ONE: connection closed (%d) error: %s", sockbuf_get_sock(idx), errmsg);
	return 0;
}
static int servone_on_read (void *client_data, int idx, char *data, int len) {
	multidata_t *cd = (multidata_t *) client_data;
	warnx ("ONE: data recieved (%d): %*s | last service: %s", len, len, data, cd->last);
	cd->last = "ONE";
	return 0;
}

// service 2
static int servtwo_on_connect (void *client_data, int idx, int newidx, const char *peer_ip, int peer_port);
static int servtwo_on_eof (void *client_data, int idx, int err, const char *errmsg);
static int servtwo_on_read (void *client_data, int idx, char *data, int len);

static sockbuf_handler_t servtwo_handler = {
        "server",
        NULL, servtwo_on_eof, servtwo_on_connect,
        servtwo_on_read, NULL, NULL
};

static int servtwo_on_connect (void *client_data, int idx, int newidx, const char *peer_ip, int peer_port) {
  warnx ("two: connection accepted (%d): %s:%d", sockbuf_get_sock(newidx), peer_ip, peer_port);
	sockbuf_set_handler(newidx, &servtwo_handler, client_data);
	return 0;
}
static int servtwo_on_eof (void *client_data, int idx, int err, const char *errmsg) {
  warnx ("two: connection closed (%d) error: %s", sockbuf_get_sock(idx), errmsg);
	return 0;
}
static int servtwo_on_read (void *client_data, int idx, char *data, int len) {
	multidata_t *cd = (multidata_t *) client_data;
	warnx ("two: data recieved (%d): %*s | last service: %s", len, len, data, cd->last);
	cd->last = "two";		
	return 0;
}

// service 3
static int servthr_on_connect (void *client_data, int idx, int newidx, const char *peer_ip, int peer_port);
static int servthr_on_eof (void *client_data, int idx, int err, const char *errmsg);
static int servthr_on_read (void *client_data, int idx, char *data, int len);

static sockbuf_handler_t servthr_handler = {
        "server",
        NULL, servthr_on_eof, servthr_on_connect,
        servthr_on_read, NULL, NULL
};

static int servthr_on_connect (void *client_data, int idx, int newidx, const char *peer_ip, int peer_port) {
  warnx ("(3): connection accepted (%d): %s:%d", sockbuf_get_sock(newidx), peer_ip, peer_port);
	sockbuf_set_handler(newidx, &servthr_handler, client_data);
	return 0;
}
static int servthr_on_eof (void *client_data, int idx, int err, const char *errmsg) {
  warnx ("(3): connection closed (%d) error: %s", sockbuf_get_sock(idx), errmsg);
	return 0;
}
static int servthr_on_read (void *client_data, int idx, char *data, int len) {
  multidata_t *cd = (multidata_t *) client_data;
	warnx ("(3): data recieved (%d): %*s | last service: %s", len, len, data, cd->last);
	cd->last = "(3)";
	return 0;
}


// MAIN //
int main (int argc, char **argv)
{
	int sbuf,sock;
	multidata_t cdata;
	cdata.last = "NONE";

	sockbuf_init();

	sbuf = sockbuf_new ();
	sock = socket_create ( NULL, 0, "127.0.0.1", serv1_port, SOCKET_SERVER|SOCKET_BIND|SOCKET_NONBLOCK|SOCKET_TCP);
	if (0 > sbuf || 0 > sock)
		errx (1, "failed to start service 1");
	sockbuf_set_sock(sbuf, sock, SOCKBUF_SERVER);
	sockbuf_set_handler(sbuf, &servone_handler, &cdata);

	sbuf = sockbuf_new ();
  sock = socket_create ( NULL, 0, "127.0.0.1", serv2_port, SOCKET_SERVER|SOCKET_BIND|SOCKET_NONBLOCK|SOCKET_TCP);
  if (0 > sbuf || 0 > sock)
    errx (1, "failed to start service 2");
  sockbuf_set_sock(sbuf, sock, SOCKBUF_SERVER);
	sockbuf_set_handler(sbuf, &servtwo_handler, &cdata);

	sbuf = sockbuf_new ();
  sock = socket_create ( NULL, 0, "127.0.0.1", serv3_port, SOCKET_SERVER|SOCKET_BIND|SOCKET_NONBLOCK|SOCKET_TCP);
  if (0 > sbuf || 0 > sock)
    errx (1, "failed to start service 3");
  sockbuf_set_sock(sbuf, sock, SOCKBUF_SERVER);
	sockbuf_set_handler(sbuf, &servthr_handler, &cdata);

	for (;;)
		sockbuf_update_all(3);

	return 0;
}
