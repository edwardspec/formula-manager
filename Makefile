.PHONY: clean debug

TARGETS = $(patsubst %.c,%,$(wildcard test-*.c))
TARGETS += $(patsubst main-%.c,app-%,$(wildcard main-*.c))
TARGETS += $(patsubst %.c,%,$(wildcard labs/lab*.c))

LIB = libformula.so
SYMTABLE = bitmask

DEFINES = -DCASE_SENSITIVE # -DDEBUG

CPPFLAGS = -I`pwd` -D"__attribute__(x)"= -D_ISOC99_SOURCE -D_BSD_SOURCE

CC = gcc
CFLAGS = $(CPPFLAGS) -W -Wall -g -O0
LDFLAGS = -L`pwd` -lformula -lm

all: $(TARGETS)

$(LIB): formula.o lex.o symtable.o mpool.o integral.o rungekutta.o min1var.o minNvars.o
	$(CC) -shared $^ -o $@ -lm

test-eval: test-eval.o $(LIB)
test-symtable-bitmask: test-symtable-bitmask.o $(LIB)
test-rungekutta: test-rungekutta.o $(LIB)

app-integral: main-integral.o $(LIB)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(DEFINES) $(CFLAGS) -fPIC -c $< -o $@

lex.c: lex_rules.l
	lex -t $< >lex.c

lex.o: lex.c y.tab.c

y.tab.c: yacc.y
	yacc yacc.y

symtable.o: symtable-$(SYMTABLE).o
	ln -sf `pwd`/$< $@

clean:
	rm -f *.o $(TARGETS) lex.h lex.c y.tab.c y.tab.h *.so

debug: all
	# echo "A+A*A" | LD_LIBRARY_PATH=. gdb -ex "run -a 0 -b 1 -N 900" --quiet --batch ./app-integral
	echo "A+A*A" | LD_LIBRARY_PATH=. ./app-integral -a 0 -b 1 -N 900
