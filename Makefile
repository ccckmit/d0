CC = gcc
CFLAGS = -g -I ./src
OBJS = obj_test lex_test parse_test

all: $(OBJS)

obj_test: test/obj_test.c
	$(CC) $(CFLAGS) $^ -o $@

lex_test: test/lex_test.c
	$(CC) $(CFLAGS) $^ -o $@

parse_test: test/parse_test.c
	$(CC) $(CFLAGS) $^ -o $@

d0: src/d0.c
	$(CC) $(CFLAGS) $^ -o $@

run: $(OBJS)
	./obj_test
	./lex_test prog/hello.d
	./lex_test prog/stmts.d
	./parse_test prog/stmts.d

clean:
	rm -f $(OBJS)
