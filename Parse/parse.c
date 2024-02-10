#include "parse.h"

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

    fclose(fp);
    
    assert(cnt<MAXNUMTOKENS);
    if (PROG(prog)){
        free(prog);
        // printf("Parsed OK\n");
        return 0;
    }
    else{
        free(prog);
        ERROR("Parsed failed ");
        exit(EXIT_FAILURE);
    }
}

// done
bool PROG(Program *p)
{
    if(!strsame(p->wds[p->cw], "START")){
        // ERROR("No START statement ");
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
    && !(strsame(p->wds[p->cw], "RIGHT"))
    && !(strsame(p->wds[p->cw], "COLOUR")) 
    && !(strsame(p->wds[p->cw], "LOOP"))
    && !(strsame(p->wds[p->cw], "SET"))
    ){
        // char inserror[FILELEN];
        // strcat(inserror, "Expecting a FORWARD or RIGHT or COLOUR or LOOP or SET, but only get: ");
        // strcat(inserror, p->wds[p->cw]);
        // ERROR(inserror);
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
    if(strsame(p->wds[p->cw], "RIGHT")){
        if(RGT(p)){
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

    return false;
}

// 
bool FWD(Program *p)
{
    p->cw = p->cw + 1;
    if (VARNUM(p)){
        return true;
    }
    return false;
}

// 
bool RGT(Program *p)
{
    p->cw = p->cw + 1;
    if (VARNUM(p)){
        return true;
    }
    return false;
}

bool COL(Program *p)
{

    p->cw = p->cw + 1;
    if (VAR(p)){
        return true;
    }
    if (WORD(p)){
        if(valid_colour_word(p)){
            return true;
        }

        /*
        char colerror[FILELEN];
        strcat(colerror, "Expecting a valid colour \"BLACK\" \"RED\" \"GREEN\"  \"BLUE\"" \
        "\"YELLOW\"  \"CYAN\"  \"MAGENTA\"  \"WHITE\" , but only get: ");
        strcat(colerror, p->wds[p->cw]);
        ERROR(colerror);
        */
    }

    return false;

}

bool LOOP(Program *p)
{
    p->cw = p->cw + 1;
    if(!LTR(p)){
        return false;
    }
    // char looperror[FILELEN];

    // "OVER" 
    p->cw = p->cw + 1;
    if(!(strsame(p->wds[p->cw], "OVER"))){
        // strcat(looperror, "Expecting a OVER, but only get: ");
        // strcat(looperror, p->wds[p->cw]);
        // ERROR(looperror);
        return false;
    }

    p->cw = p->cw + 1;
    if(!LST(p)){
        return false;
    }

    p->cw = p->cw + 1;
    if(!INSLST(p)){
        return false;
    }
    return true;

}

bool SET(Program *p)
{
    p->cw = p->cw + 1;
    if(!LTR(p)){
        return false;
    }
    // char seterror[FILELEN];

    // "(" 
    p->cw = p->cw + 1;
    if(!(strsame(p->wds[p->cw], "("))){
        // strcat(seterror, "Expecting a (, but only get: ");
        // strcat(seterror, p->wds[p->cw]);
        // ERROR(seterror);
        return false;
    }

    p->cw = p->cw + 1;
    if(!PFIX(p)){
        return false;
    }
    return true;
}

bool VARNUM(Program *p)
{
    if(VAR(p)){
        return true;
    }
    if(NUM(p)){
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

// difficult
bool VAR(Program *p)
{
    // char varerror[FILELEN];
    // strcat(varerror, "Expecting a var look like $A $Z, but only get: ");
    // strcat(varerror, p->wds[p->cw]);
    if(p->wds[p->cw][0] != '$'){
        // ERROR(varerror);
        return false;
    }
    if (!valid_uppercase(p->wds[p->cw][1])){
        // ERROR(varerror);
        return false;
    }
    if(strlen(p->wds[p->cw]) != 2){
        // ERROR(varerror);
        return false;
    }
    return true;
}

// difficult
bool LTR(Program *p)
{
    // char ltrerror[FILELEN];
    // strcat(ltrerror, "Expecting A B C D ... Z, but only get: ");
    // strcat(ltrerror, p->wds[p->cw]);

    if (!valid_uppercase(p->wds[p->cw][0]) || strlen(p->wds[p->cw]) != 1){
        // ERROR(ltrerror);
        return false;
    }

    return true;
}


// done
bool NUM(Program *p)
{
    double number;
    if (sscanf(p->wds[p->cw], "%lf", &number) == 1){
        return true;
    }

    // char numerror[FILELEN];
    // strcat(numerror, "Expecting a double number, but only get: ");
    // strcat(numerror, p->wds[p->cw]);
    // ERROR(numerror);

    return false;
}

// lasson own function
// valid colour include "BLACK", "RED", "GREEN", "BLUE",
// "YELLOW", "CYAN", "MAGENTA", "WHITE"
bool valid_colour_word(Program * p){
    if(strsame(p->wds[p->cw], "\"BLACK\"")){
        return true;
    }
    if(strsame(p->wds[p->cw], "\"RED\"")){
        return true;
    }
    if(strsame(p->wds[p->cw], "\"GREEN\"")){
        return true;
    }
    if(strsame(p->wds[p->cw], "\"BLUE\"")){
        return true;
    }
    if(strsame(p->wds[p->cw], "\"YELLOW\"")){
        return true;
    }
    if(strsame(p->wds[p->cw], "\"CYAN\"")){
        return true;
    }
    if(strsame(p->wds[p->cw], "\"MAGENTA\"")){
        return true;
    }
    if(strsame(p->wds[p->cw], "\"WHITE\"")){
        return true;
    }

    return false;
}

// difficult
// valid colour include "BLACK", "RED", "GREEN", "BLUE",
// "YELLOW", "CYAN", "MAGENTA", "WHITE"
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

    // char worderror[FILELEN];
    // strcat(worderror, "Expecting something like \"s\", but only get: ");
    // strcat(worderror, p->wds[p->cw]);
    // ERROR(worderror);
    return false;
}

bool LST(Program *p)
{
    // char lsterror[FILELEN];

    if(!(strsame(p->wds[p->cw], "{"))){
        // strcat(lsterror, "Expecting a {, but only get: ");
        // strcat(lsterror, p->wds[p->cw]);
        // ERROR(lsterror);
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
        return true;
    }
    if(WORD(p)){
        return true;
    }
    return false;
}

bool PFIX(Program *p)
{
    if((strsame(p->wds[p->cw], ")"))){
        return true;
    }

    if(!OP(p) && !VARNUM(p)){
        return false;
    }

    p->cw = p->cw+1;
    if(PFIX(p)){
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

    // char operror[FILELEN];

    // strcat(operror, "Expecting a + - * /, but only get: ");
    // strcat(operror, p->wds[p->cw]);
    // ERROR(operror);

    return false;
}

// test every function
void test()
{
    // test PROG function START END
    Program *p = (Program *) calloc(1, sizeof(Program));
    strcpy(p->wds[0], "START");
    strcpy(p->wds[1], "END");
    assert(PROG(p) == true);
    free(p);

    // test PROG function START EDD
    Program *p1 = (Program *) calloc(1, sizeof(Program));
    strcpy(p1->wds[0], "START");
    strcpy(p1->wds[1], "EDD");
    assert(PROG(p1) == false);
    free(p1);

    // test PROG function START FORWARD 10 END
    Program *p2 = (Program *) calloc(1, sizeof(Program));
    strcpy(p2->wds[0], "START");
    strcpy(p2->wds[1], "FORWARD");
    strcpy(p2->wds[2], "10");
    strcpy(p2->wds[3], "END");
    assert(PROG(p2) == true);
    free(p2);

    // test INSLST function FORWARD 10 END
    Program *p3 = (Program *) calloc(1, sizeof(Program));
    strcpy(p3->wds[0], "FORWARD");
    strcpy(p3->wds[1], "10");
    strcpy(p3->wds[2], "END");
    assert(INSLST(p3) == true);
    free(p3);

    // test INSLST function COLOUR "RED" END
    Program *p4 = (Program *) calloc(1, sizeof(Program));
    strcpy(p4->wds[0], "COLOUR");
    strcpy(p4->wds[1], "\"RED\"");
    strcpy(p4->wds[2], "END");
    assert(INSLST(p4) == true);
    free(p4);

    // test INS function FORWARD 10
    Program *p5 = (Program *) calloc(1, sizeof(Program));
    strcpy(p5->wds[0], "FORWARD");
    strcpy(p5->wds[1], "10");
    assert(INS(p5) == true);
    free(p5);

    // test INS function LOOP A OVER { 10 20 } FORWARD $A END
    Program *p6 = (Program *) calloc(1, sizeof(Program));
    strcpy(p6->wds[0], "LOOP");
    strcpy(p6->wds[1], "A");
    strcpy(p6->wds[2], "OVER");
    strcpy(p6->wds[3], "{");
    strcpy(p6->wds[4], "10");
    strcpy(p6->wds[5], "20");
    strcpy(p6->wds[6], "}");
    strcpy(p6->wds[7], "FORWARD");
    strcpy(p6->wds[8], "$A");
    strcpy(p6->wds[9], "END");
    assert(INS(p6) == true);
    free(p6);

    // test FWD function FORWARD 10
    Program *p7 = (Program *) calloc(1, sizeof(Program));
    strcpy(p7->wds[0], "FORWARD");
    strcpy(p7->wds[1], "10");
    p7->cw = 0;
    assert(FWD(p7) == true);
    free(p7);

    // test FWD function FORWARD $A
    Program *p8 = (Program *) calloc(1, sizeof(Program));
    strcpy(p8->wds[0], "FORWARD");
    strcpy(p8->wds[1], "$A");
    p8->cw = 0;
    assert(FWD(p8) == true);
    free(p8);

    // test RGT function RIGHT 10
    Program *p9 = (Program *) calloc(1, sizeof(Program));
    strcpy(p9->wds[0], "RIGHT");
    strcpy(p9->wds[1], "10");
    p9->cw = 0;
    assert(RGT(p9) == true);
    free(p9);

    // test RGT function RIGHT $A
    Program *p10 = (Program *) calloc(1, sizeof(Program));
    strcpy(p10->wds[0], "RIGHT");
    strcpy(p10->wds[1], "$A");
    p10->cw = 0;
    assert(RGT(p10) == true);
    free(p10);

    // test COL function COLOUR "RED"
    Program *p11 = (Program *) calloc(1, sizeof(Program));
    strcpy(p11->wds[0], "COLOUR");
    strcpy(p11->wds[1], "\"RED\"");
    p11->cw = 0;
    assert(COL(p11) == true);
    free(p11);

    // test COL function COLOUR $A
    Program *p12 = (Program *) calloc(1, sizeof(Program));
    strcpy(p12->wds[0], "COLOUR");
    strcpy(p12->wds[1], "$A");
    p12->cw = 0;
    assert(COL(p12) == true);
    free(p12);

    // test COL function COLOUR RED
    Program *p13 = (Program *) calloc(1, sizeof(Program));
    strcpy(p13->wds[0], "COLOUR");
    strcpy(p13->wds[1], "RED");
    p13->cw = 0;
    assert(COL(p13) == false);
    free(p13);

    // test LOOP function LOOP A OVER { 10 20 } FORWARD $A END
    Program *p14 = (Program *) calloc(1, sizeof(Program));
    strcpy(p14->wds[0], "LOOP");
    strcpy(p14->wds[1], "A");
    strcpy(p14->wds[2], "OVER");
    strcpy(p14->wds[3], "{");
    strcpy(p14->wds[4], "10");
    strcpy(p14->wds[5], "20");
    strcpy(p14->wds[6], "}");
    strcpy(p14->wds[7], "FORWARD");
    strcpy(p14->wds[8], "$A");
    strcpy(p14->wds[9], "END");
    p14->cw = 0;
    assert(LOOP(p14) == true);
    free(p14);

    // test SET function SET A ( 10 + 20 )
    Program *p15 = (Program *) calloc(1, sizeof(Program));
    strcpy(p15->wds[0], "SET");
    strcpy(p15->wds[1], "A");
    strcpy(p15->wds[2], "(");
    strcpy(p15->wds[3], "10");
    strcpy(p15->wds[4], "+");
    strcpy(p15->wds[5], "20");
    strcpy(p15->wds[6], ")");
    p15->cw = 0;
    assert(SET(p15) == true);
    free(p15);

    // TEST VARNUM function $A
    Program *p16 = (Program *) calloc(1, sizeof(Program));
    strcpy(p16->wds[0], "$A");
    p16->cw = 0;
    assert(VARNUM(p16) == true);
    free(p16);

    // TEST VARNUM function 10
    Program *p17 = (Program *) calloc(1, sizeof(Program));
    strcpy(p17->wds[0], "10");
    p17->cw = 0;
    assert(VARNUM(p17) == true);
    free(p17);

    // test valid_uppercase function
    assert(valid_uppercase('A') == true);
    assert(valid_uppercase('Z') == true);
    assert(valid_uppercase('a') == false);
    assert(valid_uppercase('z') == false);
    assert(valid_uppercase('1') == false);

    // test VAR function $A
    Program *p18 = (Program *) calloc(1, sizeof(Program));
    strcpy(p18->wds[0], "$A");
    p18->cw = 0;
    assert(VAR(p18) == true);
    free(p18);

    // test VAR function A
    Program *p19 = (Program *) calloc(1, sizeof(Program));
    strcpy(p19->wds[0], "A");
    p19->cw = 0;
    assert(VAR(p19) == false);
    free(p19);

    // test LTR function A
    Program *p20 = (Program *) calloc(1, sizeof(Program));
    strcpy(p20->wds[0], "A");
    p20->cw = 0;
    assert(LTR(p20) == true);
    free(p20);

    // test LTR function $A
    Program *p21 = (Program *) calloc(1, sizeof(Program));
    strcpy(p21->wds[0], "$A");
    p21->cw = 0;
    assert(LTR(p21) == false);
    free(p21);

    // test NUM function 10
    Program *p22 = (Program *) calloc(1, sizeof(Program));
    strcpy(p22->wds[0], "10");
    p22->cw = 0;
    assert(NUM(p22) == true);
    free(p22);

    // test NUM function A
    Program *p23 = (Program *) calloc(1, sizeof(Program));
    strcpy(p23->wds[0], "A");
    p23->cw = 0;
    assert(NUM(p23) == false);
    free(p23);

    // test valid_colour_word function
    Program *p24 = (Program *) calloc(1, sizeof(Program));
    strcpy(p24->wds[0], "\"BLACK\"");
    p24->cw = 0;
    assert(valid_colour_word(p24) == true);
    free(p24);

    // test WORD function "BLACK"
    Program *p25 = (Program *) calloc(1, sizeof(Program));
    strcpy(p25->wds[0], "\"BLACK\"");
    p25->cw = 0;
    assert(WORD(p25) == true);
    free(p25);

    // test WORD function BLACK
    Program *p26 = (Program *) calloc(1, sizeof(Program));
    strcpy(p26->wds[0], "BLACK");
    p26->cw = 0;
    assert(WORD(p26) == false);
    free(p26);

    // test LST function { 10 20 }
    Program *p27 = (Program *) calloc(1, sizeof(Program));
    strcpy(p27->wds[0], "{");
    strcpy(p27->wds[1], "10");
    strcpy(p27->wds[2], "20");
    strcpy(p27->wds[3], "}");
    p27->cw = 0;
    assert(LST(p27) == true);
    free(p27);

    // test LST function { 10 20
    Program *p28 = (Program *) calloc(1, sizeof(Program));
    strcpy(p28->wds[0], "{");
    strcpy(p28->wds[1], "10");
    strcpy(p28->wds[2], "20");
    p28->cw = 0;
    assert(LST(p28) == false);
    free(p28);

    // test ITEMS function 10 20 }
    Program *p29 = (Program *) calloc(1, sizeof(Program));
    strcpy(p29->wds[0], "10");
    strcpy(p29->wds[1], "20");
    strcpy(p29->wds[2], "}");
    p29->cw = 0;
    assert(ITEMS(p29) == true);
    free(p29);

    // test ITEMS function 10 20
    Program *p30 = (Program *) calloc(1, sizeof(Program));
    strcpy(p30->wds[0], "10");
    strcpy(p30->wds[1], "20");
    p30->cw = 0;
    assert(ITEMS(p30) == false);
    free(p30);

    // test ITEM function 10
    Program *p31 = (Program *) calloc(1, sizeof(Program));
    strcpy(p31->wds[0], "10");
    p31->cw = 0;
    assert(ITEM(p31) == true);
    free(p31);

    // test ITEM function A
    Program *p32 = (Program *) calloc(1, sizeof(Program));
    strcpy(p32->wds[0], "A");
    p32->cw = 0;
    assert(ITEM(p32) == false);
    free(p32);

    // test PFIX function 1 2 + )
    Program *p33 = (Program *) calloc(1, sizeof(Program));
    strcpy(p33->wds[0], "1");
    strcpy(p33->wds[1], "2");
    strcpy(p33->wds[2], "+");
    strcpy(p33->wds[3], ")");
    p33->cw = 0;
    assert(PFIX(p33) == true);
    free(p33);

    // test OP function +
    Program *p34 = (Program *) calloc(1, sizeof(Program));
    strcpy(p34->wds[0], "+");
    p34->cw = 0;
    assert(OP(p34) == true);
    free(p34);

    // test OP function A
    Program *p35 = (Program *) calloc(1, sizeof(Program));
    strcpy(p35->wds[0], "A");
    p35->cw = 0;
    assert(OP(p35) == false);
    free(p35);
}

