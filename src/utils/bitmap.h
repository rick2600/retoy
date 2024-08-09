#ifndef COMPILER_BITMAP_H
#define COMPILER_BITMAP_H

#include <stdio.h>
#include <stdint.h>

#define BITMAP_SIZE 256

typedef struct {
    uint8_t bits[BITMAP_SIZE / 8];
} bitmap_t;

void bitmap_set_bit(bitmap_t *bitmap, int bit);
void bitmap_clear_bit(bitmap_t *bitmap, int bit);
int bitmap_bit(const bitmap_t *bitmap, int bit);

#endif