#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "opts.h"


void opts_help(const char* prog_name) {
    printf(
        "Usage: %s [options]\n"
        "    -h, --help                 Print help menu\n"\
        "    -r, --regex                Regex\n"\
        "    -o, --output <compiled>    Compiled regex\n"\
        "    -a, --ast-file <file>      Dump AST (json) to file\n"\
        , prog_name
    );
    exit(EXIT_SUCCESS);
}


void opts_print(opts_t* opts) {
    printf("Regex:                 %s\n"\
           "Output filename:       %s\n"\
           "AST filename:          %s\n"\
           ,
           opts->regex, opts->output, opts->ast_file
           );
    printf("================================================================================\n");
}


opts_t opts_parser(int argc, char** argv) {
    opts_t opts = {
        .regex = NULL,
        .output = NULL,
        .ast_file = NULL,
    };
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"help",        no_argument,        0, 'h'},
        {"regex",       required_argument,  0, 'r'},
        {"output",      required_argument,  0, 'o'},
        {"ast-file",    required_argument,  0, 'a'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "hr:a:o:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'r':
                opts.regex = optarg;
                break;
            case 'a':
                opts.ast_file = optarg;
                break;
            case 'o':
                opts.output = optarg;
                break;
            case 'h':
            case '?':
            default:
                opts_help(argv[0]);
        }
    }
    return opts;
}
