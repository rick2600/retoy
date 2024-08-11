#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bytecode.h"


static size_t roundup(size_t size) {
    return (size + 16 - 1) & -16;
}


static void bytecode_ensure_section_size(bytecode_section_t* section, size_t write_size) {
    if (section->count + write_size > section->capacity) {
        section->capacity = roundup(section->count + write_size);
        section->mem = realloc(section->mem, section->capacity);
    }
}


void bytecode_patch_at(bytecode_section_t* section, size_t at, const uint8_t* data, size_t size) {
    // TODO: fix out-of-bound
    memcpy((uint8_t*)(section->mem + at), data, size);
}


void bytecode_write_at(bytecode_section_t* section, size_t at, const uint8_t* data, size_t size) {
    bytecode_ensure_section_size(section, size);
    memcpy((uint8_t*)(section->mem + at), data, size);
    section->count += size;
}


void bytecode_write(bytecode_section_t* section, const uint8_t* data, size_t size) {
    bytecode_write_at(section, section->count, data, size);
}


static void bytecode_section_init(bytecode_section_t* section) {
    section->mem = NULL;
    section->count = 0;
    section->capacity = 0;
}


bytecode_t* bytecode_alloc() {
    bytecode_t* bytecode = malloc(sizeof(bytecode_t));
    bytecode_section_init(&bytecode->code);
    bytecode_section_init(&bytecode->data);
    return bytecode;
}


void bytecode_free(bytecode_t* bytecode) {
    free(bytecode->code.mem);
    free(bytecode->data.mem);
    free(bytecode);
}


prog_t* bytecode2prog(bytecode_t* bytecode) {
    size_t code_size = bytecode->code.capacity;
    size_t data_size = bytecode->data.capacity;
    size_t size = sizeof(prog_t) + code_size + data_size;

    prog_t* prog = calloc(size, sizeof(uint8_t));
    prog->header.code.address =  (uint8_t*)&prog->mem - (uint8_t*)prog;
    prog->header.code.size = code_size;
    prog->header.data.address = prog->header.code.address + code_size;
    prog->header.data.size = data_size;

    uint8_t* code = (uint8_t*)prog + prog->header.code.address;
    uint8_t* data = (uint8_t*)prog + prog->header.data.address;
    memcpy(code, bytecode->code.mem, bytecode->code.count * sizeof(uint8_t));
    memcpy(data, bytecode->data.mem, bytecode->data.count * sizeof(uint8_t));
    return prog;
}
