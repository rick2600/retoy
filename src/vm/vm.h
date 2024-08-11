#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdint.h>
#include "../compiler/backend/bytecode.h"

bool VM(prog_t* prog, char* input);

#endif