#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "token.h"

#define INF ((uint32_t)-1)

typedef enum {
    NODE_QUANTIFIER,
    NODE_GROUP,
    NODE_ALTERNATION,
    NODE_CONCAT,
    NODE_SET,
    NODE_ANY,
    NODE_CHAR,
    NODE_CHAR_CLASS,
    NODE_CHAR_RANGE,
    NODE_START_OF_LINE,
    NODE_END_OF_LINE
} ast_node_type_t;


typedef struct ast_node {
    ast_node_type_t type;
    struct ast_node* quantifier;
    struct ast_node* next;
    union {
        struct {
        } any;

        struct {
            uint32_t lower;
            uint32_t upper;
        } quantifier;

        struct {
            struct ast_node* expr;
        } group;

        struct {
            struct ast_node* left;
            struct ast_node* right;
        } concat;

        struct {
            struct ast_node* left;
            struct ast_node* right;
        } alternation;

        struct {
            bool negative;
            struct {
                struct ast_node* head;
                struct ast_node* tail;
            } items;
        } set;

        struct {
            int value;
        } _char_class;

        struct {
            char lower;
            char upper;
        } _char_range;

        struct {
            int value;
        } _char;

        struct {
            struct ast_node* expr;
        } sol;      // start-of-line

        struct {
        } eol;      // end-of-line
    } as;
} ast_node_t;

typedef ast_node_t ast_t;

ast_node_t* ast_node_any();
ast_node_t* ast_node_quantifier(uint32_t lower, uint32_t upper);
ast_node_t* ast_node_group(ast_node_t* expr);
ast_node_t* ast_node_alternation(ast_node_t* left, ast_node_t* right);
ast_node_t* ast_node_concat(ast_node_t* left, ast_node_t* right);
ast_node_t* ast_node_set(bool negative);
ast_node_t* ast_node_char_class(int value);
ast_node_t* ast_node_char(int value);
ast_node_t* ast_node_char_range(char lower, char upper);
ast_node_t* ast_node_start_of_line(ast_node_t* expr);

void ast_node_set_add_item(ast_node_t* node, ast_node_t* item);
void ast_free(ast_node_t* node);

#endif

