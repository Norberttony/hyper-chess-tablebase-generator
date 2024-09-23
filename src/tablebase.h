#ifndef TABLEBASE_HEADER
#define TABLEBASE_HEADER

#include "defines.h"
#include "unmove.h"
#include "move.h"
#include "perft.h"
#include "vector.h"


#define set_bit32_arr(arr, i) (arr[i >> 5] |= 1 << (i & 31))
#define clr_bit32_arr(arr, i) (arr[i >> 5] &= ~(1 << (i & 31)))
#define get_bit32_arr(arr, i) (arr[i >> 5] & (1 << (i & 31)))

#define PIECE_COMP_POSS (9 * 9) * (3 * 3) * (3 * 3) * (2 * 2) * (2 * 2) * (2 * 2)

extern Godel possibilities;
extern int fileSize;

extern uint32_t *whiteWins;
extern uint32_t *blackLoses;
extern uint32_t *blackTemp;

extern uint32_t *endgameRefW[PIECE_COMP_POSS];
extern uint32_t *endgameRefB[PIECE_COMP_POSS];


int allocTablebase();
void initTablebase();
int tablebaseStep(int depth);

void createDTZFile(int maxDepth);

Godel getNumPossibilities(void);

// returns the endgame reference for the current piece composition.
// if isWhite, returns white to play and win. otherwise, returns black to play and lose.
uint32_t* getEndgameRef(int isWhite);

void freeEndgameRefCache(int isWhite);

#endif