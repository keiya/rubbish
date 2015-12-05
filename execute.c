#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "execute.h"
extern char **environ;
int cmd_execute(char **cmd_ary, int cmd_cnt, REDIRECT **redirects, int redir_out_append)
{
	int cpid;
	if ((cpid = fork()) < 0)
	{
		perror("fork");
	}
	if (cpid == 0) /* child */
	{
		if (redirects[0])
		{
			int in;
			in = open(redirects[0]->file,O_RDONLY);
			create_pipe(in, 0);
		}
		if (redirects[1])
		{
			int out;
			if (redirects[1]->stdout_append == 1)
				out = open(redirects[1]->file,
						O_CREAT|O_WRONLY|O_APPEND,
						S_IRUSR|S_IWUSR);
			else
				out = open(redirects[1]->file,
						O_CREAT|O_WRONLY|O_TRUNC,
						S_IRUSR|S_IWUSR);
			create_pipe(out, 1);
		}
		if (redirects[2])
		{
			int out;
			if (redirects[2]->stdout_append == 1)
				out = open(redirects[2]->file,
						O_CREAT|O_WRONLY|O_APPEND,
						S_IRUSR|S_IWUSR);
			else
				out = open(redirects[2]->file,
						O_CREAT|O_WRONLY|O_TRUNC,
						S_IRUSR|S_IWUSR);
			create_pipe(out, 2);
		}
		execvp(cmd_ary[0], cmd_ary);
	}
	else /* Parent */
	{
		int status;
		// parent
		if (wait(&status) < 0)
		{
			perror("wait");
		}
		printf("The child (pid=%d) exited with status(%d).\n",cpid, WEXITSTATUS(status));
	}
	printf("\n");
	return 0;
}

/* use fd as std_fd
   create_pipe( x , 0 ) //=> x as stdin
   create_pipe( y , 1 ) //=> y as stdout
   create_pipe( z , 2 ) //=> z as stderr
*/
void create_pipe(int fd, int std_fd)
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

