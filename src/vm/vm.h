#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdint.h>
#include "../compiler/backend/bytecode.h"
#include "../re.h"


#define THREAD(x, y)        ((thread_t){.pc = (x), .sp = (y)})


enum { MAXTHREAD = 9999 };


typedef struct  {
    uint8_t *pc;
    uint8_t *sp;
} thread_t;


typedef struct {
    thread_t threads[MAXTHREAD];
    int count;
} thread_stack_t;


typedef struct {
    size_t count;
    size_t capacity;
    uint32_t* threads;
} vm_stats_t;


re_match_t* VM(prog_t* prog, char* input);

#endif