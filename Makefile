CC=gcc
COMMON= -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -std=c99 -Werror
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3

all: parse_s interp_s

parse_s: Parse/parse.h Parse/parse.c
	$(CC) Parse/parse.c -o parse_s -I./Parse $(SANITIZE) -lm

###interp_s: Interp/interp.h Interp/interp.c neillsimplescreen.c neillsimplescreen.h
###	$(CC) neillsimplescreen.c Interp/interp.c -o interp_s -I./Interp $(SANITIZE) -lm

## lasson version, it needs to use Stack
interp_s: Interp/interp.h Interp/interp.c neillsimplescreen.c neillsimplescreen.h Stack/linked.c Stack/stack.h Stack/specific.h Stack/general.h Stack/general.c
	$(CC) neillsimplescreen.c Interp/interp.c Stack/linked.c Stack/general.h Stack/general.c -o interp_s -I./Interp $(SANITIZE) -lm

## lasson do it, it can use to compare whether two files are the same 
compare_s: Testing/compare.c
	$(CC) Testing/compare.c -o compare_s -I./Testing $(SANITIZE) -lm

### An example : you may wish to adapt this slightly
###extension_s: Extension/extension.h Extension/extension.c neillsimplescreen.c neillsimplescreen.h
###	$(CC) neillsimplescreen.c Extension/extension.c -o extension_s -I./Extension $(SANITIZE) -lm


# Our 'core' set of flags used for everything
BASEFLAGS=-Wall -Wextra -Wpedantic -std=c99 -Wvla -Wfloat-equal

# Optimise code, but allow no warnings during compilation
# pkg-config is accesible after you install SDL2 - on my machine it's equaivalent to the flags : -I/usr/include/SDL2 -D_REENTRANT
PRODFLAGS=$(BASEFLAGS) -O1 -Werror `pkg-config sdl2 --cflags`
LDLIBS= -lm

# Library linking : maths and SDL2
# pkg-config is accesible after you install SDL2 - on my machine it's equaivalent to the flags : -lSDL2
LIBS =  -lm `pkg-config sdl2 --libs`

## SDL version, because it need to use SDL, so it need to use pkg-config sdl2 --libs
extension_s: Extension/extension.h Extension/extension.c Extension/neillsdl2.h Extension/neillsdl2.c Stack/linked.c Stack/stack.h Stack/specific.h Stack/general.h Stack/general.c
	$(CC) Extension/neillsdl2.c Extension/extension.c  Stack/linked.c Stack/general.h Stack/general.c -o extension_s -I./Extension $(PRODFLAGS) $(LIBS)


## lasson do it
run_compare_s: run_interp_s compare_s
	./compare_s Results/out_empty.txt ./out_empty.txt
	./compare_s Results/out_forward.txt ./out_forward.txt
	./compare_s Results/out_set1.txt ./out_set1.txt
	./compare_s Results/out_donothing.txt ./out_donothing.txt
	./compare_s Results/out_set2.txt ./out_set2.txt
	./compare_s Results/out_turn.txt ./out_turn.txt
	./compare_s Results/out_spiral.txt ./out_spiral.txt
	./compare_s Results/out_octagon1.txt ./out_octagon1.txt
	./compare_s Results/out_octagon2.txt ./out_octagon2.txt
	./compare_s Results/out_tunnel.txt ./out_tunnel.txt
	./compare_s Results/out_hypno.txt ./out_hypno.txt
	./compare_s Results/out_5x5.txt ./out_5x5.txt
	./compare_s Results/out_fail_parse_ok_interp.txt ./out_fail_parse_ok_interp.txt


## lasson do it
run_parse_s: parse_s
	./parse_s TTLs/empty.ttl
	./parse_s TTLs/forward.ttl
	./parse_s TTLs/ok_parse_fail_interp.ttl
	./parse_s TTLs/set1.ttl
	./parse_s TTLs/donothing.ttl
	./parse_s TTLs/set2.ttl
	./parse_s TTLs/turn.ttl
	./parse_s TTLs/spiral.ttl
	./parse_s TTLs/octagon1.ttl
	./parse_s TTLs/octagon2.ttl
	./parse_s TTLs/tunnel.ttl
	./parse_s TTLs/labyrinth.ttl
	./parse_s TTLs/hypno.ttl
	./parse_s TTLs/5x5.ttl
	./parse_s TTLs/downarrow.ttl

## lasson do it
run_interp_s: interp_s
	./interp_s TTLs/empty.ttl ./out_empty.txt
	./interp_s TTLs/forward.ttl ./out_forward.txt
	./interp_s TTLs/set1.ttl ./out_set1.txt
	./interp_s TTLs/donothing.ttl ./out_donothing.txt
	./interp_s TTLs/set2.ttl ./out_set2.txt
	./interp_s TTLs/turn.ttl ./out_turn.txt
	./interp_s TTLs/spiral.ttl ./out_spiral.txt
	./interp_s TTLs/octagon1.ttl ./out_octagon1.txt
	./interp_s TTLs/octagon2.ttl ./out_octagon2.txt
	./interp_s TTLs/tunnel.ttl ./out_tunnel.txt
	./interp_s TTLs/labyrinth.ttl ./out_labyrinth.txt
	./interp_s TTLs/hypno.ttl ./out_hypno.txt
	./interp_s TTLs/5x5.ttl ./out_5x5.txt
	./interp_s TTLs/fail_parse_ok_interp.ttl ./out_fail_parse_ok_interp.txt


run: all
	./parse_s TTLs/empty.ttl
	./parse_s TTLs/forward.ttl
	./parse_s TTLs/ok_parse_fail_interp.ttl
	./parse_s TTLs/set1.ttl
	./parse_s TTLs/donothing.ttl
	./parse_s TTLs/set2.ttl
	./parse_s TTLs/turn.ttl
	./parse_s TTLs/spiral.ttl
	./parse_s TTLs/octagon1.ttl
	./parse_s TTLs/octagon2.ttl
	./parse_s TTLs/tunnel.ttl
	./parse_s TTLs/labyrinth.ttl
	./parse_s TTLs/hypno.ttl
	./parse_s TTLs/5x5.ttl
	./parse_s TTLs/downarrow.ttl
	./interp_s TTLs/empty.ttl out_empty.txt
	./interp_s TTLs/forward.ttl out_forward.txt
	./interp_s TTLs/set1.ttl out_set1.txt
	./interp_s TTLs/donothing.ttl out_donothing.txt
	./interp_s TTLs/fail_parse_ok_interp.ttl out_fail_parse_ok_interp.txt
	./interp_s TTLs/set2.ttl out_set2.txt
	./interp_s TTLs/turn.ttl out_turn.txt
	./interp_s TTLs/spiral.ttl out_spiral.txt
	./interp_s TTLs/octagon1.ttl out_octagon1.txt
	./interp_s TTLs/octagon2.ttl out_octagon2.txt
	./interp_s TTLs/tunnel.ttl out_tunnel.txt
	./interp_s TTLs/labyrinth.ttl out_labyrinth.txt
	./interp_s TTLs/hypno.ttl out_hypno.txt
	./interp_s TTLs/5x5.ttl out_5x5.txt
	./interp_s TTLs/downarrow.ttl out_downarrow.txt
	./interp_s TTLs/empty.ttl out_empty.ps
	./interp_s TTLs/forward.ttl out_forward.ps
	./interp_s TTLs/set1.ttl out_set1.ps
	./interp_s TTLs/donothing.ttl out_donothing.ps
	./interp_s TTLs/fail_parse_ok_interp.ttl out_fail_parse_ok_interp.ps
	./interp_s TTLs/set2.ttl out_set2.ps
	./interp_s TTLs/turn.ttl out_turn.ps
	./interp_s TTLs/spiral.ttl out_spiral.ps
	./interp_s TTLs/octagon1.ttl out_octagon1.ps
	./interp_s TTLs/octagon2.ttl out_octagon2.ps
	./interp_s TTLs/tunnel.ttl out_tunnel.ps
	./interp_s TTLs/labyrinth.ttl out_labyrinth.ps
	./interp_s TTLs/hypno.ttl out_hypno.ps
	./interp_s TTLs/5x5.ttl out_5x5.ps
	./interp_s TTLs/downarrow.ttl out_downarrow.ps

clean:
	rm -f parse_s interp_s extension_s compare_s out_*.txt out_*.ps out_*.pdf
