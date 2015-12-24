#include "cwd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
int try_shellcmd(char **args)
{
	if (strcmp(args[0],"exit") == 0)
	{
		exit(0);
	}
	else if (strcmp(args[0],"cd") == 0)
	{
		if ( ! cwd_chdir(args[1]))
		{
			perror("cd");
		}
		return 1;
	}
	return 0;
}
