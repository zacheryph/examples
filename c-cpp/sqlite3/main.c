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
#include <sqlite3.h>

#define HAS_AUTHORS 0x01
#define HAS_QUOTES  0x02

typedef struct dbstate_s {
  int flags;
} dbstate_t;

/* fetched quotes callback */
int print_quote_cb (void *cd, int argc, char **argv, char **columns)
{
  printf ("%4u -- %s (%s)\n  -- %s\n", atoi(argv[0]), argv[1], argv[3], argv[2]);
  return 0;
}

void die (const char *msg, sqlite3 *db)
{
  printf ("ERROR: %s\n  --> %s\n", msg, sqlite3_errmsg(db));
  exit (1);
}


int main (int argc, char **argv)
{
	int err = 0;
	char *errs = NULL;
	sqlite3 *db = NULL;
  dbstate_t state = { 0 };

	if (argc != 2) {
		printf ("* usage: %s <database file>\n", argv[0]);
		exit (0);
	}

	/* open the database file */
	if (sqlite3_open(argv[1], &db)) {
    printf ("Failed to open database file: %s\n  --> %s\n", argv[1], sqlite3_errmsg(db));
    exit (1);
	}
  printf ("Success: Opened Database file: %s\n", argv[1]);

  /* create our table if it does not exist and populate it */
  err = sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS authors ("
                           "  id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                           "  nick VARCHAR(9),"
                           "  name VARCHAR(64),"
                           "  admin BOOLEAN );", NULL, NULL, &errs);
  if (err) die(errs, db);
  err = sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS quotes ("
                           "  id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                           "  author_id INTEGER NOT NULL,"
                           "  quote TEXT,"
                           "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP );", NULL, NULL, &errs);
 if (err) die(errs, db);

  /* run our query */
 err = sqlite3_exec (db, "SELECT q.id,a.nick,q.quote,q.created_at"
                          "  FROM authors a, quotes q"
                          "  WHERE q.author_id = a.id", print_quote_cb, NULL, &errs);
 if (err) die(errs, db);

	/* close the database */
	sqlite3_close (db);

	return 0;
}
