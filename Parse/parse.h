#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define MAXNUMTOKENS 10000
#define MAXTOKENSIZE 50
#define strsame(A,B) (strcmp(A, B)==0)
#define FILELEN 1000
#define ARGCNOOUTPUT 2
#define ARGCWITHOUTPUT 3

#define ERROR(PHRASE) { fprintf(stderr, \
"Fatal Error %s occurred in %s, line %d\n", PHRASE, \
__FILE__, __LINE__);}

struct prog{
    char wds[MAXNUMTOKENS][MAXTOKENSIZE];
    int cw; // current word
};
typedef struct prog Program;

bool PROG(Program *p);
bool INSLST(Program *p);
bool INS(Program *p);
bool FWD(Program *p);
bool RGT(Program *p);
bool COL(Program *p);
bool LOOP(Program *p);
bool SET(Program *p);
bool VARNUM(Program *p);

// lasson
bool valid_uppercase(char c);

bool VAR(Program *p);
bool LTR(Program *p);
bool NUM(Program *p);

// lasson own function
bool valid_colour_word(Program * p);

bool WORD(Program *p);
bool LST(Program *p);
bool ITEMS(Program *p);
bool ITEM(Program *p);
bool PFIX(Program *p);
bool OP(Program *p);
