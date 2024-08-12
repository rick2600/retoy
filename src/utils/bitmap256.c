#include "bitmap256.h"
#include <stdint.h>
#include <stdbool.h>


void bitmap256_set(bitmap256_t* bitmap, int bit) {
    bitmap->bits[bit / 8] |= (1 << (7 - (bit % 8)));
}


void bitmap256_clear(bitmap256_t* bitmap, int bit) {
    bitmap->bits[bit / 8] &= ~(1 << (7 - (bit % 8)));
}


bool bitmap256_test(const bitmap256_t* bitmap, int bit) {
    return (bitmap->bits[bit / 8] & (1 << (7 - (bit % 8)))) ? true : false;
}


void bitmap256_clear_all(bitmap256_t* bitmap) {
    memset(bitmap, 0, sizeof(bitmap256_t));
}
