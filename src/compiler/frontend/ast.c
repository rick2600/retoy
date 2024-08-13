#include "ast.h"
#include "token.h"
#include <stdlib.h>


ast_node_t* ast_create_node(ast_node_type_t type) {
    ast_node_t* node = malloc(sizeof(ast_node_t));
    if (node == NULL) {
        fprintf(stderr, "Could not allocate memory for ast_node\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->quantifier = NULL;
    node->next = NULL;
    return node;
}


ast_node_t* ast_node_any() {
    ast_node_t* node =  ast_create_node(NODE_ANY);
    return node;
}


ast_node_t* ast_node_quantifier(uint32_t lower, uint32_t upper) {
    ast_node_t* node =  ast_create_node(NODE_QUANTIFIER);
    node->as.quantifier.lower = lower;
    node->as.quantifier.upper = upper;
    return node;
}


ast_node_t* ast_node_group(ast_node_t* expr) {
    ast_node_t* node =  ast_create_node(NODE_GROUP);
    node->as.group.expr = expr;
    return node;
}


ast_node_t* ast_node_alternation(ast_node_t* left, ast_node_t* right) {
    ast_node_t* node =  ast_create_node(NODE_ALTERNATION);
    node->as.alternation.left = left;
    node->as.alternation.right = right;
    return node;
}


ast_node_t* ast_node_concat(ast_node_t* left, ast_node_t* right) {
    ast_node_t* node =  ast_create_node(NODE_CONCAT);
    node->as.concat.left = left;
    node->as.concat.right = right;
    return node;
}


ast_node_t* ast_node_set(bool negative) {
    ast_node_t* node =  ast_create_node(NODE_SET);
    node->as.set.negative = negative;
    node->as.set.items.head = NULL;
    node->as.set.items.tail = NULL;
    return node;
}


ast_node_t* ast_node_char_class(int value) {
    ast_node_t* node =  ast_create_node(NODE_CHAR_CLASS);
    node->as._char_class.value = value;
    return node;
}


ast_node_t* ast_node_char(int value) {
    ast_node_t* node =  ast_create_node(NODE_CHAR);
    node->as._char.value = value;
    return node;
}


ast_node_t* ast_node_char_range(char lower, char upper) {
    ast_node_t* node =  ast_create_node(NODE_CHAR_RANGE);
    node->as._char_range.lower = lower;
    node->as._char_range.upper = upper;
    return node;
}


void ast_node_set_add_item(ast_node_t* node, ast_node_t* item) {
    if (node->as.set.items.head == NULL)
        node->as.set.items.head = item;
    else
        node->as.set.items.tail->next = item;
    node->as.set.items.tail = item;
}



void ast_free(ast_node_t* node) {
    if (node == NULL) return;

    if (node->quantifier)
        ast_free(node->quantifier);

    switch(node->type) {
        case NODE_CONCAT:
            ast_free(node->as.concat.left);
            ast_free(node->as.concat.right);
            break;
        case NODE_ALTERNATION:
            ast_free(node->as.alternation.left);
            ast_free(node->as.alternation.right);
            break;
        case NODE_GROUP:
            ast_free(node->as.group.expr);
            break;
        case NODE_START_OF_LINE:
            break;
        case NODE_END_OF_LINE:
            break;
        case NODE_SET:
            for (ast_node_t* item = node->as.set.items.head; item; ) {
                ast_node_t* next = item->next;
                ast_free(item);
                item = next;
            }
            break;
        default:
            break;
    }
    free(node);
}


ast_node_t* ast_node_start_of_line() {
    ast_node_t* node =  ast_create_node(NODE_START_OF_LINE);
    return node;
}


ast_node_t* ast_node_end_of_line() {
    ast_node_t* node =  ast_create_node(NODE_END_OF_LINE);
    return node;
}
