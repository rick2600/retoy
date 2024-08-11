#ifndef COMPILER_CODEGEN_H
#define COMPILER_CODEGEN_H

#include <stdio.h>
#include "../frontend/ast.h"
#include "bytecode.h"

prog_t* codegen(ast_node_t* ast);

#endif
