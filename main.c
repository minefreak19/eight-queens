#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "./defs.h"

static const char *program_name = NULL;

// Lookup table of queen moves
static Board qmtable[64];

static bool color_output = false;

void putsq(FILE *f, Board b, Square sq) {
    if (color_output) 
        fprintf(f, ((b >> sq) & 1) 
                ? "\033[31m" "#" "\033[0m"
                : "\033[34m" "." "\033[0m");
    else 
        fputc(((b >> sq) & 1) ? '#' : '.', f);
}

void print_board(FILE *f, Board board) {
    fprintf(f, "\n  a b c d e f g h\n");
    for (int i = 7; i >= 0; i--) {
        fprintf(f, "%d ", i + 1);
        for (int j = 7; j >= 0; j--) {
            putsq(f, board, i*8 + j);
            fputc(' ', f);
        }
        fputc('\n', f);
    }
    fputc('\n', f);
}

// Includes the square sq on which the queen currently stands
Board queen_attacks(Square sq) {
    return qmtable[sq];
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
    
    Board mask = 0x1llu;
    for (int i = 0; i < 64; i++, mask <<= 1) {
        if (!(available & mask)) continue;

        find_solutions(available & ~queen_attacks(i), queens | mask, level + 1);
    }
}

void usage(FILE *f) {
    fprintf(f, "USAGE: %s <queenmoves> <outfile>\n", program_name);
}

void init_qmtable(const char *table_path) {
    FILE *file = fopen(table_path, "rb");
    if (file == NULL) {
        usage(stderr);
        fprintf(stderr, "ERROR: Could not open file %s: %s\n", table_path,
                strerror(errno));
        exit(1);
    }

    int n_read = fread(qmtable, sizeof(Board), 64, file);
    assert(n_read == 64 && "Input file should have exactly 64 bitboards.");

    fclose(file);
}

void write_sols_to_file(const char *path) {
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        usage(stderr);
        fprintf(stderr, "ERROR: Could not open file %s: %s\n", path,
                strerror(errno));
        exit(1);
    }

    for (int i = 0; i < sols_count; i++) {
        print_board(f, sols[i]);
    }

    fclose(f);
}

int main(int argc, char **argv) {
    assert(argc >= 1);
    program_name = argv[0];
    if (argc < 3) {
        usage(stderr);
        fprintf(stderr, "ERROR: Too few arguments.\n");
        exit(1);
    }

    const char *qmtable_path = argv[1];
    const char *out_path = argv[2];
    
    Board available = ~0llu;
    Board queens = 0;

    clock_t then = clock();
    assert(then != (clock_t) -1);
    {
        init_qmtable(qmtable_path);
        find_solutions(available, queens, 0);
    }
    clock_t now = clock();
    assert(now != (clock_t) -1);
    double secs = (double)(now - then)/CLOCKS_PER_SEC;

    // there are exactly 92 solutions to the eight-queens problem
    // https://en.wikipedia.org/wiki/Eight_queens_puzzle#:~:text=There%20are%2092%20solutions.
    assert(sols_count == 92);
    write_sols_to_file(out_path);
    printf("Found %zu solutions in %lf secs.\n", sols_count, secs);
    return 0;
}
