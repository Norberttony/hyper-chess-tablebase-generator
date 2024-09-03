#ifndef UNMOVE_HEADER
#define UNMOVE_HEADER

#include <stdint.h>

#include "magic-bitboards.h"
#include "defines.h"
#include "move.h"
#include "godel.h"

// least significant 3 bits are piece type
// next 6 bits are from square
// next 6 bits are to square
typedef uint16_t Unmove;


extern const int unmove_typeMask;
extern const int unmove_fromMask;
extern const int unmove_toMask;


int generateUnmoves(Unmove*);
int extractMoves(Unmove, U64, Unmove*);

void makeUnmove(Unmove);
void unmakeUnmove(Unmove);

void printUnmove(Unmove);

#endif