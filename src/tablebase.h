#ifndef TABLEBASE_HEADER
#define TABLEBASE_HEADER

#include "defines.h"
#include "unmove.h"
#include "move.h"
#include "perft.h"


#define set_bit32_arr(arr, i) (arr[i >> 5] |= 1 << (i & 31))
#define clr_bit32_arr(arr, i) (arr[i >> 5] &= ~(1 << (i & 31)))
#define get_bit32_arr(arr, i) (arr[i >> 5] & (1 << (i & 31)))


extern Godel possibilities;
extern int fileSize;

extern uint32_t *whiteWins;
extern uint32_t *blackLoses;
extern uint32_t *blackTemp;

extern uint32_t *endgameRef;


int allocTablebase();
void initTablebase();
int tablebaseStep(int depth);

void createDepthToMateFile(int maxDepth);

#endif