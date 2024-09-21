#ifndef GODEL_H
#define GODEL_H

#include "stdint.h"
#include "defines.h"
#include "move.h"
#include "transform.h"

typedef unsigned long long Godel;

// number of possible positions achievable with two kings
#define TWO_KING_POSS 528


// gives the smallest number that represents the white king's current square, all symmetries removed
// index should be white king's square after the necessary transformations.
extern const int wKingLookup[28];

// returns the godel number, first is white king's square and second is black king's square
extern Godel kingsGodelLookup[10][64];

// gives 2 squares of the white king and black king respectively, given a Godel number representing
// just the two kings
extern int kingSquareLookup[TWO_KING_POSS][2];

// configuration of white and black pieces
extern int whitePieces[7];
extern int blackPieces[7];

extern const Godel _64Cr[];


// populates the kingsGodelLookup and kingSquareLookup arrays
void populateGodelLookups();

// loads the given godel number into the position
int loadGodelNumber(Godel);

// returns the Godel number of the current position
Godel getGodelNumber(void);

void clearPosition(void);

// checks for a symmetrical position that would be represented by another (unnecessary) godel
// number.
Godel getThirdPieceSymmetryGodel(Godel);

#endif