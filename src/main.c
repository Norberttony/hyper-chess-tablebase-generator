
#include "transform.h"
#include "godel.h"

#include <stdio.h>

int main(void)
{
    populateTransforms();
    populateGodelLookups();

    int godel = 527;
    int wSq = kingSquareLookup[godel][0];
    int bSq = kingSquareLookup[godel][1];

    // update position bitboards
    position[white] = 1ULL << wSq;
    position[white + king] = 1ULL << wSq;

    position[black] = 1ULL << bSq;
    position[black + king] = 1ULL << bSq;

    // update piecelists
    pieceList[wSq] = king;
    pieceList[bSq] = king;

    // print the board
    prettyPrintBoard();

    return 0;
}
