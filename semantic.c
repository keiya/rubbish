#include "stdlib.h"
#include "semantic.h"

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

