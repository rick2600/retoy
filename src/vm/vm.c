#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../compiler/backend/bytecode.h"
#include "../utils/bitmap256.h"
#include "../re.h"
#include "vm.h"
#include "../cjson/cJSON.h"



thread_stack_t stack;

vm_stats_t stats;

/*
static void print_header(prog_t* prog) {
    char sep[128] = {0};
    memset(sep, '=', 80);
    printf("%s\n", sep);
    printf(".code at 0x%04lx\n", prog->header.code.address);
    printf(".data at 0x%04lx\n", prog->header.data.address);
    memset(sep, '-', 80);
    printf("%s\n", sep);
}
*/

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


static void vm_stats_init(vm_stats_t* stats) {
    stats->count = 0;
    stats->capacity = 0;
    stats->threads = NULL;
}


static void vm_stats_ensure_size(vm_stats_t* stats) {
    if (stats->count + 1 > stats->capacity) {
        stats->capacity = (stats->count == 0) ? 1 : 2 * stats->capacity;
        stats->threads = realloc(stats->threads, stats->capacity * sizeof(uint32_t));
    }
}


static void update_stats(vm_stats_t* stats, int n_threads) {
    vm_stats_ensure_size(stats);
    stats->threads[stats->count] = n_threads;
    stats->count++;
}

// https://swtch.com/~rsc/regexp/regexp2.html
re_submatch_t* doVM(prog_t* prog, char* input) {
    uint8_t* code = (uint8_t*)prog + prog->header.code.address;
    uint8_t* data = (uint8_t*)prog + prog->header.data.address;
    uint8_t* sp = (uint8_t*)input;
    bitmap256_t* bitmap256 = NULL;

    stack.count = 0;
    push(&stack, THREAD(code, sp));

    bool eol = false;

    while (stack.count > 0) {
        thread_t t = pop(&stack);
        for (;;) {
            update_stats(&stats, stack.count+1);
            switch (*t.pc) {
            case OP_MARK_SOL:
                t.pc++;
                continue;
            case OP_MARK_EOL:
                eol = true;
                t.pc++;
                continue;
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
                bitmap256 = (bitmap256_t*)(data + fetch_operand32(t.pc + 1));
                if (!bitmap256_test(bitmap256, *t.sp)) goto fail;
                t.pc += 5; t.sp++;
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
                bitmap256 = (bitmap256_t*)(data + fetch_operand32(t.pc + 1));
                if (bitmap256_test(bitmap256, *t.sp)) goto fail;
                t.pc += 5; t.sp++;
                continue;
            case OP_ACCEPT:
                if (eol) {
                    if (*t.sp != '\0') goto fail;
                }
                return re_submatch(sp, t.sp-1);
            case OP_JMP:
                t.pc = code + fetch_operand32(t.pc + 1);
                continue;
            case OP_SPLIT:
                uint32_t br0 = fetch_operand32(t.pc + 1);
                uint32_t br1 = fetch_operand32(t.pc + 1 + 4);
                if (stack.count < MAXTHREAD)
                    push(&stack, THREAD(code + br1, t.sp)); // queue new thread
                t.pc = code + br0;                          // continue current thread
                continue;
            default:
                fprintf(stderr, "invalid opcode: 0x%02x\n", *t.pc);
                return NULL;
                break;
            }
        }
        fail: ;
    }
    return NULL;
}


static void dump_stats(vm_stats_t* stats) {
    char* filename = "/tmp/retoy.stats.json";
    cJSON* stats_obj = cJSON_CreateArray();
    for (uint32_t i = 0; i < stats->count; i++) {
        cJSON_AddItemToArray(stats_obj, cJSON_CreateNumber(stats->threads[i]));
    }
    char* data = cJSON_Print(stats_obj);
    cJSON_Delete(stats_obj);
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
    free(data);
}


re_match_t* VM(prog_t* prog, char* input) {
    vm_stats_init(&stats);
    uint8_t* code = (uint8_t*)prog + prog->header.code.address;

    re_match_t* match = NULL;

    if (*code == OP_MARK_SOL) {
        re_submatch_t* sub = doVM(prog, input);
        if (sub) {
            match = re_match_create((uint8_t*)input);
            re_add_submatch(match, sub);
        }
    }
    else  {
        char* cur = input;
        while (*cur) {
            re_submatch_t* sub = doVM(prog, cur);
            if (sub) {
                if (match == NULL) match = re_match_create((uint8_t*)input);
                re_add_submatch(match, sub);
            }
            cur++;
        }
    }
    dump_stats(&stats);
    free(stats.threads);
    return match;
}
