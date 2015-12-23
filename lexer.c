/*
 * Keiya Chinen
 * s1011420@coins.tsukuba.ac.jp
 * */
#include "rubbish.h"
#include "rubgc.h"
int yylex()
{
	int input_sz = strlen(input) + 1;

	//char buf[BUFSZ];
	char *buf;
	buf = (char *)rgc_malloc(gc_ctx, input_sz);
	buf[0] = '\0';

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
				//while(isalpha(input[input_cur]))
				while(input[input_cur])
				{
					switch (input[input_cur])
					{
						/* keyword(command,filename) does not contain these tokens: */
						case ' ':
						case '&':
						case '*':
						case '(':
						case ')':
						case '\\':
						case ';':
						case '\'':
						case '"':
						case '<':
						case '>':
						case '?':
						case '[':
						case ']':
						case '|':
							goto END_KEYWORD;
					}
					if (j < input_sz - 1) {
						buf[j] = input[input_cur];
						j++;
					}
					input_cur++;
				}
END_KEYWORD:
				buf[j] = '\0';
				yylval.element = word_element(buf);
				retval = WORD; goto BREAK;
		}
		input_cur++;
	}
BREAK:
	/* end of token, return to bison */
	return retval;
}

int yyerror()
{
	fprintf(stderr,"Syntax error\n");
}
