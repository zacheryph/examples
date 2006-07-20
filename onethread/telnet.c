/* telnetparty.c: telnet partyline interface
 *
 * Copyright (C) 2005 Zachery Hostens <source@
 * Copyright (C) 2003, 2004 Eggheads Development Team
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

#ifndef lint
static const char rcsid[] = "$Id: telnetparty.c,v 1.20 2005/11/14 04:44:43 wcc Exp $";
#endif

#include <stdio.h>
#include "sockbuf.h"
#include "telnet.h"

static int telnet_filter_read(void *client_data, int idx, char *data, int len);
static int telnet_filter_write(void *client_data, int idx, const char *data, int len);
static int telnet_filter_delete(void *client_data, int idx);

#define TELNET_FILTER_LEVEL	SOCKBUF_LEVEL_TEXT_ALTERATION

static sockbuf_filter_t telnet_filter = {
	"telnet filter", TELNET_FILTER_LEVEL,
	NULL, NULL, NULL,
	telnet_filter_read, telnet_filter_write, NULL,
	NULL, telnet_filter_delete
};

void telnet_mode(int idx, int cmd, int what)
{
	char temp[3];
	temp[0] = TELNET_CMD;
	temp[1] = cmd;
	temp[2] = what;
	sockbuf_on_write(idx, TELNET_FILTER_LEVEL, temp, 3);
}

static int telnet_filter_read(void *client_data, int idx, char *data, int len)
{
	unsigned char *cmd;
	int type, arg, remove;
	telnet_t *session = client_data;

	cmd = data;
	while ((cmd = (unsigned char *) memchr(cmd, TELNET_CMD, len))) {
		type = *(cmd+1);
		switch (type) {
			case TELNET_AYT:
				sockbuf_write(idx, "[YES]\r\n", 7);
				remove = 1;
				break;
			case TELNET_CMD:
				cmd++;
				remove = 1;
				break;
			case TELNET_DO:
				arg = *(cmd+2);
				if (arg & (TELNET_ECHO | TELNET_SUPRESS_GA)) {
					if (!(session->flags & TFLAG_ECHO)) {
						telnet_mode(idx, TELNET_WILL, arg);
						session->flags |= TFLAG_ECHO;
					}
				}
				else telnet_mode(idx, TELNET_WONT, arg);
				remove = 3;
				break;
			case TELNET_DONT:
				arg = *(cmd+2);
				if (arg == TELNET_ECHO) {
					if (session->flags & TFLAG_ECHO) {
						telnet_mode(idx, TELNET_WONT, arg);
						session->flags &= ~TFLAG_ECHO;
					}
				}
				remove = 3;
				break;
			case TELNET_WILL:
				remove = 3;
				break;
			case TELNET_WONT:
				remove = 3;
				break;
			default:
				remove = 2;
		}
		memmove(cmd, cmd+remove, len - (cmd - (unsigned char *)data) - remove + 1);
		len -= remove;
	}

	if (len) {
		if (session->flags & TFLAG_ECHO && !(session->flags & TFLAG_PASSWORD)) {
			//sockbuf_on_write(idx, TELNET_FILTER_LEVEL, data, len);
			telnet_filter_write(client_data, idx, data, len);
		}
		sockbuf_on_read(idx, TELNET_FILTER_LEVEL, data, len);
	}
	return(0);
}

/* We replace \n with \r\n and \255 with \255\255. */
static int telnet_filter_write(void *client_data, int idx, const char *data, int len)
{
	char *newline;
	const char *filterchars = "\n\r\002\007\017\026\037\255";
	char temp[64];
	int i, cur, max, n, code, esc;

	cur = 0;
	max = 128;
	newline = malloc(max+1);

	code = 0;
	esc = -1;
	while (len > 0) {
		switch (*data) {
			case '\000':
				if ('\r' == *(data - 1)) {
					egg_append_str(&newline, &cur, &max, "\r\n");
					data++;
					len--;
				}
				break;
			case '\002':
				esc = 1;
				break;
			case '\007':
				/* Skip. */
				data++; len--;
				break;
			case '\017':
				esc = 0;
				break;
			case '\026':
				esc = 7;
				break;
			case '\037':
				esc = 4;
				break;
			case '\r':
			case '\n':
				if (code) {
					/* If we've used an escape code, we
					 * append the 'reset' code right before
					 * the end of the line. */
					egg_append_str(&newline, &cur, &max, "\033[0m");
					code = 0;
				}
				if (*data != '\r') {
					/* Skip \r and wait for \n, where we
					 * add both (prevents stray \r's). */
					egg_append_str(&newline, &cur, &max, "\r\n");
				}
				data++;
				len--;
				break;
			case '\255':
				/* This is the telnet command char, so we have
				 * to escape it. */
				egg_append_str(&newline, &cur, &max, "\255\255");
				data++;
				len--;
				break;
			default:
				for (i = 0; i < sizeof(temp)-1 && i < len; i++) {
					if (strchr(filterchars, *data)) break;
					temp[i] = *data++;
				}
				temp[i] = 0;
				egg_append_str(&newline, &cur, &max, temp);
				len -= i;
		}

		if (esc != -1) {
			/* Add the escape code to the line. */
			sprintf(temp, "\033[%dm", esc);
			egg_append_str(&newline, &cur, &max, temp);
			data++; len--;
			esc = -1;

			/* Remember that we're in a code so we can reset it
			 * at the end of the line. */
			code = 1;
		}
	}
	newline[cur] = 0;
	if (cur > 0) {
		n = sockbuf_on_write(idx, TELNET_FILTER_LEVEL, newline, cur);
	}
	else n = 0;
	free(newline);
	return(n);
}

static int telnet_filter_delete(void *client_data, int idx)
{
	telnet_t *sess = client_data;
	free (sess);
	return(0);
}

int telnet_on(int idx, int flags)
{
  telnet_t *inf;

  inf = calloc(1, sizeof(*inf));
	inf->flags = flags;
  sockbuf_attach_filter(idx, &telnet_filter, inf);
  return(0);
}

int telnet_off(int idx)
{
  telnet_t *inf;
	sockbuf_detach_filter(idx, &telnet_filter, &inf);
	if (inf) free(inf);
  return(0);
}
