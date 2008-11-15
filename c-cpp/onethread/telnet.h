/* telnetparty.h: header for telnetparty.c
 *
 * Copyright (C) 2003, 2004 Eggheads Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * $Id: telnetparty.h,v 1.5 2004/06/29 21:28:17 stdarg Exp $
 */
#ifndef _INC_TELNET_H_
#define _INC_TELNET_H_

/* Possible states of the connection. */
#define STATE_PROMPT 1
#define STATE_PASSWORD 2

/* Telnet strings for controlling echo behavior. */
#define TELNET_ECHO 1
#define TELNET_SUPRESS_GA 3
#define TELNET_AYT  246
#define TELNET_WILL 251
#define TELNET_WONT 252
#define TELNET_DO 253
#define TELNET_DONT 254
#define TELNET_CMD  255

/* Flags for sessions. */
#define STEALTH_LOGIN 1
#define TFLAG_ECHO  2
#define TFLAG_PASSWORD  4

typedef struct {
	int flags;
} telnet_t;

int telnet_on  (int idx, int flags);
int telnet_off (int idx);
void telnet_mode (int idx, int cmd, int what);

#endif /* !_INC_TELNET_H_ */
