#ifndef COMPILER_CODEGEN_H
#define COMPILER_CODEGEN_H

#include <stdio.h>
#include "../frontend/ast.h"
#include "bytecode.h"

bytecode_t* codegen(ast_node_t* node);

#endif
