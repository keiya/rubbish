%{
#include <stdio.h>
#include "semantic.h"
%}

%union {
COMMAND *command;
}

%token <word> WORD
%token CND_AND CND_OR STDOUT_APPEND PIPE_STDERR PIPE_STDOUT
%left CND_AND CND_OR
%right '|' PIPE_STDERR

%type <command> command

%start command

%%
command:
		operation operation_terminator
		{
			printf("[Y]ope+term\n");
			YYACCEPT;
		}
	|	'\n'
		{
			printf("[Y]CMDTERM\n");
			YYACCEPT;
		}
	;

operation:
		WORD
		{ printf("[Y]WORD\n"); }
	;

operation_terminator:
		'\n'
		{ printf("[Y]TERM\n"); }
	;

%%

#include "lexer.c"
