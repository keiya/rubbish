int try_shellcmd(char *cmd)
{
	if (strcmp(cmd,"exit") == 0)
	{
		exit(0);
	}
	return 0;
}
