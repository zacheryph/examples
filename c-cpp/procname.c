#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define PROCNAME "plastik"

int main (int argc, char **argv)
{
	char *procname = "plastik";
	pid_t pid;

	printf ("current procname: %s\n", argv[0]);
	printf ("resetting procname\n");
	printf ("argv[0] len: %d\n", strlen (argv[0]));
	strcpy (argv[0], "           ");
	strcpy (argv[0], PROCNAME);
	//argv[0] = procname; /* doesnt work */
	//signal (SIGCHLD, SIG_IGN); /* not required */
	printf ("current procname: %s\n", argv[0]);
	pid = getpid ();
	printf ("current pid: %d\n", pid);
	
	for (;;) { sleep (10); }
	
	return (0);
}
