#
# Makefile of RubbiSH
# Keiya Chinen <s1011420@coins.tsukuba.ac.jp>
#
CFLAGS ?= -O3
CFLAGS += -Wall -g

# debug RubbishGC
#CFLAGS += -DRGCDEBUG

# debug runner
#CFLAGS += -DDEBUG

# address sanitizer
#CFLAGS += -fsanitize=address

FLAGS  := $(CFLAGS) $(LDFLAGS)
YACC   = bison


rubbish: cwd.o execute.o parser.o rubbish.o rubgc.o semantic.o shellcmd.o
	$(CC) $(FLAGS) $^ -lreadline -o $@

cwd.o: cwd.c
	$(CC) $(FLAGS) -c $^ -o $@

execute.o: execute.c
	$(CC) $(FLAGS) -c $^ -o $@

rubbish.o: rubbish.c
	$(CC) $(FLAGS) -c $^ -o $@

rubgc.o: rubgc.c
	$(CC) $(FLAGS) -c $^ -o $@

parser.o: parser.y lexer.c
	$(YACC) -o parser.c parser.y
	$(CC) $(FLAGS) -c parser.c -o $@

semantic.o: semantic.c
	$(CC) $(FLAGS) -c $^ -o $@

shellcmd.o: shellcmd.c
	$(CC) $(FLAGS) -c $^ -o $@

clean:
	rm parser.c *.o
