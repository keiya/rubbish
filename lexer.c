/*
 * Keiya Chinen
 * s1011420@coins.tsukuba.ac.jp
 * */
#include "rubbish.h"
#include "rubgc.h"
int yylex()
{
	printf("yylex(%d): %s\n",input_cur,input);
	int input_sz = strlen(input) + 1;

	//char buf[BUFSZ];
	char *buf;
	buf = (char *)rgc_malloc(gc_ctx, input_sz);

	// < > | |& >> () & && ||
	int j = 0;
	int retval = yacc_EOF;
	while (input[input_cur] != '\0'){
		switch (input[input_cur])
		{
			case '#':
				do
				{
					input_cur++;
				}
				while(input[input_cur] != '\0');
				retval = '\n'; goto BREAK;
			case '>': /* > >> */
				printf("[L]got >\n");
				if (input_cur+1 < input_sz && input[input_cur+1])
				{
					switch (input[input_cur+1])
					{
						case '>':
							input_cur += 2;
							retval = STDOUT_APPEND; goto BREAK;
						default:
							input_cur++;
							retval = '>'; goto BREAK;
					}
				}
			case '<':
				printf("[L]got <\n");
				input_cur++;
				retval = '<'; goto BREAK;
			case '|': /* | || |& */
				if (input_cur+1 < input_sz && input[input_cur+1])
				{
					switch (input[input_cur+1])
					{
						case '|':
							input_cur += 2;
							retval = CND_OR; goto BREAK;
						case '&':
							input_cur += 2;
							retval = PIPE_STDERR; goto BREAK;
						default:
							input_cur++;
							retval = '|'; goto BREAK;
					}
				}
			case '(':
				input_cur++;
				retval = '('; goto BREAK;
			case ')':
				input_cur++;
				retval = ')'; goto BREAK;
			case '&': /* & && */
				if (input_cur+1 < input_sz && input[input_cur+1])
				{
					switch (input[input_cur+1])
					{
						case '&':
							input_cur += 2;
							retval = CND_AND; goto BREAK;
						default:
							input_cur++;
							retval = '&'; goto BREAK;
					}
				}
			case ' ':
				input_cur++;
				continue;
			default:
				j=0;
				while(isalpha(input[input_cur]))
				{
					if (j < input_sz - 1) {
						buf[j] = input[input_cur];
						j++;
					}
					input_cur++;
				}
				buf[j] = '\0';
				printf("lex:buf='%s' %p\n",buf,buf);
				yylval.element = word_element(buf);
				retval = WORD; goto BREAK;
		}
		input_cur++;
	}
BREAK:
	/* end of token, return to bison */
	printf("[L]token end\n");
	return retval;
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
