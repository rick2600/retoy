#ifndef COMPILER_BYTECODE_H
#define COMPILER_BYTECODE_H

#include <stdio.h>
#include <stdint.h>


typedef enum {
    OP_NOP                  =  0,
    //
    OP_MATCH_ANY            =  1,
    OP_MATCH_CHAR           =  2,
    //
    OP_MATCH_DIGIT          =  3,
    OP_MATCH_WORDCHAR       =  4,
    OP_MATCH_WHITESPACE     =  5,
    OP_MATCH_IN_SET         =  6,
    //
    OP_MATCHNOT_DIGIT       =  7,
    OP_MATCHNOT_WORDCHAR    =  8,
    OP_MATCHNOT_WHITESPACE  =  9,
    OP_MATCHNOT_IN_SET      = 10,
    //
    OP_SPLIT                = 11,
    OP_JMP                  = 12,
    OP_ACCEPT               = 13,
    OP_MARK_SOL             = 14,
    OP_MARK_EOL             = 15
} opcode_t;


typedef struct {
    uint8_t* mem;
    size_t count;
    size_t capacity;
} bytecode_section_t;


typedef struct {
    bytecode_section_t code;
    bytecode_section_t data;
} bytecode_t;


typedef struct {
    size_t address;
    size_t size;
} prog_section_t;


typedef struct {
    struct {
        prog_section_t code;
        prog_section_t data;
    } header;
    uint8_t mem[];
} prog_t;


bytecode_t* bytecode_alloc();
void bytecode_free(bytecode_t* bytecode);
void bytecode_patch_at(bytecode_section_t* section, size_t at, const uint8_t* data, size_t size);
void bytecode_write_at(bytecode_section_t* section, size_t at, const uint8_t* data, size_t size);
void bytecode_write(bytecode_section_t* section, const uint8_t* data, size_t size);
prog_t* bytecode2prog(bytecode_t* bytecode);

#endif