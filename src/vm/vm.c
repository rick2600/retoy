#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../compiler/backend/bytecode.h"


// https://swtch.com/~rsc/regexp/regexp2.html

typedef struct  {
    uint8_t *pc;
    uint8_t *sp;
} thread_t;


bool vm(bytecode_t* bc, char* input) {
    return false;
}


/*
inline static uint8_t fetch_operand8(uint8_t* mem) {
    return *mem;
}


inline static uint8_t fetch_operand32(uint8_t* mem) {
    return *((uint32_t*)(mem));
}


static bool vm_is_digit(uint8_t ch) {
    return ch >= '0' && ch <= '9';
}


static bool vm_is_wordchar(uint8_t ch) {
    if (ch >= '0' && ch <= '9') return true;
    if (ch >= 'a' && ch <= 'z') return true;
    if (ch >= 'A' && ch <= 'Z') return true;
    return false;
}


static bool vm_is_whitespace(uint8_t ch) {
    return ch == ' ';
}


bool vm(uint8_t* code, uint8_t* data, char* input) {
    enum { MAXTHREAD = 1000 };
    thread_t threads[MAXTHREAD];
    size_t nready = 0;

    char* endinput = input + strlen(input);

    uint8_t* pc;
    uint8_t* sp;
    uint32_t off;
    uint8_t* set;

    // queue initial thread
    threads[0].pc = code;
    threads[0].sp = input;
    nready++;

    while (nready > 0) {
        nready--;
        pc = threads[nready].pc;
        sp = threads[nready].sp;

        for (;;) {
            switch (*pc) {
            case OP_MATCH_CHAR_ANY:
                if (*sp == '\0') goto fail;
                pc++;
                sp++;
                continue;
            case OP_MATCH_CHAR:
                if (*sp != fetch_operand8(pc+1)) goto fail;
                pc += 2;
                sp++;
                continue;
            case OP_MATCH_STR:
                char* str = data + fetch_operand32(pc+1);
                if ((char *)(sp + strlen(str)) > endinput) goto fail;
                if (memcmp(sp, str, strlen(str)) != 0) goto fail;
                pc += 5;
                sp += strlen(str);
                continue;
            case OP_MATCH_DIGIT:
                if (!vm_is_digit(*sp)) goto fail;
                pc++;
                sp++;
                continue;
            case OP_MATCH_WORDCHAR:
                if (!vm_is_wordchar(*sp)) goto fail;
                pc++;
                sp++;
                continue;
            case OP_MATCH_WHITESPACE:
                if (!vm_is_whitespace(*sp)) goto fail;
                pc++;
                sp++;
                continue;
            case OP_ACCEPT:
                return true;
            case OP_JMP:
                pc = code + fetch_operand32(pc+1);
                continue;
            case OP_SPLIT:
                if (nready >= MAXTHREAD) {
                    fprintf(stderr, "regexp overflow");
                    return false;
                }
                // queue new thread
                uint32_t x = fetch_operand32(pc+1);
                uint32_t y = fetch_operand32(pc+1+4);
                threads[nready].pc = code + y;
                threads[nready].sp = sp;
                nready++;
                pc = code + x; // continue current thread
                continue;
            case OP_MATCHNOT_IN_SET:
                set = data + fetch_operand32(pc+1);
                if (set[*sp]) goto fail;
                pc += 5;
                sp++;
                continue;
            case OP_MATCH_IN_SET:
                uint8_t* set = data + fetch_operand32(pc+1);
                if (!set[*sp]) goto fail;
                pc += 5;
                sp++;
                continue;
            }
        }
        fail:;
    }
    return false;
}
*/