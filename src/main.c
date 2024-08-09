#include <stdio.h>
#include <stdlib.h>
#include "compiler/compiler.h"
#include "compiler/backend/bytecode.h"
#include "utils/opts.h"



int main(int argc, char** argv) {
    opts_t opts = opts_parser(argc, argv);
    opts_print(&opts);

    if (opts.regex == NULL)
        opts_help(argv[0]);

    bytecode_t* bc = compile(&opts);
    bytecode_free(bc);
    return 0;
}
