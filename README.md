RubbiSH
=========

## implementation design
* cwd.c
  * handle current directory (cd)
* execute.c
  * process management
* lexer.c
  * Lexical Analyzer
* parser.y
  * Yacc(Bison)
* rubbish.c
  * read inputs by readline()
  * manage input historys
* rubgc.c
  * memory management
* semantic.c
  * functions that generate abstract syntax
* shellcmd.c
  * internal shell commands
