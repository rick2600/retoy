#include "bitmap.h"
#include <stdint.h>


void bitmap_set_bit(bitmap_t *bitmap, int bit) {
    bitmap->bits[bit / 8] |= (1 << (7 - (bit % 8)));
}


void bitmap_clear_bit(bitmap_t *bitmap, int bit) {
    bitmap->bits[bit / 8] &= ~(1 << (7 - (bit % 8)));
}


int bitmap_bit(const bitmap_t *bitmap, int bit) {
    return (bitmap->bits[bit / 8] & (1 << (7 - (bit % 8)))) ? 1 : 0;
}
