/*
 * RubGC (RubbishGC)
 * memory management program
 * Keiya Chinen <s1011420@coins.tsukuba.ac.jp>
 * */
#include <stdlib.h>
#include <stdio.h>
#include "rubgc.h"

struct allocated
{
	void *ptr;
	struct allocated *next;
	struct allocated *prev;
};

struct context
{
	struct allocated *list;
	struct allocated *list_cur;
};


void* rgc_init()
{
	struct context* ctx;
	ctx = malloc(sizeof(struct context));
	if (ctx == NULL)
	{
		perror("gc_malloc");
		return NULL;
	}

	ctx->list = malloc(sizeof(struct allocated));
	if (ctx->list == NULL)
	{
		perror("gc_malloc");
		return NULL;
	}
	ctx->list->ptr = NULL;
	/* bug? */
	ctx->list->next = NULL;
	ctx->list->prev = NULL;
	ctx->list_cur = ctx->list;


	return ctx;
}

void rgc_free_all(void* p)
{
	struct allocated* ctx = ((struct context *)p)->list;
	if (ctx == NULL) return;
	while(1)
	{
		if (ctx->ptr != NULL)
		{
#ifdef RGCDEBUG
			printf("[RGC]free(ptr):%p\n",ctx->ptr);
#endif
			free(ctx->ptr);
			ctx->ptr = NULL;
		}
		if (ctx->prev != NULL)
		{
#ifdef RGCDEBUG
			printf("[RGC]free(prev):%p\n",ctx->prev);
#endif
			free(ctx->prev);
			ctx->prev = NULL;
		}
		if (ctx->next != NULL)
		{
			ctx = ctx->next;
		}
		else
		{
#ifdef RGCDEBUG
			printf("[RGC]free(ctx):%p\n",ctx);
#endif
			free(ctx);
			ctx = NULL;
			break;
		}
	}
#ifdef RGCDEBUG
	printf("[RGC]free(p):%p\n",p);
#endif
	free(p);
	p = NULL;
}

void* rgc_malloc(void* p, size_t sz)
{
	struct allocated* list_ptr = ((struct context *)p)->list_cur;

	list_ptr->ptr = malloc(sz);
	if (list_ptr->ptr == NULL)
	{
		perror("gc_malloc");
		return NULL;
	}
#ifdef RGCDEBUG
	printf("[RGC]malloc(ptr):%p (%d bytes)\n",list_ptr->ptr,sz);
#endif

	list_ptr->next = malloc(sizeof(struct allocated));
	if (list_ptr->next == NULL)
	{
		perror("gc_malloc");
		return NULL;
	}

	list_ptr->next->prev = list_ptr;
	//list_ptr = list_ptr->next;
	((struct context *)p)->list_cur = list_ptr->next;
	return list_ptr->ptr;
}


#ifdef MAINTEST
#define RGCDEBUG 1
#include <stdio.h>
int main()
{
	void *ctx = rgc_init();
	char *str1,*str2,*str3;
	str1 = (char *)rgc_malloc(ctx, 8);
	str2 = (char *)rgc_malloc(ctx, 2);
	str3 = (char *)rgc_malloc(ctx, 3);
	str1[0] = '\0';
	str2[1] = '\0';
	str3[2] = '\0';
	printf("%p %p %p\n",str1,str2,str3);
	printf("%s%s%s\n",str1,str2,str3);
	rgc_free_all(ctx);

	/*
	int i=0;
	for (i=0;i<100;i++) {
		char *str = (char *)gc_malloc(ctx, 256);
		//printf("%p\n",((struct allocated*)ctx)->next);
		int j;
		for (j=0;j<254;j++)
			str[j] = 'A';
		str[255] = '\0';
		//printf("%d %s\n",i,((struct allocated*)ctx)->ptr);
	}
	gc_free_all(ctx);
	*/

	return 0;
}
#endif
