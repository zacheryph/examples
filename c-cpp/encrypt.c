/* encrypt.c: simple encryption from sdbot */

#include <stdio.h>
#include <stdlib.h>

const int cryptkey = 14;

char * decryptstr(char *str, int strlen)
 {
 	int i;

	for (i = 0; i < strlen; ++i)
		str[i] = str[i] ^ (cryptkey + (i * (cryptkey % 10) + 1));

	return str;
}

int main (int argc, char **argv)
{
	char *line = "This line will be Encrypted!\0";
	int len = strlen (line);

	printf ("before:  \"%s\"\n", line);
	decryptstr (line, len);
	printf ("encrypt: \"%s\"\n", line);
	decryptstr (line, len);
	printf ("decrypt: \"%s\"\n", line);

	return 0;
}
