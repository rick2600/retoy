#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../compiler/frontend/token.h"
#include "../cjson/cJSON.h"
#include "ast_json_dump.h"


static char* char_class_to_str(int type) {
    switch (type) {
        case DIGIT:             return "digit";
        case WORDCHAR:          return "wordchar";
        case WHITESPACE:        return "whitespace";
        case NOT_DIGIT:         return "not digit";
        case NOT_WORDCHAR:      return "not wordchar";
        case NOT_WHITESPACE:    return "not whitespace";
        default:                return "unknown";
    }
}

static char* node_type_to_str(ast_node_type_t type) {
    switch (type) {
        case NODE_QUANTIFIER:       return "quantifier";
        case NODE_GROUP:            return "group";
        case NODE_ALTERNATION:      return "alternation";
        case NODE_CONCAT:           return "concat";
        case NODE_ANY:              return "any";
        case NODE_CHAR:             return "char";
        case NODE_CHAR_CLASS:       return "char_class";
        case NODE_CHAR_RANGE:       return "char_range";
        case NODE_SET:              return "char_set";
        case NODE_START_OF_LINE:    return "start_of_line";
        case NODE_END_OF_LINE:      return "end_of_line";
        default:                    return "unknown";
    }
}



static cJSON* visit(ast_node_t* node) {
    cJSON* obj = NULL;
    if (node == NULL) return NULL;
    char* node_type = node_type_to_str(node->type);
    obj = cJSON_CreateObject();
    cJSON_AddItemToObject(obj, "type", cJSON_CreateString(node_type));

    if (node->type == NODE_QUANTIFIER) {
        cJSON* quantifier = cJSON_CreateArray();
        cJSON_AddItemToArray(quantifier, cJSON_CreateNumber(node->as.quantifier.lower));
        cJSON_AddItemToArray(quantifier, cJSON_CreateNumber(node->as.quantifier.upper));
        cJSON_AddItemToObject(obj, "range", quantifier);
    }
    else if (node->type == NODE_CHAR_CLASS) {
        char *class_name = char_class_to_str(node->as._char_class.value);
        cJSON_AddItemToObject(obj, "class", cJSON_CreateString(class_name));
    }
    else if (node->type == NODE_CHAR) {
        cJSON_AddNumberToObject(obj, "char", node->as._char.value);
    }
    else if (node->type == NODE_ALTERNATION) {
        cJSON_AddItemToObject(obj, "left", visit(node->as.alternation.left));
        cJSON_AddItemToObject(obj, "right", visit(node->as.alternation.right));
    }
    else if (node->type == NODE_CONCAT) {
        cJSON_AddItemToObject(obj, "left", visit(node->as.alternation.left));
        cJSON_AddItemToObject(obj, "right", visit(node->as.alternation.right));
    }
    else if (node->type == NODE_GROUP) {
        cJSON_AddItemToObject(obj, "expr", visit(node->as.group.expr));
    }
    else if (node->type == NODE_CHAR_RANGE) {
        char lower = node->as._char_range.lower;
        char upper = node->as._char_range.upper;
        cJSON* range = cJSON_CreateArray();
        cJSON_AddItemToArray(range, cJSON_CreateNumber(lower));
        cJSON_AddItemToArray(range, cJSON_CreateNumber(upper));
        cJSON_AddItemToObject(obj, "range", range);
    }
    else if (node->type == NODE_SET) {
        cJSON_AddItemToObject(obj, "negative", cJSON_CreateBool(node->as.set.negative));
        cJSON* items = cJSON_CreateArray();
        cJSON_AddItemToObject(obj, "items", items);
        ast_node_t* head = node->as.set.items.head;
        for (ast_node_t* item = head; item; item = item->next) {
            cJSON_AddItemToArray(items, visit(item));
        }
    }
    else if (node->type == NODE_ANY) {
    }
    else if (node->type == NODE_START_OF_LINE) {
    }
    else if (node->type == NODE_END_OF_LINE) {
    }

    if (node->quantifier)
        cJSON_AddItemToObject(obj, "quantifier", visit(node->quantifier));
    return obj;
}


char* ast_json_dump(ast_node_t *node, const char* regex) {
    cJSON* json_obj = cJSON_CreateObject();
    cJSON* ast_obj = visit(node);
    cJSON* regex_obj = cJSON_CreateString(regex);
    cJSON_AddItemToObject(json_obj, "regex", regex_obj);
    cJSON_AddItemToObject(json_obj, "ast", ast_obj);
    char* output = cJSON_Print(json_obj);
    cJSON_Delete(json_obj);
    return output;
}

