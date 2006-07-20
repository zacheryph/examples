#include <stdlib.h>
#include <stdio.h>

int main (int argc, char **argv)
{
	int num;

	if (argc < 2) {
		printf ("usage: %s <number|string>\n", argv[0]);
		return 0;
	}

	num = atoi (argv[1]);

	if (num == 0)
		printf ("param was 0 or a string\n");
	else
		printf ("param was a number: %i\n", num);

	return 0;
}
