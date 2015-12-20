void run(COMMAND*);
int cmd_execute(char**, int, REDIRECT**, int);
void create_redir(int, int);

typedef struct _cmd_chain {
	struct _cmd_chain *next;
	struct _cmd_chain *prev;
	struct _elem_chain *elem;
	int elem_cnt;
	int pipe; /* use pipe out? */
	int pipefd[2]; /* file descriptor of my pipe */
	int ran;
//	struct _redirect *red;
	struct _redirect_ios *redios;
} CMD_CHAIN;

typedef struct _elem_chain {
	struct _elem_chain *next;
	char *word;
} ELEM_CHAIN;

typedef struct _redirect_ios {
	char *infile;
	char *outfile;
	char *errfile;
	int outappend;
	int errappend;
} REDIRECT_IOS;

typedef struct _pipes {
	int pfd;
	int cfd;
} PIPES;


void forkexec(CMD_CHAIN*);
