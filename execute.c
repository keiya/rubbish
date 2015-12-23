/*
 * execute.c
 * process management (pipeline,redirection,fork-exec)
 * Keiya Chinen <s1011420@coins.tsukuba.ac.jp>
 * */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "rubbish.h"
#include "semantic.h"
#include "execute.h"
#include "rubgc.h"
#include "shellcmd.h"

#define MAX_ARGS_CNT 100
extern char **environ;

/* use fd as std_fd
   create_redir( x , 0 ) //=> x as stdin
   create_redir( y , 1 ) //=> y as stdout
   create_redir( z , 2 ) //=> z as stderr
*/
void create_redir(int fd, int std_fd)
{
	if (fd < 0)
	{
		perror("open");
		close(fd);
		exit(1);
	}
	close(std_fd);
	if (dup2(fd, std_fd)<0)
	{
		perror("dup2");
		close(fd);
		exit(1);
	}
	close(fd);
}

void child_redirect(REDIRECT_IOS *redios)
{
	if (redios->infile != NULL)
	{
		printf("  in(%s)",redios->infile);
		int in;
		in = open(redios->infile,O_RDONLY);
		create_redir(in, 0);
	}

	if (redios->outfile != NULL)
	{
		printf("  out(%s)",redios->outfile);
		int out;
		if (redios->outappend == 1)
			out = open(redios->outfile,
					O_CREAT|O_WRONLY|O_APPEND,
					S_IRUSR|S_IWUSR);
		else
			out = open(redios->outfile,
					O_CREAT|O_WRONLY|O_TRUNC,
					S_IRUSR|S_IWUSR);
		create_redir(out, 1);
	}

	if (redios->errfile != NULL)
	{
		printf("  err(%s)",redios->errfile);
		int out;
		if (redios->errappend == 1)
			out = open(redios->errfile,
					O_CREAT|O_WRONLY|O_APPEND,
					S_IRUSR|S_IWUSR);
		else
			out = open(redios->errfile,
					O_CREAT|O_WRONLY|O_TRUNC,
					S_IRUSR|S_IWUSR);
		create_redir(out, 2);
	}
}

void run(COMMAND* _cmd)
{
	if (_cmd == NULL) return; /* empty */
	CMD_CHAIN *cmdchain_head,*cmdchain;
	cmdchain_head = (CMD_CHAIN *)rgc_malloc(gc_ctx, sizeof(CMD_CHAIN));
	cmdchain = cmdchain_head;

	cmdchain->elem = (ELEM_CHAIN *)rgc_malloc(gc_ctx, sizeof(ELEM_CHAIN));
	ELEM_CHAIN *elemchain_tail;
	elemchain_tail = cmdchain->elem;

	COMMAND *cmd = _cmd;
 	while(cmd != NULL)
	{
		printf("running:%p (optype:%d;opflag:%d)\n",cmd,cmd->type,cmd->flag);

		// pipe?
		if (cmd->flag & PROC_PIPE)
		{
			cmdchain->pipe = 1;
			//forkexec(cmdchain_head);
			cmdchain->next = (CMD_CHAIN *)rgc_malloc(gc_ctx, sizeof(CMD_CHAIN));
			cmdchain->next->prev = cmdchain;
			cmdchain = cmdchain->next;
			cmdchain->elem = (ELEM_CHAIN *)rgc_malloc(gc_ctx, sizeof(ELEM_CHAIN));
			elemchain_tail = cmdchain->elem;
			cmdchain->elem_cnt = 0;
		}

		if (cmd->elem != NULL)
		{
			REDIRECT_IOS *redios;
			redios = (REDIRECT_IOS *)rgc_malloc(gc_ctx, sizeof(REDIRECT_IOS));
			if (cmd->elem->redirect != NULL)
			{
				REDIRECT *redir;
				redir = cmd->elem->redirect;
				do {
					printf("  redirection (next:%p;file:'%s';fd:%d;app:%d)\n",
								redir->next,
								redir->file,
								redir->fd,
								redir->stdout_append
								);
					switch (redir->fd)
					{
						case 0:
							//tail_chain->infile = (char *)rgc_malloc(gc_ctx, strlen(redir->file)+1);
							//strcpy(tail_chain->infile,redir->file);
							//tail_chain->infile = redir->file;
							redios->infile = redir->file;
							break;
						case 1:
							//tail_chain->outfile = (char *)rgc_malloc(gc_ctx, strlen(redir->file)+1);
							//strcpy(tail_chain->outfile,redir->file);
							//tail_chain->outfile = redir->file;
							//tail_chain->outappend = redir->stdout_append;
							redios->outfile = redir->file;
							redios->outappend = redir->stdout_append;
							break;
						case 2:
							//tail_chain->errfile = (char *)rgc_malloc(gc_ctx, strlen(redir->file)+1);
							//strcpy(tail_chain->errfile,redir->file);
							//tail_chain->errfile = redir->file;
							//tail_chain->errappend = redir->stdout_append;
							redios->errfile = redir->file;
							redios->outappend = redir->stdout_append;
							break;
					}
				}
				while ((redir = redir->next) !=NULL);
				cmdchain->redios = redios;
			}
			else
			{
				elemchain_tail->word = cmd->elem->word;
				elemchain_tail->next = (ELEM_CHAIN *)rgc_malloc(gc_ctx, sizeof(ELEM_CHAIN));
				elemchain_tail = elemchain_tail->next;
				cmdchain->elem_cnt++;
			}
		}
		cmd = cmd->next;
	}
	forkexec(cmdchain_head);

	/* Parent */
	int status;
	// parent
	if (waitpid(-1,&status,0) < 0)
	{
		perror("waitpid");
	}

}

/* arg:
 * - chain    : list of a single command and arguments
 * 
 * returns pipefd
 * */
void forkexec(CMD_CHAIN *cmdchain)
{
	while (1)
	{
		printf("cmd @ %p (%p<-prev|next->%p)\n  elem:%p\n  elemcnt:%d\n  pipe:%d\n  pipefd[0]:%d\n  pipefd[1]:%d\n  redios:%p\n",cmdchain,
						cmdchain->prev,cmdchain->next,cmdchain->elem,cmdchain->elem_cnt,cmdchain->pipe,cmdchain->pipefd[0],cmdchain->pipefd[1],cmdchain->redios);
		ELEM_CHAIN *elemchain = cmdchain->elem;

		char **args;
		args = (char **)rgc_malloc(gc_ctx, sizeof(char *)*(cmdchain->elem_cnt+1));
		int i;
		for (i=0; i<cmdchain->elem_cnt;i++)
		{
			printf("  elem(%d) @ %p (next->%p)\n    word:%s\n",i,cmdchain,
							elemchain->next,elemchain->word);

			//args[i] = (char *)rgc_malloc(gc_ctx,strlen(elemchain->word)+1);
			//strcpy(args[i],elemchain->word);
			args[i] = elemchain->word;
			if (elemchain->next != NULL)
				elemchain = elemchain->next;
			else break;
		}

		// try to run shell command (exit,cd, etc...)
		if (try_shellcmd(args[0]))
		{
			return;
		}

		if (cmdchain->pipe) // command stdout => pipeline
		{
			int pipefd[2];
			if (pipe(pipefd) < 0)
			{
				perror("pipe");
				exit(1);
			}
			cmdchain->pipefd[0] = pipefd[0];
			cmdchain->pipefd[1] = pipefd[1];
		}

		int cpid;
		if ((cpid = fork()) < 0)
		{
			perror("fork");
		}
	
		if (cpid == 0) /* child */
		{
			/* pipe out (stdout) */
			if (cmdchain->pipe)
			{
			//printf("[CHILD][OUT] close=%d dup2(%d,%d)\n"
			//				,cmdchain->pipefd[0]
			//				,cmdchain->pipefd[1],1);
				close(cmdchain->pipefd[0]); /* close parent-side fd */
				if (dup2(cmdchain->pipefd[1], 1)<0) /* pipefd[1] -> stdout */
				{
					perror("dup2");
					exit(1);
				}
			}
	
			/* pipe in (stdin) */
			if (cmdchain->prev!=NULL&&cmdchain->prev->pipe)
			{
			//printf("[CHILD][IN] close=%d dup2(%d,%d)\n"
			//				,cmdchain->prev->pipefd[1]
			//				,cmdchain->prev->pipefd[0],0);
				if (dup2(cmdchain->prev->pipefd[0], 0)<0) /*  -> stdin */
				{
					perror("dup2");
					exit(1);
				}
			}
	
			if (cmdchain->redios != NULL)
			child_redirect(cmdchain->redios);
	
			//printf("\n");
	
			//printf("EXECVP:%s %p %p\n",args[0],args,args[0]);
			if (execvp(args[0], args) == -1)
			{
				perror("exec");
				exit(-1);
			}
		}
		else /* Parent */
		{
		printf("[PARENT] fork %d\n",cpid);
			if (cmdchain->pipe)
			{
				printf("[PARENT](current)close %d\n",cmdchain->pipefd[1]);
				close(cmdchain->pipefd[1]); /* close child-side fd */
			}
			if (cmdchain->prev != NULL&&cmdchain->prev->pipe)
			{
				printf("[PARENT](prev)close %d\n",cmdchain->prev->pipefd[0]);
				close(cmdchain->prev->pipefd[0]);
			}
		}



		if (cmdchain->next != NULL)
			cmdchain = cmdchain->next;
		else break;
	}
	return;
}
