#ifndef COMPILER_SCANNER_H
#define COMPILER_SCANNER_H

#include "token.h"

typedef struct {
    const char* start;
    const char* current;
} scanner_t;

void scanner_init(const char* input);
token_t scan_token();

#endif


