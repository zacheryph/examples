// example showing memmove through what could be a text buffer.
// loops through until reaching end with no \n.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv)
{
	char *msg, *cur;
	msg = (char *) calloc(1, 128);

	snprintf (msg, 128, "some %s\ntext %s\nor another", "on the first line", "on the second");

	while (NULL != (cur = strchr(msg, '\n'))) {
		printf ("--- msg ---\n%s\n--- cur ---\n%s\n-----------\n", msg, ++cur);
		memmove (msg, cur, 128-(cur-msg));
	}

	printf ("--- msg ---\n%s\n", msg);
}
