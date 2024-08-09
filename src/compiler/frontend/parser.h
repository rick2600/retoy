#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "token.h"
#include "ast.h"


typedef struct {
    token_t current;
    token_t previous;
    bool inside_set;
} parser_t;


ast_node_t* parse(const char* regex);

#endif


