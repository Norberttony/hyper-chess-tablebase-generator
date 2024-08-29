
#include "godel.h"

const int wKingLookup[28] =
{
     0,  1,  2,  3, -1, -1, -1, -1,
    -1,  4,  5,  6, -1, -1, -1, -1,
    -1, -1,  7,  8, -1, -1, -1, -1,
    -1, -1, -1,  9
};

Godel kingsGodelLookup[10][64];

int kingSquareLookup[528][2];


void populateGodelLookups()
{
    const int tri[] = { b8, c8, c7, d8, d7, d6 };

    // in this case, black's king can be on any of the 64 squares.
    for (int i = 0; i < 6; i++)
    {
        int wGo = wKingLookup[tri[i]];
        for (int j = 0; j < 64; j++)
        {
            int godel = i * 64 + j;
            kingsGodelLookup[wGo][j] = godel;

            // set squares based on godel number
            kingSquareLookup[godel][0] = tri[i];
            kingSquareLookup[godel][1] = j;
        }
    }

    // Godel number offset from previous numbers
    int currGodel = 6 * 64;

    // for the diagonals, there are 28 possibilities that cannot occur (black king under the a8 - h1
    // diagonal)
    const int diag[] = { a8, b7, c6, d5 };

    for (int i = 0; i < 4; i++)
    {
        int wGo = wKingLookup[diag[i]];
        for (int j = 0; j < 64; j++)
        {
            if (!isUnderDiagonal[j])
            {
                kingsGodelLookup[wGo][j] = currGodel;

                // set squares based on godel number
                kingSquareLookup[currGodel][0] = diag[i];
                kingSquareLookup[currGodel][1] = j;

                currGodel++;
            }
        }
    }

}
