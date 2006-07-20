#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main (int argc, char **argv)
{
	struct stat finfo;
	
	if (argc != 2)
		return 0;

	if (stat (argv[1], &finfo))
		printf ("file: %s does NOT exists\n", argv[1]);

	return 0;
}
	      
