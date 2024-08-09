#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "./frontend/parser.h"
#include "./frontend/ast.h"
#include "./backend/bytecode.h"
#include "./backend/codegen.h"
#include "../utils/ast_json_dump.h"
#include "../utils/opts.h"


static void ast2file(const char *filename, const char *data) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        exit(EXIT_FAILURE);
    }
    if (fprintf(file, "%s\n", data) < 0) {
        fprintf(stderr, "Error writing to file %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
}


static void prog2file(const char* filename, const uint8_t* data, size_t data_size) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        exit(EXIT_FAILURE);
    }
    size_t written = fwrite(data, sizeof(uint8_t), data_size, file);
    if (written != data_size) {
        fprintf(stderr, "Error writing to file %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
}


static void dump_ast(opts_t* opts, ast_t* ast) {
    char* json = ast_json_dump(ast, opts->regex);
    ast2file(opts->ast_file, json);
    free(json);
}


static void dump_prog(opts_t* opts, prog_t* prog) {
    size_t size = sizeof(prog_t) + prog->header.code.size + prog->header.data.size;
    prog2file(opts->output, (uint8_t*)prog, size);
    free(prog);
}


bytecode_t* compile(opts_t* opts) {
    ast_node_t* ast = parse(opts->regex);
    bytecode_t* bc = codegen(ast);
    prog_t* prog = create_prog(bc);

    if (opts->ast_file != NULL) dump_ast(opts, ast);
    if (opts->output != NULL) dump_prog(opts, prog);
    ast_free(ast);
    return bc;
}
