/*
 * RubbiSH Command Line Interpreter
 * Keiya Chinen <s1011420@coins.tsukuba.ac.jp>
 * */
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "rubbish.h"
#include "rubgc.h"
#include <signal.h>
#include "execute.h"
#define MAX_HISTORY_CNT 3
#define MAX_ARGS_CNT 100

/* input buffer, cursor */
char *input;
int input_cur;

/* Garbage collector context */
void *gc_ctx;

void sigint_action(int signum, siginfo_t *info, void *ctx)
{
	printf("sigint_handler(%d): signo(%d) code(0x%x)\n",
		signum, info->si_signo, info->si_code);
}

void set_sighandler()
{
	struct sigaction sa_sigint;
	
	memset(&sa_sigint, 0, sizeof(sa_sigint));
	sa_sigint.sa_sigaction = sigint_action;
	sa_sigint.sa_flags = SA_RESTART | SA_SIGINFO;
	
	if (sigaction(SIGINT, &sa_sigint, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}
}

void show_prompt(char *buf)
{
	char cwd_str[128];
	getcwd(&cwd_str,128);
	snprintf(buf,128,"[%s](RubbiSH)> ",cwd_str);
}

int main()
{
	char prompt[128] = "$ ";
	int history_no = 0;
	HIST_ENTRY *history = NULL;

	set_sighandler();

	show_prompt(prompt);
	while (input = readline(prompt))
	{
		gc_ctx = rgc_init();
		show_prompt(prompt);

		/* add to readline history */
		add_history(input);

		/* parse: see lexer.c, parser.y, semantic.c */
		input_cur = 0;
		yyparse();

		/* execute */
		run(parsed_semantic);
		parsed_semantic = NULL; /* clear old semantic (abstract syntax) */

		rgc_free_all(gc_ctx); /* RubbishGC clean up. free all allocated memory */

		/* clear old readline history */
		if (++history_no > MAX_HISTORY_CNT)
		{
			history = remove_history(0);
			free(history);
		}
	}
	return 0;
}

