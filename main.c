#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// a8 b8 c8 d8 .. h8
// .
// .
// .
// a1 b1 c1 d1 .. h1
typedef uint64_t Board;

typedef enum {
    H1, G1, F1, E1, D1, C1, B1, A1, 
    H2, G2, F2, E2, D2, C2, B2, A2, 
    H3, G3, F3, E3, D3, C3, B3, A3, 
    H4, G4, F4, E4, D4, C4, B4, A4, 
    H5, G5, F5, E5, D5, C5, B5, A5, 
    H6, G6, F6, E6, D6, C6, B6, A6, 
    H7, G7, F7, E7, D7, C7, B7, A7, 
    H8, G8, F8, E8, D8, C8, B8, A8, 
} Square;

typedef enum {
    NORTH = A2-A1,
    EAST = B1-A1,
    SOUTH = -NORTH,
    WEST = -EAST,

    NORTH_WEST = NORTH + WEST,
    NORTH_EAST = NORTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    SOUTH_EAST = SOUTH + EAST,
} Direction;

static bool color_output = false;

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

void putsq(Board b, Square sq) {
    if (color_output) 
        printf(((b >> sq) & 1) 
                ? "\033[31m" "#" "\033[0m"
                : "\033[34m" "." "\033[0m");
    else 
        putchar(((b >> sq) & 1) ? '#' : '.');
}

void print_rank(uint8_t rank) {
    for (int i = 7; i >= 0; i--) {
        putchar(((rank >> i) & 1) ? '1' : '0');
        putchar(' ');
    }
}

void print_board(Board board) {
    printf("\n  a b c d e f g h\n");
    for (int i = 7; i >= 0; i--) {
        printf("%d ", i + 1);
        for (int j = 7; j >= 0; j--) {
            putsq(board, i*8 + j);
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

// Includes the square sq on which the queen currently stands
Board queen_attacks(Square sq) {
    Board result = 0;

    result |= rank_mask(sq);
    result |= file_mask(sq);
    result |= diag_mask(sq);

    return result;
}

#define SOLS_CAP 256
Board sols[SOLS_CAP] = {0};
size_t sols_count = 0;

void register_solution(Board sol) {
    // ensure no duplicate solutions
    for (int i = 0; i < sols_count; i++) {
        if (sols[i] == sol) return;
    }

    assert(sols_count < SOLS_CAP);
    sols[sols_count++] = sol;
}

void find_solutions(Board available, Board queens, int level) {
    if (level == 8) {
        register_solution(queens);
        return;
    }

    if (!available) return;

    for (int i = 0; i < 64; i++) {
        if (!(available & sq_mask(i))) continue;

        find_solutions(available & ~queen_attacks(i), queens | sq_mask(i), level + 1);
    }
}

int main(int argc, char **argv) {
    Board available = ~0llu;
    Board queens = 0;

    clock_t then = clock();
    assert(then != (clock_t) -1);
    find_solutions(available, queens, 0);
    clock_t now = clock();
    assert(then != (clock_t) -1);
    double secs = (double)(now - then)/CLOCKS_PER_SEC;

    printf("Took %lf secs.\n", secs);
    return 0;
}
