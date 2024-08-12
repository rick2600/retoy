#ifndef COMPILER_BITMAP_H
#define COMPILER_BITMAP_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


#define BITMAP_SIZE 256

typedef struct {
    uint8_t bits[BITMAP_SIZE / 8];
} bitmap256_t;

void bitmap256_set(bitmap256_t* bitmap, int bit);
void bitmap256_clear(bitmap256_t* bitmap, int bit);
bool bitmap256_test(const bitmap256_t* bitmap, int bit);
void bitmap256_clear_all(bitmap256_t* bitmap);


#endif