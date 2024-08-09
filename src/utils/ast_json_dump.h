#ifndef AST_SHOW_H
#define AST_SHOW_H

#include "../compiler/frontend/ast.h"

char* ast_json_dump(ast_node_t *node, const char* regex);
#endif


