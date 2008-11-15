/* Copyright (c) 2008, Zachery Hostens <zacheryph@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef CONTEXT_IRC_H_
#define CONTEXT_IRC_H_

#ifndef __GNUC__
# define __attribute__(x) /* NOTHING */
#endif

typedef struct circ_message_t {
  char buffer[513];
  char *command;
  char *hostmask;
  char *nick;
  char *ident;
  char *host;
  char *argv[19];
  int argc;
} circ_message_t;

typedef int (*event_callback) (int sid, void *data, circ_message_t *message);

typedef struct circ_callback_t {
  char *command;
  event_callback callback;
} circ_callback_t;


#define circ_update(i) cnet_select(i)

int circ_session_create (const char *host, circ_callback_t *callbacks, void *data);
int circ_session_destroy (int sid);
int circ_session_valid (int sid);
void *circ_session_data (int sid);
int circ_connect (int sid, const char *host, int port);
int circ_disconnect (int sid, const char *message);
int circ_send (int sid, const char *fmt, ...)
      __attribute__((format(printf,2,3)));

#endif /* CONTEXT_IRC_H_ */
