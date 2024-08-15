#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler/compiler.h"
#include "compiler/backend/bytecode.h"
#include "utils/opts.h"
#include "vm/vm.h"
#include "re.h"


static void run_prog(prog_t* prog) {
    char input[4096];
    while (fgets(input, sizeof(input), stdin) != NULL) {
        char* p = strchr(input, '\n');
        if (p) *p = '\0';
        re_match_t* match = VM(prog, input);
        if (match) {
            re_print_match(match);
            re_free(match);
        }
    }
}


int main(int argc, char** argv) {
    opts_t opts = opts_parser(argc, argv);
    if (opts.regex == NULL) {
        opts_help(argv[0]);
        exit(EXIT_SUCCESS);
    }
    //opts_print(&opts);
    prog_t* prog = compile(&opts);
    run_prog(prog);
    free(prog);
    exit(EXIT_SUCCESS);
}
