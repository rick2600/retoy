#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdint.h>
#include "../compiler/backend/bytecode.h"


typedef struct {
    uint8_t* start;
    uint8_t* end;
} match_t;

match_t VM(prog_t* prog, char* input);

#endif