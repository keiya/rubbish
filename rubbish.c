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
#include "execute.h"
#define MAX_HISTORY_CNT 3
#define MAX_ARGS_CNT 100

/* input buffer, cursor */
char *input;
int input_cur;

/* Garbage collector context */
void *gc_ctx;

void run(COMMAND* cmd)
{
	printf("== runner ==\n");
	REDIRECT **redirects;
	redirects = (REDIRECT *)rgc_malloc(gc_ctx, sizeof(REDIRECT *) * 3);
	redirects[0] = NULL; /* fd=0 stdin */
	redirects[1] = NULL; /* fd=1 stdout */
	redirects[2] = NULL; /* fd=2 stderr */
	int redir_out_append = 0; /* append(1) or overwrite(0) */

	char **cmd_ary;
	cmd_ary = (char *)rgc_malloc(gc_ctx, sizeof(char *) * MAX_ARGS_CNT);
	int cmd_cnt = 0;
	if (cmd == NULL) return;
	REDIRECT *redir;
	do {
		printf("running:%p\n",cmd);
		if (cmd->elem != NULL)
		{

			/* command element */
			if (cmd->elem->redirect == NULL)
			{
				size_t cmdlen = strlen(cmd->elem->word);
				cmd_ary[cmd_cnt] = (char *)rgc_malloc(gc_ctx, cmdlen + 1);
				strcpy(cmd_ary[cmd_cnt],cmd->elem->word);
				cmd_cnt++;
				printf("  elem[W]:%p %p\n",cmd->elem,cmd->elem->word);
				printf("  %s\n",cmd->elem->word);
			}
			/* redirect element */
			else
			{
				redir = cmd->elem->redirect;
				do {
					printf("  elem[R]:%p %p\n",cmd->elem,redir);
					printf("  %s (%d,%d)\n",redir->file,redir->fd,redir->stdout_append);
					size_t redirlen = strlen(redir->file);
					redirects[redir->fd] = (REDIRECT *)rgc_malloc(gc_ctx, sizeof(REDIRECT));
					memcpy(redirects[redir->fd],redir,sizeof(REDIRECT));
					if (redir->fd == 1)
						redir_out_append = redir->stdout_append;
				}
				while ((redir = redir->next) !=NULL);
			}
			/*
			do
			{
				printf("  elem:%p\n",elem);
			} while((elem = elem->next) != NULL);
			*/
		}
	} while((cmd = cmd->next) != NULL);
	cmd_execute(cmd_ary, cmd_cnt, redirects, redir_out_append);

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

