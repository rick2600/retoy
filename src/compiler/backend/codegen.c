#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "codegen.h"
#include "bytecode.h"
#include "../utils/bitmap256.h"


#define CURRENT_ADDRESS(bc) (((bc)->code).count)


static size_t emit8(bytecode_section_t* section, uint8_t data) {
    size_t written_addr = section->count;
    bytecode_write(section, &data, sizeof(uint8_t));
    return written_addr;
}


static size_t emit32(bytecode_section_t* section, uint32_t data) {
    size_t written_addr = section->count;
    bytecode_write(section, (uint8_t*)&data, sizeof(uint32_t));
    return written_addr;
}


static void patch32(bytecode_section_t* section, size_t addr, uint32_t data) {
    bytecode_patch_at(section, addr, (uint8_t*)&data, sizeof(uint32_t));
}


static size_t write_bitmap(bytecode_section_t* section, bitmap256_t* bitmap) {
    size_t written_addr = section->count;
    bytecode_write(section, bitmap->bits, sizeof(bitmap256_t));
    return written_addr;
}


static size_t do_codegen(bytecode_t* bc, ast_node_t* node) {
    size_t inst_addr = CURRENT_ADDRESS(bc);
    if (node == NULL) return inst_addr;

    if (node->quantifier) {
        ast_node_t* q = node->quantifier;
        node->quantifier = NULL;

        if (q->as.quantifier.lower == 0 && q->as.quantifier.upper == 1) {
            /*  e?
                    split L1, L2
                    L1: codes for e
                    L2:
            */
            emit8(&bc->code, OP_SPLIT);
            size_t l1_ref = emit32(&bc->code, 0);
            size_t l2_ref = emit32(&bc->code, 0);

            //L1:
            size_t l1_addr = do_codegen(bc, node);

            //L2:
            // ...
            patch32(&bc->code, l1_ref, l1_addr);
            patch32(&bc->code, l2_ref, CURRENT_ADDRESS(bc));
        }
        else if (q->as.quantifier.lower == 0 && q->as.quantifier.upper == INF) {
            /*  e*
                    L1: split L2, L3
                    L2: codes for e; jmp L1
                    L3:
            */
            // L1:
            size_t l1_addr = emit8(&bc->code, OP_SPLIT);
            size_t l2_ref = emit32(&bc->code, 0);
            size_t l3_ref = emit32(&bc->code, 0);

            // L2:
            size_t l2_addr = do_codegen(bc, node);
            emit8(&bc->code, OP_JMP);
            size_t l1_ref = emit32(&bc->code, 0);

            // L3:
            // ...
            patch32(&bc->code, l1_ref, l1_addr);
            patch32(&bc->code, l2_ref, l2_addr);
            patch32(&bc->code, l3_ref, CURRENT_ADDRESS(bc));
        }
        else if (q->as.quantifier.lower == 1 && q->as.quantifier.upper == INF) {
            /*  e+
                    L1: codes for e; split L1, L2
                    L2:
            */
            // L1:
            size_t l1_addr = do_codegen(bc, node);
            emit8(&bc->code, OP_SPLIT);
            size_t l1_ref = emit32(&bc->code, 0);
            size_t l2_ref = emit32(&bc->code, 0);

            // L2:
            // ...
            patch32(&bc->code, l1_ref, l1_addr);
            patch32(&bc->code, l2_ref, CURRENT_ADDRESS(bc));
        }
        else {
            size_t lower = q->as.quantifier.lower;
            size_t upper = q->as.quantifier.upper;

            for (size_t i = 0; i < lower; i++)
               do_codegen(bc, node);

            if (upper == INF) {
                // L1:
                size_t l1_addr = emit8(&bc->code, OP_SPLIT);
                size_t l2_ref = emit32(&bc->code, 0);
                size_t l3_ref = emit32(&bc->code, 0);

                // L2:
                size_t l2_addr = do_codegen(bc, node);
                emit8(&bc->code, OP_JMP);
                size_t l1_ref = emit32(&bc->code, 0);

                // L3:
                // ...
                patch32(&bc->code, l1_ref, l1_addr);
                patch32(&bc->code, l2_ref, l2_addr);
                patch32(&bc->code, l3_ref, CURRENT_ADDRESS(bc));
            }
            else {
                for (size_t i = 0; i < upper-lower; i++) {
                    emit8(&bc->code, OP_SPLIT);
                    size_t l1_ref = emit32(&bc->code, 0);
                    size_t l2_ref = emit32(&bc->code, 0);

                    // L1:
                    size_t l1_addr = do_codegen(bc, node);

                    // L2:
                    // ...
                    patch32(&bc->code, l1_ref, l1_addr);
                    patch32(&bc->code, l2_ref, CURRENT_ADDRESS(bc));
                }
            }

        }
        node->quantifier = q;
        return inst_addr;
    }

    if (node->type == NODE_ANY) {
        emit8(&bc->code, OP_MATCH_ANY);
    }
    else if (node->type == NODE_CHAR) {
        emit8(&bc->code, OP_MATCH_CHAR);
        emit8(&bc->code, node->as._char.value);
    }
    else if (node->type == NODE_CHAR_CLASS) {
        switch (node->as._char_class.value) {
            case DIGIT:             emit8(&bc->code, OP_MATCH_DIGIT);           break;
            case WORDCHAR:          emit8(&bc->code, OP_MATCH_WORDCHAR);        break;
            case WHITESPACE:        emit8(&bc->code, OP_MATCH_WHITESPACE);      break;
            case NOT_DIGIT:         emit8(&bc->code, OP_MATCHNOT_DIGIT);        break;
            case NOT_WORDCHAR:      emit8(&bc->code, OP_MATCHNOT_WORDCHAR);     break;
            case NOT_WHITESPACE:    emit8(&bc->code, OP_MATCHNOT_WHITESPACE);   break;
        }
    }
    else if (node->type == NODE_GROUP) {
        do_codegen(bc, node->as.group.expr);
    }

    else if (node->type == NODE_START_OF_LINE) {
        emit8(&bc->code, OP_MARK_SOL);
        do_codegen(bc, node->as.sol.expr);
    }

    else if (node->type == NODE_CONCAT) {
        do_codegen(bc, node->as.concat.left);
        do_codegen(bc, node->as.concat.right);
    }
    else if (node->type == NODE_ALTERNATION) {
        /*
        e1|e2
            split L1, L2
            L1: codes for e1; jmp L3
            L2: codes for e2
            L3:
        */
        emit8(&bc->code, OP_SPLIT);
        size_t l1_ref = emit32(&bc->code, 0);
        size_t l2_ref = emit32(&bc->code, 0);

        // L1:
        size_t l1_addr = do_codegen(bc, node->as.alternation.left);
        emit8(&bc->code, OP_JMP);
        size_t l3_ref = emit32(&bc->code, 0);

        // L2:
        size_t l2_addr = do_codegen(bc, node->as.alternation.right);

        // L3:
        // ...
        patch32(&bc->code, l1_ref, l1_addr);
        patch32(&bc->code, l2_ref, l2_addr);
        patch32(&bc->code, l3_ref, CURRENT_ADDRESS(bc));
    }
    else if (node->type == NODE_SET) {
        bitmap256_t bitmap;
        bitmap256_clear_all(&bitmap);
        ast_node_t* head = node->as.set.items.head;

        for (ast_node_t* item = head; item; item = item->next) {
            switch (item->type) {
                case NODE_CHAR_RANGE:
                    size_t lower = item->as._char_range.lower;
                    size_t upper = item->as._char_range.upper;
                    for (size_t i = lower; i <= upper; i++)
                        bitmap256_set(&bitmap, i);
                break;

                case NODE_CHAR:
                    bitmap256_set(&bitmap, item->as._char.value);
                break;

                case NODE_CHAR_CLASS:
                    switch (item->as._char_class.value) {
                        case DIGIT:
                            for (int i = '0'; i <= '9'; i++) bitmap256_set(&bitmap, i);
                        break;

                        case WORDCHAR:
                            for (int i = '0'; i <= '9'; i++) bitmap256_set(&bitmap, i);
                            for (int i = 'a'; i <= 'z'; i++) bitmap256_set(&bitmap, i);
                            for (int i = 'A'; i <= 'Z'; i++) bitmap256_set(&bitmap, i);
                            bitmap256_set(&bitmap, '_');
                        break;

                        case WHITESPACE:
                            // FIXME: Only space for now
                            bitmap256_set(&bitmap, ' ');
                        break;

                        case NOT_DIGIT:
                            for (int i = 0; i < 256; i++) {
                                if (i >= '0' && i <= '9') continue;
                                bitmap256_set(&bitmap, i);
                            }
                        break;

                        case NOT_WORDCHAR:
                            for (int i = 0; i < 256; i++) {
                                if (i >= '0' && i <= '9') continue;
                                if (i >= 'a' && i <= 'z') continue;
                                if (i >= 'A' && i <= 'Z') continue;
                                if (i == '_') continue;
                                bitmap256_set(&bitmap, i);
                            }
                        break;

                        case NOT_WHITESPACE:
                            for (int i = 0; i < 256; i++) {
                                if (i == ' ') continue;
                                bitmap256_set(&bitmap, i);
                            }
                        break;
                    }
                break;
            }
        }
        emit8(&bc->code, node->as.set.negative ? OP_MATCHNOT_IN_SET : OP_MATCH_IN_SET);
        size_t bitmap_ref = emit32(&bc->code, 0);
        size_t bitmap_addr = write_bitmap(&bc->data, &bitmap);
        patch32(&bc->code, bitmap_ref, bitmap_addr);
    }
    return inst_addr;
}


prog_t* codegen(ast_node_t* ast) {
    bytecode_t* bc = bytecode_alloc();
    do_codegen(bc, ast);
    emit8(&bc->code, OP_ACCEPT);
    prog_t* prog = bytecode2prog(bc);
    return prog;
}
