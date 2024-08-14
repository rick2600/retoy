#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdint.h>
#include "../compiler/backend/bytecode.h"
#include "../re.h"


#define THREAD(x, y)        ((thread_t){.pc = (x), .sp = (y)})


enum { MAXTHREAD = 10000 };


typedef struct  {
    uint8_t *pc;
    uint8_t *sp;
} thread_t;


typedef struct {
    thread_t threads[MAXTHREAD];
    int count;
} thread_stack_t;


re_match_t* VM(prog_t* prog, char* input);

#endif