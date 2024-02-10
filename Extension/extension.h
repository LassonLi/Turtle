#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include "../Stack/stack.h"
#include "neillsdl2.h"

#define MAXNUMTOKENS 10000
#define MAXTOKENSIZE 50
#define strsame(A,B) (strcmp(A, B)==0)
#define FILELEN 400
#define ARGCNOOUTPUT 2
#define ARGCWITHOUTPUT 3
#define BOARDWIDTH 51
#define BOARDHEIGHT 33
#define CIRCLE180 180.0
#define CIRCLE360 360.0
#define CIRCLE90 90.0
#define M_PI 3.14159265358979323846
#define RGBMAX 255

#define ALPHASIZE 26

// Converts N in degrees to radians
#define deg_to_rad(N) (N * M_PI / CIRCLE180)

#define ERROR(PHRASE) { fprintf(stderr, \
"Fatal Error %s occurred in %s, line %d\n", PHRASE, \
__FILE__, __LINE__);}

typedef struct variable{
    double val;
    bool is_set;
} variable;

typedef struct variable_col{
    char val[MAXTOKENSIZE];
    bool is_set;
} variable_col;

typedef struct position{
    int y; // height, pos_row
    int x; // width, pos_col
} position;


struct prog{
    char wds[MAXNUMTOKENS][MAXTOKENSIZE];
    char grid[BOARDHEIGHT][BOARDWIDTH+1]; // 2d array to store colour in a board
    int cnt; // mark down how many words in a file

    bool is_write_txt; // true mean write to txt; false mean print to screen
    int loop_col_or_figure; // what is the current loop processing? 0 mean still don't know; 1 mean colour; 2 mean double
    char output_filepath[200]; // if write to txt, save this

    double rot; // Current rotation of curser in degrees
    position coords; // similar to pos, but different when processing
    int pos_row; // initial in 16-1, height, y
    int pos_col; // initial in 26-1, width, x
    variable var[ALPHASIZE]; // to store double value for A - Z
    variable_col var_col[ALPHASIZE]; // to store WORD, colour value for A - Z

    variable loop_figure[FILELEN]; // to store the double value in a loop
    variable_col loop_col[FILELEN]; // to store WORD in a loop

    int loop_figure_index; // to store the double value index in a loop
    int loop_col_index; // to store the colour index in a loop

    /* Used for storing variable index values,*/
    double result;
    //variable values, numbers and the result of PFIX sums; not include WORD or colour value 
    double result_figure; 
    char result_col[MAXTOKENSIZE]; // used for storing variable's colour value

    int cw; // current word

    // Used for creating SDL window and graphics
    SDL_Simplewin sw;
};
typedef struct prog Program;

bool PROG(Program *p);
bool INSLST(Program *p);
bool INS(Program *p);
bool FWD(Program *p);
bool BWD(Program *p);
bool RGT(Program *p);
bool LGT(Program *p);
bool COL(Program *p);
bool LOOP(Program *p);

bool REPEAT_LOOP(Program *p, int var_index, int list_len);

bool SET(Program *p);
bool RESET(Program *p);
bool VARNUM(Program *p);

// lasson
bool valid_uppercase(char c);

bool VAR(Program *p);
bool LTR(Program *p);
bool NUM(Program *p);

// lasson own function
bool valid_colour_word(char c[MAXTOKENSIZE]);

bool WORD(Program *p);
bool LST(Program *p);
bool ITEMS(Program *p);
bool ITEM(Program *p);

bool calc_PFIX(Program *p, stack* s);

bool PFIX(Program *p, stack* s);
bool OP(Program *p);

// lasson own function
double mod_360(double num);
void init_prog(Program *p);
/* Updates coordinates based on current rotation
and given distance using trigonometry */
void update_coords(Program* p);

int sign(int x);
// map BLACK TO 'K'
char map_colour(Program *p);
// draw line
void draw_line(Program* prog, int x1,int y1, int x2, int y2);
// print board to txt
void print_to_txt(Program* p);
//
bool find_end_cw_if_no_content_in_loop(Program* p);

void init_SDL(Program* p);
void run_SDL(Program* p);