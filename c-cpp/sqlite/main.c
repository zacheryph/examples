/* sqlite/main.c: sqlite example
 *
 * Zachery Hostens - 20040331 - extort ! #god @ EFnet -
 *
 * this source code is in the open domain is not
 * guaranteed, but may be used however you choose
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite.h>

/* this example just does a select * from a pre made table
 * either add an sqlite_exec to create a table or premake
 * it with the shell program sqlite
 */
#define DB_TABLE "tcb"

/* callback function
 * param void *cd: client data that can be passed from sqlite_exec()
 */
int db_callback (void *cd, int argc, char **argv, char **columns);

int main (int argc, char **argv)
{
	int err;
	char *errs;
	sqlite *db = NULL;

	if (argc != 2) {
		printf ("* usage: %s <database file>\n", argv[0]);
		exit (0);
	}

	/* open the database file */
	db = sqlite_open (argv[1], 0, &errs);

	if (db == NULL) {
		printf ("! sqlite_open: %s\n", errs);
		exit (0);
	}

	printf ("* successfully opened %s\n"
			"* querying table: %s\n", argv[1], DB_TABLE);

	/* query the database, the NULL is the first param of our callback function */
	err = sqlite_exec (db, "SELECT * FROM tcb;", db_callback, NULL, &errs);

	if (err != 0) {
		printf ("! query failed: %s\n", errs);
		exit (0);
	}

	/* close the database */
	sqlite_close (db);

	return 0;
}

int db_callback (void *cd, int argc, char **argv, char **columns)
{
	int i;
	for (i = 0; i < argc; i++)
		printf ("   %s --> %s\n", columns[i], argv[i]);

	printf ("\n");
	return 0;
}
