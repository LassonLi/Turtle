#include "extension.h"

void test();

int main(int argc, char *argv[])
{
    // test(); // when test, comment out the following code
    FILE *fp = NULL;

    char filepath[FILELEN];
    if(argc == ARGCWITHOUTPUT){
        strcpy(filepath, argv[1]);
    }
    strcpy(filepath, argv[1]);
    fp = fopen(filepath, "r");

    if(fp == NULL){
        char fperror[FILELEN];
        strcat(fperror, "can not open file ");
        strcat(fperror, filepath);
        ERROR(fperror);
        exit(EXIT_FAILURE);
    }

    Program *prog = (Program *) calloc(1, sizeof(Program));
    int cnt=0;
    char tmp[MAXTOKENSIZE];
    while(!feof(fp) && cnt < MAXNUMTOKENS){
        fscanf(fp, "%s\r\n", tmp);
        strcpy(prog->wds[cnt],tmp);
        cnt++;
    }

    init_prog(prog); // init
    init_SDL(prog);
    prog->cnt = cnt;

    if(argc == ARGCWITHOUTPUT){
        prog->is_write_txt = true;
        strcpy(prog->output_filepath, argv[ARGCWITHOUTPUT - 1]); // argv[2] is the output_filepath
    }
    else{
        prog->is_write_txt = false;
    }

    fclose(fp);
    assert(cnt<MAXNUMTOKENS);
    if (PROG(prog)){
        if(argc == ARGCWITHOUTPUT){
            print_to_txt(prog);
        }
        else{
            run_SDL(prog);
        }
        // printf("interp OK\n");
        free(prog);
        return 0;
    }
    else{
        free(prog);
        ERROR("extension failed ");
        exit(EXIT_FAILURE);
    }
}

// done
bool PROG(Program *p)
{
    if(!strsame(p->wds[p->cw], "START")){
        return false;
    }
    p->cw = p->cw + 1;
    if (INSLST(p)){
        return true;
    }
    return false;
}

// done
bool INSLST(Program *p)
{
    if(strsame(p->wds[p->cw], "END")){
        return true;
    }

    if (INS(p)){
        p->cw = p->cw + 1;
    }
    else{
        return false;
    }

    if (INSLST(p)){
        return true;
    }

    return false;
}

bool INS(Program *p)
{
    if(!(strsame(p->wds[p->cw], "FORWARD")) 
    && !(strsame(p->wds[p->cw], "BACKWARD"))
    && !(strsame(p->wds[p->cw], "RIGHT"))
    && !(strsame(p->wds[p->cw], "LEFT"))
    && !(strsame(p->wds[p->cw], "COLOUR")) 
    && !(strsame(p->wds[p->cw], "LOOP"))
    && !(strsame(p->wds[p->cw], "SET"))
    && !(strsame(p->wds[p->cw], "RESET"))
    ){

        return false;
    }

    if(strsame(p->wds[p->cw], "FORWARD")){
        if(FWD(p)){
            return true;
        }
        else{
            return false;
        }
    }
    if(strsame(p->wds[p->cw], "BACKWARD")){
        if(BWD(p)){
            return true;
        }
        else{
            return false;
        }
    }
    if(strsame(p->wds[p->cw], "RIGHT")){
        if(RGT(p)){
            return true;
        }
        else{
            return false;
        }
    }
    if(strsame(p->wds[p->cw], "LEFT")){
        if(LGT(p)){
            return true;
        }
        else{
            return false;
        }
    }
    if(strsame(p->wds[p->cw], "COLOUR")){
        if(COL(p)){
            return true;
        }
        else{
            return false;
        }
    }
    if(strsame(p->wds[p->cw], "LOOP")){
        if(LOOP(p)){
            return true;
        }
        else{
            return false;
        }
    }
    if(strsame(p->wds[p->cw], "SET")){
        if(SET(p)){
            return true;
        }
        else{
            return false;
        }
    }
    if(strsame(p->wds[p->cw], "RESET")){
        if(RESET(p)){
            return true;
        }
        else{
            return false;
        }
    }

    return false;
}


bool FWD(Program *p)
{
    int prev_x = p->coords.x;
    int prev_y = p->coords.y;
    p->cw = p->cw + 1;
    if (VARNUM(p)){
        update_coords(p);
        draw_line(p, prev_x, prev_y, p->coords.x,p->coords.y);
        return true;
    }
    return false;
}

bool BWD(Program *p)
{
    int prev_x = p->coords.x;
    int prev_y = p->coords.y;
    p->cw = p->cw + 1;
    if (VARNUM(p)){
        p->result_figure = -p->result_figure;
        update_coords(p);
        draw_line(p, prev_x, prev_y, p->coords.x,p->coords.y);
        return true;
    }
    return false;
}


bool RGT(Program *p)
{
    p->cw = p->cw + 1;
    if (VARNUM(p)){
        p->rot = mod_360(p->rot - p->result_figure);

        return true;
    }
    return false;
}

bool LGT(Program *p)
{
    p->cw = p->cw + 1;
    if (VARNUM(p)){
        p->rot = mod_360(p->rot + p->result_figure);

        return true;
    }
    return false;
}

bool COL(Program *p)
{

    p->cw = p->cw + 1;
    if (VAR(p)){
        // check if trying to use an unset variable
        if(p->var_col[(int)p->result].is_set == false){
            return false;
        }

        strcpy(p->result_col, p->var_col[(int)p->result].val);
        if(!valid_colour_word(p->result_col)){
            return false;
        }

        return true;
    }
    if (WORD(p)){
        if(valid_colour_word(p->wds[p->cw])){
            strcpy(p->result_col, p->wds[p->cw]);
            return true;
        }

    }

    return false;

}

bool LOOP(Program *p)
{
    int var_index;
    p->cw = p->cw + 1;
    if(!LTR(p)){
        return false;
    }

    var_index = (int)p->result;

    // "OVER" 
    p->cw = p->cw + 1;
    if(!(strsame(p->wds[p->cw], "OVER"))){
        return false;
    }

    p->cw = p->cw + 1;
    if(!LST(p)){
        return false;
    }
    int list_len=0;
    double orig_loop_figure[FILELEN]; // to store the double value in a loop
    char orig_loop_col[FILELEN][MAXTOKENSIZE]; // to store WORD in a loop

    // set var_col, since each ITEM is WORD
    if(p->loop_col_or_figure == 1){
        p->var_col[var_index].is_set = true;
        list_len = p->loop_col_index;
        p->loop_col_index = 0; // reset in case nested loop
        for(int i=0; i<list_len; i++){
            strcpy(orig_loop_col[i], p->loop_col[i].val);
        }
    }
    // set var, since each ITEM is double
    else if(p->loop_col_or_figure == 2){
        p->var[var_index].is_set = true;
        list_len = p->loop_figure_index;
        p->loop_figure_index = 0; // reset in case nested loop
        for (int i=0; i<list_len; i++){
            orig_loop_figure[i] = p->loop_figure[i].val;
        }
    }
    int orig_loop_col_index = p->loop_col_index;
    int orig_loop_figure_index = p->loop_figure_index;

    p->cw = p->cw + 1;
    if(!REPEAT_LOOP(p, var_index, list_len)){
        return false;
    }

    int orig_loop_col_or_figure = p->loop_col_or_figure;

    // if list_len == 0, means no content in the loop, so do nothing, and
    // find the next END
    if (list_len==0){
        if(find_end_cw_if_no_content_in_loop(p)){
            return true;
        }
        else{
            return false;
        }
    }

    // if LST is { "BLACK" "BLACK" "RED" }, p->var_col[var_index].val = "RED"
    if (p->loop_col_or_figure == 1){
        strcpy(p->var_col[var_index].val, orig_loop_col[list_len-1]);
    }
    // if LST is { 1 3 5 }, p->var[var_index].val = 5
    else if (p->loop_col_or_figure == 2){
        p->var[var_index].val = orig_loop_figure[list_len-1];
    }

    // example: if LST is { 1 3 5 }, then in  REPEAT_LOOP, it only process the item 1 3
    if(!INSLST(p)){
        return false;
    }

    p->loop_col_or_figure = orig_loop_col_or_figure;
    p->loop_col_index = orig_loop_col_index;
    p->loop_figure_index = orig_loop_figure_index;
    return true;
}

// lasson still don't know how to process colour
// TODO
bool REPEAT_LOOP(Program *p, int var_index, int list_len)
{
    /*save original current word
    therefore it can be looped back to*/
    int orig_cw = p->cw;
    int orig_loop_col_or_figure = p->loop_col_or_figure;
    double orig_loop_figure[FILELEN]; // to store the double value in a loop
    char orig_loop_col[FILELEN][MAXTOKENSIZE]; // to store WORD in a loop

    // no content in the loop, so do nothing
    if(p->loop_col_or_figure == 0){
        return true;
    }

    if(p->loop_col_or_figure == 1){
        for(int i=0; i<list_len; i++){
            strcpy(orig_loop_col[i], p->loop_col[i].val);
        }
    }
    else{
        for (int i=0; i<list_len; i++){
            orig_loop_figure[i] = p->loop_figure[i].val;
        }
    }

    // so if list_len = 1, REPEAT_LOOP doesn't do anything
    if (list_len==1){
        return true;
    }
    // example: if LST is { 1 3 5 }, then in  REPEAT_LOOP, it only process the item 1 3
    // in LOOP, process the third item 5 
    for(int i=0; i<list_len-1; i++){
        p->cw = orig_cw;
        p->loop_col_or_figure = orig_loop_col_or_figure;
        // process col loop
        if (p->loop_col_or_figure == 1){
            // fill var_col the colour with loop_col[i]
            strcpy(p->var_col[var_index].val, orig_loop_col[i]);
        }
        // process figure loop
        else{
            p->var[var_index].val = orig_loop_figure[i];
        }
        if(!INSLST(p)){
            return false;
        }
    }

    p->cw = orig_cw;
    p->loop_col_or_figure = orig_loop_col_or_figure;
    return true;
}

bool SET(Program *p)
{
    int var_index;
    stack* s;
    p->cw = p->cw + 1;
    if(!LTR(p)){
        return false;
    }
    var_index = p->result;
    // "(" 
    p->cw = p->cw + 1;
    if(!(strsame(p->wds[p->cw], "("))){
        return false;
    }

    s = stack_init(); // ADT
    p->cw = p->cw + 1;

    if(!PFIX(p, s)){

        stack_free(s);
        return false;
    }

    /* Variable value at var_index
    is set to the result of polish calculation */
    p->var[var_index].is_set = true;
    p->var[var_index].val = p->result_figure;
    stack_free(s);

    return true;
}

bool RESET(Program *p)
{
    // just reset the coords, rot, and var
    p->coords.x = (int) (ceil(BOARDWIDTH / 2.0) - 1.0);
    p->coords.y = (int) (ceil(BOARDHEIGHT / 2.0) - 1.0);
    p->rot = mod_360(CIRCLE180);
    for(int i=0; i<ALPHASIZE; i++) {
        p->var[i].is_set = false;
        p->var_col[i].is_set = false;
    }

    // reset the grid
    for(int row=0; row<BOARDHEIGHT; row++){
        for(int col=0; col<BOARDWIDTH; col++){
            p->grid[row][col] = ' '; // initial to .
        }
    }

    /* Clear screen, & set draw colour to black */

    if(p->is_write_txt == false){
        // Wait for 1 second (1000 milliseconds)
        SDL_Delay(1000);
        SDL_SetRenderTarget(p->sw.renderer, p->sw.display);
        SDL_SetRenderDrawColor(p->sw.renderer, 0, 0, 0, 255);
        SDL_RenderClear(p->sw.renderer);
        Neill_SDL_UpdateScreen(&p->sw);
    }

    return true;
}

bool VARNUM(Program *p)
{
    if(VAR(p)){
        // check if trying to use an unset variable
        if(p->var[(int)p->result].is_set == false){
            return false;
        }

        p->result_figure = p->var[(int)p->result].val;
        return true;
    }
    if(NUM(p)){
        // already save p->result_figure in function NUM
        return true;
    }

    return false;
}

// lasson
bool valid_uppercase(char c){
    if(c >= 'A' && c <= 'Z'){
        return true;
    }
    return false;
}

bool VAR(Program *p)
{
    if(p->wds[p->cw][0] != '$'){
        return false;
    }
    if (!valid_uppercase(p->wds[p->cw][1])){
        return false;
    }
    if(strlen(p->wds[p->cw]) != 2){
        return false;
    }
    // since current token is similart to $C
    p->result = p->wds[p->cw][1] - 'A';
    return true;
}

bool LTR(Program *p)
{
    if (!valid_uppercase(p->wds[p->cw][0]) || strlen(p->wds[p->cw]) != 1){
        return false;
    }

    // since current token is similart to C
    p->result = p->wds[p->cw][0] - 'A';

    return true;
}


// done
bool NUM(Program *p)
{
    double number;
    if (sscanf(p->wds[p->cw], "%lf", &number) == 1){
        p->result_figure = number;
        return true;
    }

    return false;
}

// lasson own function
// valid colour include "BLACK", "RED", "GREEN", "BLUE",
// "YELLOW", "CYAN", "MAGENTA", "WHITE"
bool valid_colour_word(char c[MAXTOKENSIZE])
{
    if(strsame(c, "\"BLACK\"")){
        return true;
    }
    if(strsame(c, "\"RED\"")){
        return true;
    }
    if(strsame(c, "\"GREEN\"")){
        return true;
    }
    if(strsame(c, "\"BLUE\"")){
        return true;
    }
    if(strsame(c, "\"YELLOW\"")){
        return true;
    }
    if(strsame(c, "\"CYAN\"")){
        return true;
    }
    if(strsame(c, "\"MAGENTA\"")){
        return true;
    }
    if(strsame(c, "\"WHITE\"")){
        return true;
    }

    return false;
}

// valid colour include "BLACK", "RED", "GREEN", "BLUE",
// "YELLOW", "CYAN", "MAGENTA", "WHITE"
// fail 1 YELLOW 
bool WORD(Program *p)
{
    // lasson don't know whether write it in this way
    char w[MAXTOKENSIZE];
    if(sscanf(p->wds[p->cw], "\"%s\"", w) == 1){
        // just make sure there is something inside ""
        if (strlen(w)>=2){
            return true;
        }
    }

    return false;
}

bool LST(Program *p)
{

    if(!(strsame(p->wds[p->cw], "{"))){

        return false;
    }

    p->cw = p->cw+1;
    if(ITEMS(p)){
        return true;
    }
    return false;
}

bool ITEMS(Program *p)
{
    if((strsame(p->wds[p->cw], "}"))){
        return true;
    }

    if(!ITEM(p)){
        return false;
    }

    p->cw = p->cw+1;
    if(ITEMS(p)){
        return true;
    }
    return false;
}

bool ITEM(Program *p)
{
    if(VARNUM(p)){
        p->loop_col_or_figure = 2;
        p->loop_figure[p->loop_figure_index].val = p->result_figure;
        p->loop_figure[p->loop_figure_index].is_set = true;
        p->loop_figure_index++;
        return true;
    }
    if(WORD(p)){
        p->loop_col_or_figure = 1;
        // since this is a word, use strcpy
        strcpy(p->loop_col[p->loop_col_index].val, p->wds[p->cw]);
        p->loop_col[p->loop_col_index].is_set = true;
        p->loop_col_index++;
        return true;
    }
    return false;
}

bool calc_PFIX(Program *p, stack* s)
{
    double n1, n2;
    // if number or variable push result
    if(VARNUM(p)){
        stack_push(s, p->result_figure);
    }

    // if operand, pop 2 off stack and perform operation
    // push result back into stack
    else{
        // if there is less than 2 numbers on stack
        // give error
        if(!stack_pop(s, &n2) || !stack_pop(s, &n1)) {
            return false;
        }
        switch(p->wds[p->cw][0])
        {
            case '+':
                p->result_figure = n1 + n2;
                break;
            case '-':
                p->result_figure = n1 - n2;
                break;
            case '*':
                p->result_figure = n1 * n2;
                break;
            case '/':
                p->result_figure = n1 / n2;
                break;
        }
        stack_push(s, p->result_figure);
    }

    return true;
}

bool PFIX(Program *p, stack* s)
{
    double test;
    if((strsame(p->wds[p->cw], ")"))){
        stack_pop(s, &p->result_figure);
        /*check if stack is empty,
        if not empty, return false*/
        if(stack_peek(s, &test)){
            return false;
        }
        // printf("%lf\n", p->result_figure);

        // stack is empty, return true
        return true;
    }

    if(!OP(p) && !VARNUM(p)){

        return false;
    }

    if(!calc_PFIX(p, s)){

        return false;
    }

    p->cw = p->cw+1;
    if(PFIX(p, s)){
        return true;
    }

    return false;
}

bool OP(Program *p)
{
    if((strsame(p->wds[p->cw], "+"))){
        return true;
    }
    if((strsame(p->wds[p->cw], "-"))){
        return true;
    }
    if((strsame(p->wds[p->cw], "*"))){
        return true;
    }
    if((strsame(p->wds[p->cw], "/"))){
        return true;
    }

    return false;
}

double mod_360(double num)
{
    if (num <= CIRCLE360) {
        return num;
    }
    return num - CIRCLE360;
}

// lasson may know
void init_prog(Program *p)
{
    for(int i=0; i<BOARDHEIGHT; i++){
        for(int j=0; j<BOARDWIDTH; j++){
            p->grid[i][j] = ' '; // initial to .
        }
    }
    p->is_write_txt = true; // default true, because write to terminal annoying
    p->loop_col_or_figure = 0;

    // p->rot = mod_360(CIRCLE360 - CIRCLE90); // North
    p->rot = mod_360(CIRCLE180); // North

    // x = ceil(51 / 2) - 1 = 25
    p->coords.x = (int) (ceil(BOARDWIDTH / 2.0) - 1.0); 
    // y = ceil(33 / 2) - 1 = 16
    p->coords.y = (int) (ceil(BOARDHEIGHT / 2.0) - 1.0); 
    
    for(int i=0; i<ALPHASIZE; i++) {
        p->var[i].is_set = false;
        p->var_col[i].is_set = false;
    }

    for(int i=0; i<FILELEN; i++) {
        p->loop_figure[i].is_set = false;
        p->loop_col[i].is_set = false;
    }

    p->loop_figure_index = 0;
    p->loop_col_index = 0;

    p->result_figure = 0; // default 0
    strcpy(p->result_col, "\"WHITE\""); // default WHITE
}

// lasson don't know
void update_coords(Program* p)
{
    p->coords.x = p->coords.x 
    +round((p->result_figure * (sin(deg_to_rad(p->rot)))));

    if(p->coords.x < 0){
        p->coords.x = 0;
    }
    if(p->coords.x > BOARDWIDTH-1){
        p->coords.x = BOARDWIDTH-1;
    }

    p->coords.y = p->coords.y 
    +round((p->result_figure * (cos(deg_to_rad(p->rot)))));

    if (p->coords.y < 0){
        p->coords.y = 0;
    }
    if (p->coords.y > BOARDHEIGHT-1){
        p->coords.y = BOARDHEIGHT-1;
    }
}

int sign(int x)
{
    if(x>0){
        return 1;
    }
    else if(x<0){
        return -1;
    }
    else{
        return 0;
    }
}

char map_colour(Program *p)
{
    char c='W';
    //blac(K), (R)ed, (G)reen, (Y)ellow, (B)lue, (M)agenta, (C)yan and (W)hite
    if(strsame(p->result_col, "\"BLACK\"")){
        c = 'K';
    }
    if(strsame(p->result_col, "\"RED\"")){
        c = 'R';
    }
    if(strsame(p->result_col, "\"GREEN\"")){
        c = 'G';
    }
    if(strsame(p->result_col, "\"BLUE\"")){
        c = 'B';
    }
    if(strsame(p->result_col, "\"YELLOW\"")){
        c = 'Y';
    }
    if(strsame(p->result_col, "\"CYAN\"")){
        c = 'C';
    }
    if(strsame(p->result_col, "\"MAGENTA\"")){
        c = 'M';
    }
    if(strsame(p->result_col, "\"WHITE\"")){
        c = 'W';
    }
    return c;
}

void draw_line(Program* prog, int x1,int y1, int x2, int y2)
{
    int x,y,dx,dy,swap,temp,s1,s2,p,i;

    x=x1;
    y=y1;
    dx=abs(x2-x1);
    dy=abs(y2-y1);
    s1=sign(x2-x1);
    s2=sign(y2-y1);
    swap=0;

    char c = map_colour(prog);
    prog->grid[y1][x1] = c;
    if(dy>dx)
    {
        temp=dx;
        dx=dy;
        dy=temp;
        swap=1;
    }
    p=2*dy-dx;
    for(i=0;i<dx;i++)
    {
        prog->grid[y][x] = c;
        while(p>=0)
        {
            p=p-2*dx;
            if(swap){
                x+=s1;
            }
            else{
                y+=s2;
            }
        }
        p=p+2*dy;
        if(swap){
            y+=s2;
        }
        else{
            x+=s1;
        }
    }

    prog->grid[y2][x2] = c;
    // print sdl
    if (prog->is_write_txt == false){
        /* Draws line from previous coords to new updated coords */
        switch(c)
        {
            case 'K':
                Neill_SDL_SetDrawColour(&prog->sw, 0, 0, 0);
                break;
            case 'R':
                Neill_SDL_SetDrawColour(&prog->sw, RGBMAX, 0, 0);
                break;
            case 'G':
                Neill_SDL_SetDrawColour(&prog->sw, 0, RGBMAX, 0);
                break;
            case 'Y':
                Neill_SDL_SetDrawColour(&prog->sw, RGBMAX, RGBMAX, 0);
                break;
            case 'B':
                Neill_SDL_SetDrawColour(&prog->sw, 0, 0, RGBMAX);
                break;
            case 'M':
                Neill_SDL_SetDrawColour(&prog->sw, RGBMAX, 0, RGBMAX);
                break;
            case 'C':
                Neill_SDL_SetDrawColour(&prog->sw, 0, RGBMAX, RGBMAX);
                break;
            case 'W':
                Neill_SDL_SetDrawColour(&prog->sw, RGBMAX, RGBMAX, RGBMAX);
                break;
            default:
                Neill_SDL_SetDrawColour(&prog->sw, RGBMAX, RGBMAX, RGBMAX);
                break;
        }
        // Wait for 1 second (1000 milliseconds)
        SDL_Delay(1000);
        SDL_RenderDrawLine(prog->sw.renderer, (float)x1 * 10 + 100, (float)y1 * 10 + 100,
        (float)x2 * 10 + 100, (float)y2 * 10 + 100);
        Neill_SDL_UpdateScreen(&prog->sw);
    }
}

// print board to txt
void print_to_txt(Program* p)
{
    FILE *fpout = NULL;
    fpout = fopen(p->output_filepath, "w");
    for(int row=0; row<BOARDHEIGHT; row++){
        for(int col=0; col<BOARDWIDTH; col++){
            fprintf(fpout, "%c", p->grid[row][col]);
        }
        fprintf(fpout, "\n");
    }
    fclose(fpout);
}

bool find_end_cw_if_no_content_in_loop(Program* p)
{
    int cw = p->cw;
    while(cw < p->cnt && !(strsame(p->wds[cw], "END"))){
        cw++;
    }
    if(strsame(p->wds[cw], "END")){
        p->cw = cw;
        return true;
    }
    else{
        return false;
    }
}

void init_SDL(Program* p)
{
   Neill_SDL_Init(&p->sw);
   // set colour to white
   Neill_SDL_SetDrawColour(&p->sw, RGBMAX, RGBMAX, RGBMAX);
}

void run_SDL(Program* p)
{
   do
   {
      Neill_SDL_Events(&p->sw);
   } while (!p->sw.finished);
   SDL_Quit();
   atexit(SDL_Quit);
}

// test all functions except  print_to_txt , print_to_terminal
void test()
{
    //Test PROG function START END
    Program *testp1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp1);
    strcpy(testp1->wds[0], "START");
    strcpy(testp1->wds[1], "END");
    testp1->cw = 0;
    assert(PROG(testp1));
    free(testp1);
    // printf("PROG test OK\n");

    // Test PROG function SAAT END
    Program *testp1_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp1_1);
    strcpy(testp1_1->wds[0], "SAAT");
    strcpy(testp1_1->wds[1], "END");
    testp1_1->cw = 0;
    assert(!PROG(testp1_1));
    free(testp1_1);
    // printf("PROG false test OK\n");

    //Test SET function Set A ( 1 2 + 3 *)
    Program *testp2 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp2);
    strcpy(testp2->wds[0], "SET");
    strcpy(testp2->wds[1], "A");
    strcpy(testp2->wds[2], "(");
    strcpy(testp2->wds[3], "1");
    strcpy(testp2->wds[4], "2");
    strcpy(testp2->wds[5], "+");
    strcpy(testp2->wds[6], "3");
    strcpy(testp2->wds[7], "*");
    strcpy(testp2->wds[8], ")");
    testp2->cw = 0;
    assert(SET(testp2));
    assert((int)testp2->var[0].val == 9);
    free(testp2);
    // printf("SET test OK\n");

    //Test INSLST function FORWARD 10 END
    Program *testp3 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp3);
    strcpy(testp3->wds[0], "FORWARD");
    strcpy(testp3->wds[1], "10");
    strcpy(testp3->wds[2], "END");
    testp3->cw = 0;
    assert(INSLST(testp3));
    free(testp3);
    // printf("INSLST test OK\n");

    //Test INS function RIGHT 10
    Program *testp4 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp4);
    strcpy(testp4->wds[0], "RIGHT");
    strcpy(testp4->wds[1], "10");
    testp4->cw = 0;
    assert(INS(testp4));
    free(testp4);
    // printf("INS test OK\n");
    
    //Test FWD function FORWARD 10
    Program *testp5 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp5);
    strcpy(testp5->wds[0], "FORWARD");
    strcpy(testp5->wds[1], "10");
    testp5->cw = 0;
    assert(FWD(testp5));
    free(testp5);
    // printf("FWD test OK\n");

    //Test FWD function FORWARD $A
    Program *testp5_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp5_1);
    strcpy(testp5_1->wds[0], "FORWARD");
    strcpy(testp5_1->wds[1], "$A");
    testp5_1->cw = 0;
    testp5_1->var[0].is_set = true;
    testp5_1->var[0].val = 10;
    assert(FWD(testp5_1));
    free(testp5_1);
    // printf("FWD VAR test OK\n");

    //Test RGT function RIGHT 45
    Program *testp6 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp6);
    strcpy(testp6->wds[0], "RIGHT");
    strcpy(testp6->wds[1], "45");
    testp6->cw = 0;
    assert(RGT(testp6));
    free(testp6);
    // printf("RGT test OK\n");

    //Test RGT function RIGHT $A
    Program *testp6_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp6_1);
    strcpy(testp6_1->wds[0], "RIGHT");
    strcpy(testp6_1->wds[1], "$A");
    testp6_1->cw = 0;
    testp6_1->var[0].is_set = true;
    testp6_1->var[0].val = 45;
    assert(RGT(testp6_1));
    free(testp6_1);
    // printf("RGT VAR test OK\n");

    //Test COL function COLOUR "RED"
    Program *testp7 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp7);
    strcpy(testp7->wds[0], "COLOUR");
    strcpy(testp7->wds[1], "\"RED\"");
    testp7->cw = 0;
    assert(COL(testp7));
    free(testp7);
    // printf("COL WORD test OK\n");

    //Test COL function COLOUR $A
    Program *testp7_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp7_1);
    strcpy(testp7_1->wds[0], "COLOUR");
    strcpy(testp7_1->wds[1], "$A");
    testp7_1->cw = 0;
    testp7_1->var_col[0].is_set = true;
    strcpy(testp7_1->var_col[0].val, "\"BLUE\"");
    assert(COL(testp7_1));
    free(testp7_1);
    // printf("COL VAR test OK\n");


    //Test LOOP function LOOP C OVER { 3 4 5 } END
    Program *testp8 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp8);
    strcpy(testp8->wds[0], "LOOP");
    strcpy(testp8->wds[1], "C");
    strcpy(testp8->wds[2], "OVER");
    strcpy(testp8->wds[3], "{");
    strcpy(testp8->wds[4], "3");
    strcpy(testp8->wds[5], "4");
    strcpy(testp8->wds[6], "5");
    strcpy(testp8->wds[7], "}");
    strcpy(testp8->wds[8], "END");
    testp8->cw = 0;
    assert(LOOP(testp8));
    assert((int)testp8->var[2].val == 5);
    free(testp8);
    // printf("LOOP FIGURE test OK\n");

    //Test LOOP function LOOP C OVER { "RED" "GREEN" "BLUE" } END
    Program *testp8_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp8_1);
    strcpy(testp8_1->wds[0], "LOOP");
    strcpy(testp8_1->wds[1], "C");
    strcpy(testp8_1->wds[2], "OVER");
    strcpy(testp8_1->wds[3], "{");
    strcpy(testp8_1->wds[4], "\"RED\"");
    strcpy(testp8_1->wds[5], "\"GREEN\"");
    strcpy(testp8_1->wds[6], "\"BLUE\"");
    strcpy(testp8_1->wds[7], "}");
    strcpy(testp8_1->wds[8], "END");
    testp8_1->cw = 0;
    assert(LOOP(testp8_1));
    assert(strsame(testp8_1->var_col[2].val, "\"BLUE\""));
    free(testp8_1);
    // printf("LOOP COL test OK\n");

    //Test LOOP function Loop A OVER ( 1 2 3) SET A ($A 1 + ) RIGHT 90 FORWARD $A END
    Program *testp8_2 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp8_2);
    strcpy(testp8_2->wds[0], "LOOP");
    strcpy(testp8_2->wds[1], "A");
    strcpy(testp8_2->wds[2], "OVER");
    strcpy(testp8_2->wds[3], "{");
    strcpy(testp8_2->wds[4], "1");
    strcpy(testp8_2->wds[5], "2");
    strcpy(testp8_2->wds[6], "3");
    strcpy(testp8_2->wds[7], "}");
    strcpy(testp8_2->wds[8], "SET");
    strcpy(testp8_2->wds[9], "A");
    strcpy(testp8_2->wds[10], "(");
    strcpy(testp8_2->wds[11], "$A");
    strcpy(testp8_2->wds[12], "1");
    strcpy(testp8_2->wds[13], "+");
    strcpy(testp8_2->wds[14], ")");
    strcpy(testp8_2->wds[15], "RIGHT");
    strcpy(testp8_2->wds[16], "90");
    strcpy(testp8_2->wds[17], "FORWARD");
    strcpy(testp8_2->wds[18], "$A");
    strcpy(testp8_2->wds[19], "END");
    testp8_2->cw = 0;
    assert(LOOP(testp8_2));
    assert((int)testp8_2->var[0].val == 4);
    assert(testp8_2->grid[16][25]=='W');
    assert(testp8_2->grid[16][28]==' ');
    assert(testp8_2->grid[17][27]=='W');
    assert(testp8_2->grid[18][27]=='W');
    assert(testp8_2->grid[19][27]=='W');
    assert(testp8_2->grid[20][27]==' ');
    assert(testp8_2->grid[19][26]=='W');
    assert(testp8_2->grid[19][25]=='W');
    assert(testp8_2->grid[19][24]=='W');
    assert(testp8_2->grid[19][23]=='W');
    assert(testp8_2->grid[19][22]==' ');
    free(testp8_2);
    // printf("LOOP PFIX test OK\n");

    // SET A ($A 1 + ) RIGHT 90 FORWARD $A END
    // Test REPEAT_LOOP function SET A ($A 1 + ) RIGHT 90 FORWARD $A END
    Program *testp9 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp9);
    strcpy(testp9->wds[0], "SET");
    strcpy(testp9->wds[1], "A");
    strcpy(testp9->wds[2], "(");
    strcpy(testp9->wds[3], "$A");
    strcpy(testp9->wds[4], "1");
    strcpy(testp9->wds[5], "+");
    strcpy(testp9->wds[6], ")");
    strcpy(testp9->wds[7], "RIGHT");
    strcpy(testp9->wds[8], "90");
    strcpy(testp9->wds[9], "FORWARD");
    strcpy(testp9->wds[10], "$A");
    strcpy(testp9->wds[11], "END");

    testp9->cw = 0;
    // manually set loop_figure = [1 2 5]
    testp9->var[0].is_set = true;
    testp9->var[0].val = 1;
    testp9->loop_col_or_figure = 2;
    testp9->loop_figure_index = 3;
    testp9->loop_figure[0].val = 1;
    testp9->loop_figure[0].is_set = true;
    testp9->loop_figure[1].val = 2;
    testp9->loop_figure[1].is_set = true;
    testp9->loop_figure[2].val = 5;
    testp9->loop_figure[2].is_set = true;

    assert(REPEAT_LOOP(testp9, 0, 3));
    assert((int)testp9->var[0].val == 3);
    assert(testp9->grid[16][25]=='W');
    assert(testp9->grid[16][26]=='W');
    assert(testp9->grid[16][28]==' ');
    assert(testp9->grid[17][27]=='W');
    assert(testp9->grid[18][27]=='W');
    assert(testp9->grid[19][27]=='W');
    assert(testp9->grid[20][27]==' ');
    assert(testp9->grid[19][26]==' ');
    assert(testp9->grid[19][25]==' ');
    assert(testp9->grid[19][24]==' ');
    assert(testp9->grid[19][23]==' ');
    assert(testp9->grid[19][22]==' ');
    free(testp9);
    // printf("REPEAT_LOOP PFIX test OK\n");

    // Test VARNUM function 10
    Program *testp10 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp10);
    strcpy(testp10->wds[0], "10");
    testp10->cw = 0;
    assert(VARNUM(testp10));
    assert((int)testp10->result_figure == 10);
    free(testp10);
    // printf("VARNUM NUM test OK\n");

    // Test VARNUM function $A
    Program *testp10_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp10_1);
    strcpy(testp10_1->wds[0], "$A");
    testp10_1->cw = 0;
    testp10_1->var[0].is_set = true;
    testp10_1->var[0].val = 10;
    assert(VARNUM(testp10_1));
    assert((int)testp10_1->result_figure == 10);
    free(testp10_1);
    // printf("VARNUM VAR test OK\n");

    // Test valid_uppercase function
    assert(valid_uppercase('A'));
    assert(valid_uppercase('Z'));
    assert(!valid_uppercase('a'));
    assert(!valid_uppercase('z'));
    // printf("valid_uppercase test OK\n");

    // Test VAR function $C
    Program *testp11 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp11);
    strcpy(testp11->wds[0], "$C");
    testp11->cw = 0;
    assert(VAR(testp11));
    assert((int)testp11->result == 2);
    free(testp11);
    // printf("VAR test OK\n");

    // Test LTR function C
    Program *testp12 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp12);
    strcpy(testp12->wds[0], "C");
    testp12->cw = 0;
    assert(LTR(testp12));
    assert((int)testp12->result == 2);
    free(testp12);
    // printf("LTR test OK\n");

    // Test NUM function 10
    Program *testp13 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp13);
    strcpy(testp13->wds[0], "10");
    testp13->cw = 0;
    assert(NUM(testp13));
    assert((int)testp13->result_figure == 10);
    free(testp13);
    // printf("NUM 10 test OK\n");

    // Test NUM function A
    // it should return false
    Program *testp13_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp13_1);
    strcpy(testp13_1->wds[0], "A");
    testp13_1->cw = 0;
    assert(!NUM(testp13_1));
    free(testp13_1);
    // printf("NUM A false test OK\n");

    // Test valid_colour_word function
    assert(valid_colour_word("\"BLACK\""));
    assert(valid_colour_word("\"RED\""));
    assert(!valid_colour_word("GARBAGE"));
    assert(!valid_colour_word("123"));
    assert(!valid_colour_word("RED"));
    // printf("valid_colour_word test OK\n");

    // Test WORD function "RED"
    Program *testp14 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp14);
    strcpy(testp14->wds[0], "\"RED\"");
    testp14->cw = 0;
    assert(WORD(testp14));
    free(testp14);
    // printf("WORD test OK\n");

    // Test LST function { 1 2 3 }
    Program *testp15 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp15);
    strcpy(testp15->wds[0], "{");
    strcpy(testp15->wds[1], "1");
    strcpy(testp15->wds[2], "2");
    strcpy(testp15->wds[3], "3");
    strcpy(testp15->wds[4], "}");
    testp15->cw = 0;
    assert(LST(testp15));
    assert((int)testp15->loop_figure_index == 3);
    assert(testp15->loop_col_or_figure == 2);
    assert((int)testp15->loop_figure[0].val == 1);
    assert((int)testp15->loop_figure[1].val == 2);
    assert((int)testp15->loop_figure[2].val == 3);
    free(testp15);
    // printf("LST test OK\n");

    // Test ITEMS function 1 2 3 }
    Program *testp16 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp16);
    strcpy(testp16->wds[0], "1");
    strcpy(testp16->wds[1], "2");
    strcpy(testp16->wds[2], "3");
    strcpy(testp16->wds[3], "}");
    testp16->cw = 0;
    assert(ITEMS(testp16));
    assert((int)testp16->loop_figure_index == 3);
    assert(testp16->loop_col_or_figure == 2);
    assert((int)testp16->loop_figure[0].val == 1);
    assert((int)testp16->loop_figure[1].val == 2);
    assert((int)testp16->loop_figure[2].val == 3);
    free(testp16);
    // printf("ITEMS test OK\n");

    // Test ITEM function 1
    Program *testp17 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp17);
    strcpy(testp17->wds[0], "1");
    testp17->cw = 0;
    assert(ITEM(testp17));
    assert((int)testp17->loop_figure_index == 1);
    assert(testp17->loop_col_or_figure == 2);
    assert((int)testp17->loop_figure[0].val == 1);
    free(testp17);
    // printf("ITEM test OK\n");

    // Test ITEM function "RED"
    Program *testp17_1 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp17_1);
    strcpy(testp17_1->wds[0], "\"RED\"");
    testp17_1->cw = 0;
    assert(ITEM(testp17_1));
    assert((int)testp17_1->loop_col_index == 1);
    assert(testp17_1->loop_col_or_figure == 1);
    assert(strsame(testp17_1->loop_col[0].val, "\"RED\""));
    free(testp17_1);
    // printf("ITEM test OK\n");

    // Test calc_PFIX function 1 2 +
    Program *testp18 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp18);
    strcpy(testp18->wds[0], "1");
    strcpy(testp18->wds[1], "2");
    strcpy(testp18->wds[2], "+");
    testp18->cw = 0;
    stack* s = stack_init();
    assert(calc_PFIX(testp18, s));
    assert((int)testp18->result_figure == 1);
    testp18->cw += 1;
    assert(calc_PFIX(testp18, s));
    assert((int)testp18->result_figure == 2);
    testp18->cw += 1;
    assert(calc_PFIX(testp18, s));
    assert((int)testp18->result_figure == 3);
    testp18->cw += 1;
    free(testp18);
    stack_free(s);
    // printf("calc_PFIX test OK\n");

    // Test PFIX function  1 2 + )
    Program *testp19 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp19);
    strcpy(testp19->wds[0], "1");
    strcpy(testp19->wds[1], "2");
    strcpy(testp19->wds[2], "+");
    strcpy(testp19->wds[3], ")");
    testp19->cw = 0;
    stack* s1 = stack_init();
    assert(PFIX(testp19, s1));
    assert((int)testp19->result_figure == 3);
    free(testp19);
    stack_free(s1);
    // printf("PFIX test OK\n");

    // Test OP function +
    Program *testp20 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp20);
    strcpy(testp20->wds[0], "+");
    testp20->cw = 0;
    assert(OP(testp20));
    free(testp20);
    // printf("OP test OK\n");

    // Test mod_360 function
    assert((int)mod_360(0) == 0);
    assert((int)mod_360(361) == 1);
    // printf("mod_360 test OK\n");

    // Test update_coords function
    Program *testp21 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp21);
    testp21->result_figure = 10;
    testp21->rot = 0;
    testp21->coords.x = 0;
    testp21->coords.y = 0;
    update_coords(testp21);
    assert(testp21->coords.x == 0);
    assert(testp21->coords.y == 10);
    free(testp21);
    // printf("update_coords test OK\n");

    // Test sign function
    assert(sign(0) == 0);
    assert(sign(1) == 1);
    assert(sign(-1) == -1);
    // printf("sign test OK\n");

    // Test map_colour function
    Program *testp22 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp22);
    strcpy(testp22->result_col, "\"RED\"");
    assert(map_colour(testp22) == 'R');
    strcpy(testp22->result_col, "\"GREEN\"");
    assert(map_colour(testp22) == 'G');

    free(testp22);
    // printf("map_colour test OK\n");

    // Test draw_line function
    Program *testp23 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp23);
    strcpy(testp23->result_col,"\"WHITE\"");
    draw_line(testp23, 16, 25, 16, 30);
    // x = col, y = row
    assert(testp23->grid[25][16]=='W');
    assert(testp23->grid[26][16]=='W');
    assert(testp23->grid[27][16]=='W');
    assert(testp23->grid[28][16]=='W');
    assert(testp23->grid[29][16]=='W');
    assert(testp23->grid[30][16]=='W');
    assert(testp23->grid[31][16]==' ');
    free(testp23);
    // printf("draw_line test OK\n");

    // Test init_prog function
    Program *testp24 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp24);
    assert(testp24->is_write_txt == true);
    assert(testp24->loop_col_or_figure == 0);
    assert((int)testp24->rot == 180);
    assert(testp24->coords.x == 25);
    assert(testp24->coords.y == 16);
    assert((int)testp24->result_figure == 0);
    free(testp24);
    // printf("init_prog test OK\n");

    // Test find_end_cw_if_no_content_in_loop function GARBAGE GARBAGE END
    Program *testp25 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp25);
    testp25->cnt = 3;
    strcpy(testp25->wds[0], "GARBAGE");
    strcpy(testp25->wds[1], "GARBAGE");
    strcpy(testp25->wds[2], "END");
    testp25->cw = 0;
    assert(find_end_cw_if_no_content_in_loop(testp25));
    assert(testp25->cw == 2);
    free(testp25);
    // printf("find_end_cw_if_no_content_in_loop test OK\n");

    // Test BWD function BACK 10
    Program *testp26 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp26);
    strcpy(testp26->wds[0], "BACK");
    strcpy(testp26->wds[1], "10");
    testp26->cw = 0;
    assert(BWD(testp26));
    free(testp26);
    // printf("BWD test OK\n");

    // Test LGT function LEFT 45
    Program *testp27 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp27);
    strcpy(testp27->wds[0], "LEFT");
    strcpy(testp27->wds[1], "45");
    testp27->cw = 0;
    assert(LGT(testp27));
    free(testp27);
    // printf("LGT test OK\n");

    // Test RESET function RESET
    Program *testp28 = (Program *) calloc(1, sizeof(Program));
    init_prog(testp28);
    strcpy(testp28->wds[0], "RESET");
    testp28->cw = 0;
    assert(RESET(testp28));
    assert((int)testp28->rot == 180);
    assert(testp28->coords.x == 25);
    assert(testp28->coords.y == 16);
    assert(testp28->grid[16][25]==' ');
    assert(testp28->grid[16][26]==' ');
    assert(testp28->grid[16][27]==' ');
    free(testp28);

}
