#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdint.h>
#include "../compiler/backend/bytecode.h"


#define THREAD(x, y)        ((thread_t){.pc = (x), .sp = (y)})
#define MATCH(x, y)         ((match_t){.start = (x), .end = (y)})
#define UNMATCH             ((match_t){.start = NULL, .end = NULL})


typedef struct {
    uint8_t* input;
    uint8_t* start;
    uint8_t* end;
} match_t;


enum { MAXTHREAD = 10000 };

typedef struct  {
    uint8_t *pc;
    uint8_t *sp;
} thread_t;


typedef struct {
    thread_t threads[MAXTHREAD];
    int count;
} thread_stack_t;


match_t VM(prog_t* prog, char* input);

#endif