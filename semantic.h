enum optype
{
	TYPE_SHELL         = 0,
	TYPE_SIMPLE        = 1,
	TYPE_FILE          = 2,
	TYPE_PARAM         = 3
};
enum opflag
{
	/* type: file */
	/*
	FILE_STIN          = 1 << 0,
	FILE_STOUT_CREAT   = 1 << 1,
	FILE_STOUT_APPEND  = 2 << 2,
	*/

	PROC_BACKGROUND    = 1 << 3,
	PROC_PIPE          = 1 << 4,

	COND_AND           = 1 << 5,
	COND_OR            = 1 << 6
};
enum redirect_fd
{
	REDIR_STDIN  = 0,
	REDIR_STDOUT = 1,
	REDIR_STDERR = 2
};

typedef struct _command {
	struct _command *next;  /* list structure (pipe connection,command parameters,etc..) */
	struct _element *elem;  /* command which excuted */
	struct _redirect *rein;
	struct _redirect *reout;
	struct _redirect *reerr;
	enum optype type;
	enum opflag flag;
} COMMAND;

typedef struct _element {
	char *word;
	struct _redirect *redirect;
} ELEMENT;

typedef struct _redirect {
	struct _redirect *next;
	char *file;
	enum redirect_fd fd;
	int stdout_append;
} REDIRECT;

ELEMENT* word_element(char *);
COMMAND* element_command(ELEMENT *, COMMAND *);

COMMAND* parsed_semantic;

extern void* gc_ctx;

REDIRECT* redirection(ELEMENT *, int, int);
ELEMENT* redirect_element(REDIRECT *);
