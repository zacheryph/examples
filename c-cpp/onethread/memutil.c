/* memutil.c: some macros and functions for common operations with strings and
 *            memory in general
 *
 * Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004 Eggheads Development Team
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
static const char rcsid[] = "$Id: memutil.c,v 1.21 2005/05/31 03:35:07 stdarg Exp $";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "memutil.h"
#include "sockbuf.h"

void str_redup(char **str, const char *newstr)
{
	int len;

	if (!newstr) {
		if (*str) free(*str);
		*str = NULL;
		return;
	}
	len = strlen(newstr) + 1;
	*str = realloc(*str, len);
	memcpy(*str, newstr, len);
}

/* Append string to a dynamic buffer with reallocation. */
void egg_append_str(char **dest, int *cur, int *max, const char *src)
{
  int len;

  if (!src) return;

  len = strlen(src);
  if (*cur + len + 10 > *max) {
    *max += len + 128;
    *dest = realloc(*dest, *max+1);
  }
  memmove(*dest + *cur, src, len);
  *cur += len;
}

char *egg_mprintf(const char *format, ...)
{
	va_list args;
	char *ptr;

	va_start(args, format);
	ptr = egg_mvsprintf(NULL, 0, NULL, format, args);
	va_end(args);
	return(ptr);
}

char *egg_msprintf(char *buf, int len, int *final_len, const char *format, ...)
{
	va_list args;
	char *ptr;

	va_start(args, format);
	ptr = egg_mvsprintf(buf, len, final_len, format, args);
	va_end(args);
	return(ptr);
}

char *egg_mvsprintf(char *buf, int len, int *final_len, const char *format, va_list args)
{
	char *output;
	int n;

	if (buf && len > 10) output = buf;
	else {
		output = malloc(512);
		len = 512;
	}
	while (1) {
		len -= 10;
		n = vsnprintf(output, len, format, args);
		if (n > -1 && n < len) break;
		if (n > len) len = n+1;
		else len *= 2;
		len += 10;
		if (output == buf) output = malloc(len);
		else output = realloc(output, len);
		if (!output) return(NULL);
	}
	if (final_len) {
		if (!(n % 1024)) n = strlen(output);
		*final_len = n;
	}
	if (output != buf && len - n > 256) output = realloc(output, n+20);
	return(output);
}

/* Print to an idx, using printf notation. */
int iprintf(int idx, const char *format, ...)
{
  char *ptr, buf[1024];
  int len;
  va_list args;

  va_start(args, format);
  ptr = egg_mvsprintf(buf, sizeof(buf), &len, format, args);
  va_end(args);
  sockbuf_write(idx, ptr, len);
  if (ptr != buf) free(ptr);
  return(len);
}
