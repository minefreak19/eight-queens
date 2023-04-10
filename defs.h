#ifndef DEFS_H_
#define DEFS_H_

#include <assert.h>
#include <stdint.h>

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

#endif // DEFS_H_
