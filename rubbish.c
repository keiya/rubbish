#include <stdlib.h>
#include "semantic.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "rubbish.h"
#define MAX_HISTORY_CNT 3

char *input; // extern
int input_cur;
int main()
{
	char prompt[128] = "";
	int history_no = 0;
	HIST_ENTRY *history = NULL;
	while (input = readline(prompt))
	{
		add_history(input);
		printf("[R]%s\n",input);
		input_cur = 0;
		yyparse();
		if (++history_no > MAX_HISTORY_CNT)
		{
			history = remove_history(0);
			free(history);
		}

	}
	return 0;
}

