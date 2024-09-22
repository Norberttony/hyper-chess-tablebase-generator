#ifndef PERMUTATIONS
#define PERMUTATIONS

#include "bitboard-utility.h"

#include <string.h>

// forward declaration of 64 choose r look up table
typedef unsigned long long Godel;
extern const Godel _64Cr[];


#define MAX_PERMS 2


U64 perms1[64];
U64 perms2[2016];


extern U64 *perms[MAX_PERMS + 1];


void initPerms(void);
int getPerm2Index(U64 board);
U64 nextPerm(U64 current);

#endif