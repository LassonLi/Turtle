#pragma once

// #define FORMATSTR "%i"
#define FORMATSTR "%lf" // lasson, since I use double as stacktype

#define ELEMSIZE 20
#define STACKTYPE "Linked"

struct dataframe {
   stacktype i;
   struct dataframe* next;
};
typedef struct dataframe dataframe;

struct stack {
   /* Underlying array */
   dataframe* start;
   int size;
};
