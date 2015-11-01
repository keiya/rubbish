/*
 * RubbiSH Command Line Interpreter
 * Keiya Chinen <s1011420@coins.tsukuba.ac.jp>
 * */
#include <stdlib.h>
#include "semantic.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "rubbish.h"
#include "rubgc.h"
#define MAX_HISTORY_CNT 3

/* input buffer, cursor */
char *input;
int input_cur;

/* Garbage collector context */
void *gc_ctx;

void run(COMMAND* cmd)
{
	printf("runner:");
	REDIRECT *redir;
	if (cmd == NULL) return;
	do {
		printf("running:%p\n",cmd);
		if (cmd->elem != NULL)
		{
			/* redirect element */
			if (cmd->elem->redirect != NULL)
			{
				printf("  elem[R]:%p %p\n",cmd->elem,cmd->elem->redirect);
				printf("  %s\n",cmd->elem->redirect->file);
			}
			/* command element */
			else
			{
				printf("  elem[W]:%p %p\n",cmd->elem,cmd->elem->word);
				printf("  %s\n",cmd->elem->word);
			}
			/*
			do
			{
				printf("  elem:%p\n",elem);
			} while((elem = elem->next) != NULL);
			*/
		}
	} while((cmd = cmd->next) != NULL);
	//if (sem && sem->elem && sem->elem->word)
	//	printf("'%s'",sem->elem->word);
	printf("\n");
}

int main()
{
	char prompt[128] = "";
	int history_no = 0;
	HIST_ENTRY *history = NULL;
	while (input = readline(prompt))
	{
		gc_ctx = rgc_init();

		/* add to readline history */
		add_history(input);

		printf("[R]%s\n",input);

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

