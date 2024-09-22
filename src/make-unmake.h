#ifndef MAKE_UNMAKE_HEADER
#define MAKE_UNMAKE_HEADER

#include "move.h"
#include "godel.h"

// forward declaration for white and black piece configurations (in case a piece gets captured
// and the arrays have to be updated)
extern int whitePieces[7];
extern int blackPieces[7];


void makeMove(Move);
void unmakeMove(Move);

// used to pass (or "unpass") a turn
void makeNullMove();

#endif