#ifndef COMPILER_RE_H
#define COMPILER_RE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RESET   "\033[0m"         // Reset to default color
#define BLACK   "\033[1;30m"      // Black
#define RED     "\033[1;31m"      // Red
#define GREEN   "\033[1;32m"      // Green
#define YELLOW  "\033[1;33m"      // Yellow
#define BLUE    "\033[1;34m"      // Blue
#define MAGENTA "\033[1;35m"      // Magenta
#define CYAN    "\033[1;36m"      // Cyan
#define WHITE   "\033[1;37m"      // White


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
void re_free(re_match_t* match);
#endif