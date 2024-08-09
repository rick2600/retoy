#ifndef OPTS_H
#define OPTS_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char* regex;
    char* output;
    char* ast_file;
} opts_t;

void opts_help();
opts_t opts_parser(int argc, char** argv);
void opts_print(opts_t* opts);

#endif
