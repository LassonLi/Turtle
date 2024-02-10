CC=gcc
COMMON= -Wall -Wextra -Wfloat-equal -Wvla -Wpedantic -std=c99 -Werror
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3
LDLIBS =

all: testrealloc_s testrealloc_v testrealloc testlinked testlinked_s testlinked_v revstr_realloc revstr_linked pdfs buildpostfix


testlinked: stack.h specific.h linked.c teststack.c general.h general.c
	$(CC) teststack.c linked.c general.c -o testlinked -I./Linked $(PRODUCTION) $(LDLIBS)

testlinked_s: stack.h specific.h linked.c teststack.c general.h general.c
	$(CC) teststack.c linked.c general.c -o testlinked_s -I./Linked $(SANITIZE) $(LDLIBS)

testlinked_v: stack.h specific.h linked.c teststack.c general.h general.c
	$(CC) teststack.c linked.c general.c -o testlinked_v -I./Linked $(VALGRIND) $(LDLIBS)


clean:
	rm -f testrealloc_s testrealloc_v testrealloc testlinked testlinked_s testlinked_v Linkeds.dot Linkeds.pdf Reallocs.dot Reallocs.pdf revstr_realloc revstr_linked postfix_linked postfix_realloc

run: all buildpostfix
	./testlinked_s
	valgrind ./testlinked_v
        

































DOT := $(shell command -v dot 2> /dev/null)

Linkeds.dot:  testlinked
	./testlinked
Reallocs.dot: testrealloc
	./testrealloc

Linkeds.pdf:  teststack.c testlinked Linkeds.dot
ifndef DOT
    $(error "dot is not available please install graphviz")
endif
	dot -Tpdf -o Linkeds.pdf Linkeds.dot
Reallocs.pdf:  teststack.c testrealloc Reallocs.dot
ifndef DOT
    $(error "dot is not available please install graphviz")
endif
	dot -Tpdf -o Reallocs.pdf Reallocs.dot

pdfs : Linkeds.pdf Reallocs.pdf
