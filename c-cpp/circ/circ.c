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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "circ.h"
#include "cnet.h"

#define CIRC_CONNECTED  0x01
#define CIRC_DESTROY    0x02

/* socket handler declerations */
int circ_on_connect (int sid, void *conn_data);
int circ_on_read (int sid, void *conn_data, char *data, int len);
int circ_on_eof (int sid, void *conn_data, int err);
int circ_on_close (int sid, void *conn_data);

cnet_handler_t circ_socket_handler = {
  circ_on_connect, circ_on_read, circ_on_eof, circ_on_close, NULL
};


typedef struct circ_session_t {
  int sid;
  int nsid;
  int flags;
  char *host;
  void *data;
  circ_message_t msg;
  circ_callback_t *callbacks;
} circ_session_t;


static circ_session_t *sessions = NULL;
static int nsessions = 0;


static circ_session_t *circ_fetch (int sid)
{
  if (sid < 0 || sid > nsessions) return NULL;
  return &sessions[sid];
}

static circ_message_t *circ_parse (circ_message_t *msg, char *text)
{
  char *space, *brk;

  memset(msg, '\0', sizeof(*msg));
  strncpy(msg->buffer, text, 512);
  text = msg->buffer;

  if (':' == *text) {
    msg->hostmask = text + 1;
    space = strchr(text, ' ');
    if (!space) return msg;
    *space = '\0';
    text = space + 1;

    msg->nick = strtok_r(msg->hostmask, "!", &brk);
    msg->ident = strtok_r(NULL, "@", &brk);
    msg->host = strtok_r(NULL, "", &brk);
  }

  msg->command = text;
  space = strchr(text, ' ');
  if (!space) return msg;
  *space = '\0';
  text = space + 1;

  while (*text && (*text != ':')) {
    space = strchr(text, ' ');
    msg->argv[msg->argc++] = text;
    if (!space || 18 == msg->argc) break;
    *space = '\0';
    text = space+1;
  }

  if (*text == ':' && 19 > msg->argc) msg->argv[msg->argc++] = text+1;
  return msg;
}

static int circ_process(int sid, char *buffer)
{
  circ_session_t *sess;
  circ_callback_t *cb;
  sess = circ_fetch(sid);

  circ_parse(&sess->msg, buffer);

	if (!sess->msg.command) return 0;

  for (cb = sess->callbacks; cb->command; cb++)
    if (!strcasecmp(sess->msg.command, cb->command)) break;

  if (!cb->command) return 0;
  return (cb->callback)(sid, sess->data, &sess->msg);
}

int circ_session_create (const char *host, circ_callback_t *callbacks, void *data)
{
  circ_session_t *sess;

  sessions = realloc(sessions, (nsessions+1) * sizeof(*sessions));
  if (!sessions) {
    fprintf(stderr, "ERROR: Failed to allocate circ_session_t struct (circ_session_create)\n");
    return -1;
  }

  sess = &sessions[nsessions];
  memset(sess, '\0', sizeof(*sess));
  if (host) sess->host = strdup(host);
  sess->sid = nsessions;
  sess->data = data;
  sess->callbacks = callbacks;
  nsessions++;
  return sess->sid;
}

int circ_session_destroy (int sid)
{
  circ_session_t *sess;
  if (NULL == (sess = circ_fetch(sid))) return -1;

  sess->flags |= CIRC_DESTROY;
  return circ_disconnect(sid, NULL);
}

int circ_session_valid (int sid)
{
  return circ_fetch(sid) ? 1 : 0;
}

void *circ_session_data (int sid)
{
  circ_session_t *sess;
  if (NULL == (sess = circ_fetch(sid))) return NULL;
  return sess->data;
}

int circ_connect (int sid, const char *host, int port)
{
  circ_session_t *sess;
  if (NULL == (sess = circ_fetch(sid))) return -1;
  if (sess->flags & CIRC_CONNECTED) return 0;
  if (-1 == (sess->nsid = cnet_connect(host, port, sess->host, 0))) return -1;
  cnet_handler(sess->nsid, &circ_socket_handler);
  cnet_conndata(sess->nsid, sess);
  cnet_linemode(sess->nsid, 1);
  return 1;
}

int circ_disconnect (int sid, const char *message)
{
  circ_session_t *sess;
  if (NULL == (sess = circ_fetch(sid))) return -1;

  sess->flags &= ~(CIRC_CONNECTED);
  circ_send(sid, "QUIT :%s", message);
  return 0;
}

int circ_send (int sid, const char *fmt, ...)
{
  int len, ret;
  char data[513];
  va_list args;
  circ_session_t *sess;
  if (NULL == (sess = circ_fetch(sid))) return -1;

  memset(data, '\0', 513);
  va_start(args, fmt);
  len = vsnprintf(data, 510, fmt, args);
  strcat(data, "\r\n");

  ret = cnet_write(sess->nsid, data, len+2);
  return len;
}


/* network handlers */
int circ_on_connect (int sid, void *conn_data)
{
  circ_session_t *sess;
  (void)sid;

  sess = conn_data;
  circ_process(sess->sid, "CONNECT");
  return 0;
}

int circ_on_read (int sid, void *conn_data, char *data, int len)
{
  circ_session_t *sess;
  (void)sid;
  (void)len;

  sess = conn_data;
  circ_process(sess->sid, data);
  return 0;
}

int circ_on_eof (int sid, void *conn_data, int err)
{
  circ_session_t *sess;
  (void)sid;
  (void)err;

  sess = conn_data;
  circ_process(sess->sid, "QUIT :Disconnected");
  return 0;
}

int circ_on_close (int sid, void *conn_data)
{
  circ_session_t *sess;
  (void)sid;

  sess = conn_data;
  circ_process(sess->sid, "QUIT :Socket Closed");
  return 0;
}
