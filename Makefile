#
# Makefile of RubbiSH
# Keiya Chinen <s1011420@coins.tsukuba.ac.jp>
#
CFLAGS ?= -O3
CFLAGS += -Wall -g

# debug RubbishGC
#CFLAGS += -DRGCDEBUG

# address sanitizer
CFLAGS += -fsanitize=address

FLAGS  := $(CFLAGS) $(LDFLAGS)
YACC   = bison

rubbish: semantic.o parser.o rubbish.o rubgc.o
	$(CC) $(FLAGS) $^ -lreadline -o $@

rubgc.o: rubgc.c
	$(CC) $(FLAGS) -c $^ -o $@

rubbish.o: rubbish.c
	$(CC) $(FLAGS) -c $^ -o $@

semantic.o: semantic.c
	$(CC) $(FLAGS) -c $^ -o $@

parser.o: parser.y lexer.c
	$(YACC) -o parser.c parser.y
	$(CC) $(FLAGS) -c parser.c -o $@

clean:
	rm parser.c *.o
