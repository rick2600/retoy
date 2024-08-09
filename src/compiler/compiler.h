#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "../utils/opts.h"
#include "./backend/bytecode.h"

bytecode_t* compile(opts_t* opts);

#endif

