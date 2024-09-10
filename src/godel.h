#ifndef GODEL_H
#define GODEL_H

#include "stdint.h"
#include "defines.h"
#include "move.h"
#include "transform.h"

typedef uint32_t Godel;

// number of possible positions achievable with two kings
#define TWO_KING_POSS 528

#define WHITE_PIECES 1
#define BLACK_PIECES 1


// gives the smallest number that represents the white king's current square, all symmetries removed
// index should be white king's square after the necessary transformations.
extern const int wKingLookup[28];

// returns the godel number, first is white king's square and second is black king's square
extern Godel kingsGodelLookup[10][64];

// gives 2 squares of the white king and black king respectively, given a Godel number representing
// just the two kings
extern int kingSquareLookup[TWO_KING_POSS][2];

// populates the kingsGodelLookup and kingSquareLookup arrays
void populateGodelLookups();

// loads the given godel number into the position
int loadGodelNumber(Godel);

// returns the Godel number of the current position
Godel getGodelNumber(void);

// returns the previous (endgame reference) Godel number of the current position.
Godel getRefGodelNumber(void);

void clearPosition(void);

#endif