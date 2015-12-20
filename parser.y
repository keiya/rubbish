%{
#include <stdio.h>
#include "semantic.h"
%}

%union {
ELEMENT *element;
COMMAND *command;
REDIRECT *redirect;
}

%token CND_AND CND_OR STDOUT_APPEND PIPE_STDERR

%type <command> start operations operation pipeline command simple_command
%type <element> simple_command_element
%type <redirect> redirection redirection_list
%token <element> WORD

%start start

%left '&' ';' '\n' yacc_EOF
%left CND_AND CND_OR
%right '|' PIPE_STDERR
%%
start:
		operations operation_terminator
		{
			parsed_semantic = $1;
			printf("[Y]ope+term %p\n",$1);
			YYACCEPT;
		}
	|	'\n'
		{
			printf("[Y]accepted by \\n\n");
			YYACCEPT;
		}
	|	yacc_EOF
		{
			printf("[Y]accepted by EOF\n");
			YYACCEPT;
		}
	;

newlines:
		/* empty */
	|	'\n'
	;

operations:
		operation
	|	operation '&'
	|	operation ';'
	;

operation:
		operation CND_AND newlines operation
	|	operation CND_OR newlines operation
	|	operation '&' operation
	|	operation ';' operation
	|	pipeline
			{
				printf("[Y]operation->pipeline\n");
			}
	;

redirection:
		'>' WORD
			{
				$$ = redirection($2,REDIR_STDOUT,0);
			}
	|	'<' WORD
			{
				$$ = redirection($2,REDIR_STDIN,0);
			}
	|	STDOUT_APPEND WORD
			{
				$$ = redirection($2,REDIR_STDOUT,1);
			}

redirection_list:
		redirection
		{
			//$$ = $1
		}
	|	redirection_list redirection
		{
			// TODO: add $2 to $1(list)
		}
	;

simple_command_element:
		WORD
		{
			$$ = $1;
		}
	|	redirection
		{
			$$ = redirect_element($1);
		}
	;

simple_command:
		simple_command_element
			{
				$$ = element_command($1,NULL);
			}
	|	simple_command simple_command_element
			{
				$$ = element_command($2,$1);
			}
	;

command:
		simple_command
	;

pipeline:
		pipeline '|' newlines pipeline
		{ $$ = create_pipeline($1,$4); }
	|	pipeline PIPE_STDERR newlines pipeline
	|	command
	;

operation_terminator:
		'\n'
	|	yacc_EOF
	;

%%

#include "lexer.c"
