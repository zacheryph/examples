/* readline example
 *
 * compile: gcc [-I/opt/local/include] -o rl readline.c [-L/opt/local/lib] -lreadline
 * run: ./rl
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#define CMD(x) int x (char *args)
typedef int (*cmd_callback) (char *args);

typedef struct {
	char *cmd;
	cmd_callback callback;
	char *usage;
} cmd_t;

CMD(cmd_help);
CMD(cmd_shell);
CMD(cmd_exit);
char **shutils_completor (const char *cmd, int start, int end);
char *cmd_list_generator (const char *cmd, int state);
cmd_t *cmd_find (char *cmd);

cmd_t cmd_list[] = {
	{ "help", cmd_help, "help [command]: display help" },
	{ "shell", cmd_shell, "shell <server>: change shell server" },
	{ "exit", cmd_exit, "exit: exit shutils" },
	{ "quit", cmd_exit, "quit: exit shutils" },
	{ NULL, NULL, NULL }
};

static char *prompt = NULL;

int main (int argc, char **argv)
{
  char *line, *tokline, *cmd, *args;
	cmd_t *curcmd;
	cmd_shell ("local");

	/* initialize readline */
	rl_attempted_completion_function = shutils_completor;

  while (line = readline (prompt)) {
		tokline = strdup(line);
		cmd = strtok(tokline, " \t");
		args = strtok(NULL, "\0");
    if (!strncasecmp("exit", cmd, 4)) exit(0);

		if (NULL != (curcmd = cmd_find(cmd))) {
	    add_history (line);
			curcmd->callback (args);
		}
		free (line);
		free (tokline);
  }

  return 0;
}

char **shutils_completor (const char *cmd, int start, int end)
{
	char **match;
	match = (char **) NULL;

	if (0 == start)
		match = rl_completion_matches (cmd, cmd_list_generator);

	return match;
}

char *cmd_list_generator (const char *cmd, int state)
{
	static int i, len;
	char *name;

	if (!state) {
		i = 0;
		len = strlen(cmd);
	}

	while (name = cmd_list[i].cmd) {
		i++;

		if (!strncasecmp (cmd, name, len))
			return (strdup(name));
	}
	return ((char *)NULL);
}

cmd_t *cmd_find (char *cmd)
{
	int i;

	for (i = 0; cmd_list[i].cmd; i++)
		if (!strncasecmp(cmd, cmd_list[i].cmd, strlen(cmd_list[i].cmd)))
			return &cmd_list[i];

	return NULL;
}

CMD(cmd_help)
{
	int i;

	for (i = 0; cmd_list[i].cmd; i++)
		if (NULL == args || !strncasecmp (args, cmd_list[i].cmd, strlen(args)))
			printf ("%s\n", cmd_list[i].usage);
	return 0;
}

CMD(cmd_shell)
{
	int len;
	if (!args) {
		cmd_help ("shell");
		return 1;
	}
	if (prompt) free(prompt);
	len = 11 + strlen(args);
	prompt = malloc(len);
	snprintf (prompt, len, "shutils:%s# ", args);
	return 0;
}

CMD(cmd_exit)
{
	exit (0);
}
