#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../compiler/backend/bytecode.h"
#include "../utils/bitmap.h"

#define THREAD(x, y) ((thread_t){.pc = (x), .sp = (y)})
enum { MAXTHREAD = 1000 };


typedef struct  {
    uint8_t *pc;
    uint8_t *sp;
} thread_t;


typedef struct {
    thread_t threads[MAXTHREAD];
    int count;
} thread_stack_t;


static void print_header(prog_t* prog) {
    char sep[128] = {0};
    memset(sep, '=', 80);
    printf("%s\n", sep);
    printf(".code at 0x%04lx\n", prog->header.code.address);
    printf(".data at 0x%04lx\n", prog->header.data.address);
    memset(sep, '-', 80);
    printf("%s\n", sep);
}


static void push(thread_stack_t* stack, thread_t t) {
    stack->threads[stack->count] = t;
    stack->count++;
}


static thread_t pop(thread_stack_t* stack) {
    stack->count--;
    return stack->threads[stack->count];
}


inline static uint8_t fetch_operand8(uint8_t* mem) {
    return *mem;
}


inline static uint8_t fetch_operand32(uint8_t* mem) {
    return *((uint32_t*)(mem));
}


static bool is_digit(uint8_t ch) {
    return ch >= '0' && ch <= '9';
}


static bool is_wordchar(uint8_t ch) {
    if (ch >= '0' && ch <= '9') return true;
    if (ch >= 'a' && ch <= 'z') return true;
    if (ch >= 'A' && ch <= 'Z') return true;
    if (ch == '_') return true;
    return false;
}


static bool is_whitespace(uint8_t ch) {
    return ch == ' ';
}


// https://swtch.com/~rsc/regexp/regexp2.html
bool VM(prog_t* prog, char* input) {
    uint8_t* code = (uint8_t*)prog + prog->header.code.address;
    uint8_t* data = (uint8_t*)prog + prog->header.data.address;
    uint8_t* sp = (uint8_t*)input;
    bitmap_t* bitmap = NULL;

    thread_stack_t stack;
    stack.count = 0;

    push(&stack, THREAD(code, sp));

    while (stack.count > 0) {
        thread_t t = pop(&stack);
        for (;;) {
            switch (*t.pc) {
            case OP_MATCH_ANY:
                if (*t.sp == '\0') goto fail;
                t.pc++; t.sp++;
                continue;
            case OP_MATCH_CHAR:
                if (*t.sp != fetch_operand8(t.pc + 1)) goto fail;
                t.pc += 2; t.sp++;
                continue;
            case OP_MATCH_DIGIT:
                if (!is_digit(*t.sp)) goto fail;
                t.pc++; t.sp++;
                continue;
            case OP_MATCH_WORDCHAR:
                if (!is_wordchar(*t.sp)) goto fail;
                t.pc++; t.sp++;
                continue;
            case OP_MATCH_WHITESPACE:
                if (!is_whitespace(*t.sp)) goto fail;
                t.pc++; t.sp++;
                continue;
            case OP_MATCH_IN_SET:
                bitmap = (bitmap_t*)(data + fetch_operand32(t.pc + 1));
                if (!bitmap_bit(bitmap, *t.sp)) goto fail;
                t.pc += 5;
                t.sp++;
                continue;
            case OP_MATCHNOT_DIGIT:
                if (is_digit(*t.sp)) goto fail;
                t.pc++; t.sp++;
                continue;
            case OP_MATCHNOT_WORDCHAR:
                if (is_wordchar(*t.sp)) goto fail;
                t.pc++; t.sp++;
                continue;
            case OP_MATCHNOT_WHITESPACE:
                if (is_whitespace(*t.sp)) goto fail;
                t.pc++; t.sp++;
                continue;
            case OP_MATCHNOT_IN_SET:
                bitmap = (bitmap_t*)(data + fetch_operand32(t.pc + 1));
                if (bitmap_bit(bitmap, *t.sp)) goto fail;
                t.pc += 5;
                t.sp++;
                continue;
            case OP_ACCEPT:
                return true;
            case OP_JMP:
                t.pc = code + fetch_operand32(t.pc + 1);
                continue;
            case OP_SPLIT:
                if (stack.count >= MAXTHREAD) {
                    fprintf(stderr, "regexp overflow");
                    return false;
                }
                uint32_t br0 = fetch_operand32(t.pc + 1);
                uint32_t br1 = fetch_operand32(t.pc + 1 + 4);
                push(&stack, THREAD(code + br1, t.sp)); // queue new thread
                t.pc = code + br0;                      // continue current thread
                continue;
            }
        }
        fail:;
    }
    return false;
}
