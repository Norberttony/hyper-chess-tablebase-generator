#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "defines.h"
#include "move.h"

// given the position of the two kings, returns the necessary transformations the board has to take
// to eliminate having to re-solve positions where it is just the reflection of a previously solved
// position.
extern int* transforms[64][64];

extern const int isUnderDiagonal[64];

// populates the transforms array
void populateTransforms();

// prints out a combination of the two transforms
void combineTransforms(int* tran1, int* tran2);

#endif