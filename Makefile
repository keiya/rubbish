CFLAGS ?= -O3
CFLAGS += -Wall -g
FLAGS  := $(CFLAGS) $(LDFLAGS)
YACC   = bison

rubbish: semantic.o parser.o rubbish.o
	$(CC) $(FLAGS) $^ -lreadline -o $@

rubbish.o: rubbish.c
	$(CC) $(FLAGS) -c $^ -o $@

semantic.o: semantic.c
	$(CC) $(FLAGS) -c $^ -o $@

parser.o: parser.y lexer.c
	$(YACC) -o parser.c parser.y
	$(CC) $(FLAGS) -c parser.c -o $@

clean:
	rm parser.c
