#ifndef GODEL_H
#define GODEL_H

#include "stdint.h"
#include "defines.h"
#include "move.h"
#include "transform.h"

typedef uint32_t Godel;

// gives the smallest number that represents the white king's current square, all symmetries removed
// index should be white king's square after the necessary transformations.
extern const int wKingLookup[28];

// returns the godel number, first is white king's square and second is black king's square
extern Godel kingsGodelLookup[10][64];

// gives 2 squares of the white king and black king respectively, given a Godel number representing
// just the two kings
extern int kingSquareLookup[528][2];


// populates the kingsGodelLookup and kingSquareLookup arrays
void populateGodelLookups();


#endif