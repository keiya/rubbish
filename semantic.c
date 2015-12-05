/*
 * semantic.c , part of RubbiSH
 * Convert parsed data into RubbiSH Semantic ASL (Abstract Syntax List)
 * Keiya Chinen <s1011420@coins.tsukuba.ac.jp>
 * */
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include <stdio.h>
#include "rubgc.h"

/*
COMMAND *command_str(char *str)
{
	COMMAND *cmd;
	cmd = malloc( sizeof(COMMAND) );
	char *buf;
	buf = malloc( strlen(str) );
	if( buf == NULL )
	{
		perror("malloc()");
		exit( -1 );
	}
	cmd->str = buf;
	return cmd;
}
*/
ELEMENT* word_element(char *str)
{
	ELEMENT *e;
	e = (ELEMENT *)rgc_malloc(gc_ctx, sizeof(ELEMENT));
	e->word = (char *)rgc_malloc(gc_ctx, strlen(str)+1);
	strcpy(e->word,str);
	printf("WE:%p <<<<<<<\n",e);
	return e;
}
COMMAND* element_command(ELEMENT *elem, COMMAND *srccmd)
{
	if (srccmd == NULL)
	{
		COMMAND *c;
		c = (COMMAND *)rgc_malloc(gc_ctx, sizeof(COMMAND));
		c->elem = elem;
		printf("[S]element_command(new)->%p (got %p)\n",c,elem);
		return c;
	}

	COMMAND *next,*last;
	last = srccmd;
	next = srccmd->next;
	while (next != NULL)
	{
		last = next;
		next = next->next;
	}

	next = (COMMAND *)rgc_malloc(gc_ctx, sizeof(COMMAND));
	next->elem = elem;
	printf("[S]element_command(concat)->%p\n",next);
	last->next = next;
	return srccmd;
}

REDIRECT* redirection(ELEMENT *e, int fd, int append)
{
	printf("REDIR:%p\n",e);
	REDIRECT *r;
	r = (REDIRECT *)rgc_malloc(gc_ctx, sizeof(REDIRECT));
	printf("[S]redirection->%p\n",r);
	r->file = (char *)rgc_malloc(gc_ctx, strlen(e->word)+1);
	strcpy(r->file,e->word);
	r->fd = fd;
	r->stdout_append = append;
	return r;
}

ELEMENT* redirect_element(REDIRECT *r)
{
	
	ELEMENT *e;
	e = (ELEMENT *)rgc_malloc(gc_ctx, sizeof(ELEMENT));
	e->redirect = r;
	return e;
}
