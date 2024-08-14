#ifndef COMPILER_RE_H
#define COMPILER_RE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



typedef struct re_submatch {
    uint8_t* start;
    uint8_t* end;
    struct re_submatch* next;
} re_submatch_t;


typedef struct re_match {
    uint8_t* input;
    re_submatch_t* head;
    re_submatch_t* tail;
} re_match_t;


re_submatch_t* re_submatch(uint8_t* start, uint8_t* end);
re_match_t* re_match_create(uint8_t* input);
void re_add_submatch(re_match_t* match, re_submatch_t* sub);
void re_print_match(re_match_t* match);
#endif