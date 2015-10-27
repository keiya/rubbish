#include "rubbish.h"
#define BUFSZ 8
#define MAX_HISTORY_NO 3
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
int yylex()
{
	printf("yylex(%d): %s\n",input_cur,input);

	char buf[BUFSZ];

	int sz = strlen(input);
	// < > | |& >> () & && ||
	int j = 0;
	while (input[input_cur] != '\0'){
		switch (input[input_cur])
		{
			case '>': /* > >> */
				if (input_cur+1 < sz && input[input_cur+1])
				{
					switch (input[input_cur+1])
					{
						case '>':
							return STDOUT_APPEND;
						default:
							return '>';
					}
				}
			case '<':
				return '<';
			case '|': /* | || |& */
				if (input_cur+1 < sz && input[input_cur+1])
				{
					switch (input[input_cur+1])
					{
						case '|':
							return CND_OR;
						case '&':
							return PIPE_STDERR;
						default:
							return PIPE_STDOUT;
					}
				}
			case '(':
				return '(';
			case ')':
				return ')';
			case '&': /* & && */
				if (input_cur+1 < sz && input[input_cur+1])
				{
					switch (input[input_cur+1])
					{
						case '&':
							return CND_AND;
						default:
							return '&';
					}
				}
			default:
				j=0;
				while(isalpha(input[input_cur]))
				{
					if (input_cur >= BUFSZ - 1) break;
					buf[j] = input[input_cur];
					j++;
					input_cur++;
				}
				buf[j] = '\0';
				printf("lex:buf='%s'\n",buf);
				return WORD;
		}
		input_cur++;
	}
	printf("input end\n");
	return '\n';
	/*
	char *buf;
	buf = malloc( BUFSZ );
	if( buf == NULL )
	{
		perror("malloc()");
		exit( -1 );
	}

	char prompt[128] = "";
	int history_no = 0;
	HIST_ENTRY *history = NULL;
	char *line;
	char *unescaped_line;
	unescaped_line = malloc(128);
	unescaped_line[0] = '\0';
	int continuous_line = 0;
		int i;
		int j=0;
		int c;
		// char loop
		for (i=0; line[i] != '\0'; ++i)
		{
			c = line[i];
			//if (isspace(c))
			//	continue;
			char *c2 = line;
			int k=0;
			do {
				printf("  [a]%s\n",c2[k]);
			} while(isalpha(c2[k]));
			if (j < BUFSZ - 1)
				buf[j++] = line[i];
			printf("%d(%d):%c\n",i,j,c);
		}
		printf("#%s\n",line);
		add_history(line);
		if (++history_no > MAX_HISTORY_NO)
		{
			history = remove_history(0);
			free(history);
		}
		printf(">%s\n",buf);
		yylval.command = command_str(buf);
		return WORD;
	}
	*/
}

int yyerror()
{
	fprintf(stderr,"Syntax error\n");
}
