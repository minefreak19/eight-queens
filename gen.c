#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./defs.h"

#define A1H8_DIAG 0x0102040810204080llu
#define H1A8_DIAG 0x8040201008040201llu

static inline bool is_valid_sq(int sq) {
    return H1 <= sq && sq <= A8;
}

static inline Board sq_mask(Square sq) {
    assert(is_valid_sq(sq));
    return 1llu << sq;
}

static inline Board file_mask(Square sq) {
    // 0 -> h, 7 -> a
    uint8_t file = sq % 8; 
    const Board hfile = 0x0101010101010101llu;
    
    return hfile << file;
}

static inline Board rank_mask(Square sq) {
    // 0 -> 1st, 7 -> 8th
    uint8_t rank = sq / 8;
    const Board rank1 = 0x00000000000000ffllu;

    return rank1 << (rank * 8);
}

static inline Board diag_mask(Square sq) {
    Board result = 0;
    
    const Board main_diags [] = { A1H8_DIAG, H1A8_DIAG };

    for (int idx = 0; idx < 2; idx++) {
        const Board main_diag = main_diags[idx];
        for (int i = -7; i <= 7; i++) {
            Board diag = 0;
            // choice of left- and right-shifts here is arbitrary
            // the point is to implement the functionality of shifting in
            //   the opposite direction when i is negative
            if (i < 0) {
                diag = main_diag >> i*8;
            } else {
                diag = main_diag << i*8;
            }
            if (sq_mask(sq) & diag) {
                result |= diag;
                break;
            }
        }
    }

    return result;
}


// Includes the square sq on which the queen currently stands
Board queen_attacks(Square sq) {
    Board result = 0;

    result |= rank_mask(sq);
    result |= file_mask(sq);
    result |= diag_mask(sq);
    
    return result;
}

Board table[64];

int main(int argc, char **argv) {
    assert(argc >= 2);

    const char *file_path = argv[1];

    for (int i = 0; i < 64; i++) {
        table[i] = queen_attacks(i);
    }

    FILE *file = fopen(file_path, "wb");
    int result = fwrite(table, sizeof(Board), 64, file);
    assert(result == 64);
    fclose(file);

    return 0;
}
