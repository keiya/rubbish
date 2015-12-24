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

ELEMENT* word_element(char *str)
{
	ELEMENT *e;
	e = (ELEMENT *)rgc_malloc(gc_ctx, sizeof(ELEMENT));
	e->word = (char *)rgc_malloc(gc_ctx, strlen(str)+1);
	strcpy(e->word,str);
	return e;
}

COMMAND* element_command(ELEMENT *elem, COMMAND *srccmd)
{
	if (srccmd == NULL)
	{
		COMMAND *c;
		c = (COMMAND *)rgc_malloc(gc_ctx, sizeof(COMMAND));
		c->elem = elem;
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
	last->next = next;
	return srccmd;
}

REDIRECT* redirection(ELEMENT *e, int fd, int append)
{
	REDIRECT *r;
	r = (REDIRECT *)rgc_malloc(gc_ctx, sizeof(REDIRECT));
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

COMMAND* create_pipeline(COMMAND *from, COMMAND *to)
{
	COMMAND *tail;
	tail = from;
	while (tail->next != NULL)
	{
		tail = tail->next;
	}

	COMMAND *c;
	c = (COMMAND *)rgc_malloc(gc_ctx, sizeof(COMMAND));
	c->next = to;
	c->flag = PROC_PIPE;

	tail->next = c;
	
	return from;
}
