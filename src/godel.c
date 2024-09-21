
#include "godel.h"

const int wKingLookup[28] =
{
     0,  1,  2,  3, -1, -1, -1, -1,
    -1,  4,  5,  6, -1, -1, -1, -1,
    -1, -1,  7,  8, -1, -1, -1, -1,
    -1, -1, -1,  9
};

Godel kingsGodelLookup[10][64];

int kingSquareLookup[TWO_KING_POSS][2];

// quantity of pieces per piece type
int whitePieces[7] =
{
    0, // empty, should always be 0
    0, // straddlers
    0, // retractor
    0, // springer
    0, // coordinator
    0, // immobilizer
    1  // chameleon
};
int blackPieces[7] =
{
    0, // empty, should always be 0
    1, // straddlers
    0, // retractor
    0, // springer
    0, // coordinator
    0, // immobilizer
    0  // chameleon
};

// look up table for 64 choose idx
const Godel _64Cr[] =
{
    1ULL,
    64ULL,
    2016ULL,
    41664ULL,
    635376ULL,
    7624512ULL,
    74974368ULL,
    621216192ULL,
    4426165368ULL
};


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

void clearPosition(void)
{
    // clear piece lists
    U64 totalBoard = position[white] | position[black];
    while (totalBoard)
    {
        pieceList[pop_lsb(totalBoard)] = 0;
        totalBoard &= totalBoard - 1;
    }

    // clear position
    for (int i = 0; i < 17; i++)
    {
        position[i] = 0ULL;
    }
}

int loadGodelNumber(Godel godel)
{
    clearPosition();

    // extract king squares and remove them from the godel number
    int kingId = godel % TWO_KING_POSS;
    godel /= TWO_KING_POSS;

    int whiteKingSq = kingSquareLookup[kingId][0];
    int blackKingSq = kingSquareLookup[kingId][1];

    position[white + king] = 1ULL << whiteKingSq;
    position[black + king] = 1ULL << blackKingSq;

    pieceList[whiteKingSq] = king;
    pieceList[blackKingSq] = king;


    position[white] = position[white + king];
    position[black] = position[black + king];

    int* transform = transforms[whiteKingSq][blackKingSq];

    // extract the squares of the white pieces
    for (int i = 1; i < 7; i++)
    {
        int pieceSq = transform[godel & 63];

        // avoid placing two pieces on the same square
        if (whitePieces[i] > 0 && pieceList[pieceSq])
        {
            return 0;
        }

        U64 pieceBoard = (1ULL << pieceSq) * (whitePieces[i] > 0);
        
        position[white + i] |= pieceBoard;
        position[white] |= pieceBoard;

        pieceList[pieceSq] = i * (whitePieces[i] > 0);

        godel >>= 6 * (whitePieces[i] > 0);
    }

    // extract the squares of the black pieces
    for (int i = 1; i < 7; i++)
    {
        int pieceSq = transform[godel & 63];

        // avoid placing two pieces on the same square
        if (blackPieces[i] > 0 && pieceList[pieceSq])
        {
            return 0;
        }

        U64 pieceBoard = (1ULL << pieceSq) * (blackPieces[i] > 0);
        
        position[black + i] |= pieceBoard;
        position[black] |= pieceBoard;

        pieceList[pieceSq] = i * (blackPieces[i] > 0);

        godel >>= 6 * (blackPieces[i] > 0);
    }

    // return 1 if no pieces intersect and 0 if they do intersect
    return (position[white + king] & position[black + king]) == 0;
}

Godel getGodelNumber(void)
{
    int whiteKingSq = pop_lsb(position[white + king]);
    int blackKingSq = pop_lsb(position[black + king]);

    int* transform = transforms[whiteKingSq][blackKingSq];

    whiteKingSq = transform[whiteKingSq];
    blackKingSq = transform[blackKingSq];

    Godel g = kingsGodelLookup[wKingLookup[whiteKingSq]][blackKingSq];
    // assumes that there is at most one of each piece type
    // fetch white pieces
    Godel gw = 0;
    int o = 0;
    for (int i = 1; i < 7; i++)
    {
        int pieceSq = pop_lsb(position[white + i]);
        gw += transform[pieceSq] * (whitePieces[i] > 0) * (o + !o);
        o += _64Cr[whitePieces[i]] - (whitePieces[i] == 0);
    }

    // fetch black pieces
    Godel gb = 0;
    for (int i = 1; i < 7; i++)
    {
        int pieceSq = pop_lsb(position[black + i]);
        gb += transform[pieceSq] * (blackPieces[i] > 0) * (o + !o);
        o += _64Cr[blackPieces[i]] - (blackPieces[i] == 0);
    }

    return g + (gw + gb) * TWO_KING_POSS;
}

#define TRI_DIAG_1 (1ULL << a8 | 1ULL << b7 | 1ULL << c6 | 1ULL << d5 | 1ULL << e4 | 1ULL << f3 | 1ULL << g2 | 1ULL << h1)
#define TRI_DIAG_3 (1ULL << a1 | 1ULL << b2 | 1ULL << c3 | 1ULL << d4 | 1ULL << e5 | 1ULL << f6 | 1ULL << g7 | 1ULL << h8)

Godel getThirdPieceSymmetryGodel(Godel g)
{
    // if the kings aren't on the tri diag, then it cannot be a third piece symmetry
    U64 whiteKing = position[white + king];
    U64 blackKing = position[black + king];
    int isDiag = (TRI_DIAG_1 & whiteKing && TRI_DIAG_1 & blackKing) || (TRI_DIAG_3 & whiteKing && TRI_DIAG_3 & blackKing);
    if (!isDiag)
    {
        return 0;
    }

    // the king's godel numbers would not change, but the other piece's squares might.
    Godel kings = g % TWO_KING_POSS;
    Godel symmG = 0;
    g /= TWO_KING_POSS;

    // check white pieces
    for (int i = 1; i < 7; i++)
    {
        int pieceExists = whitePieces[i] > 0;
        symmG <<= pieceExists * 6;
        symmG += pieceExists * reflectA8H1[g & 63];
        g >>= pieceExists * 6;
    }

    // check black pieces
    for (int i = 1; i < 7; i++)
    {
        int pieceExists = blackPieces[i] > 0;
        symmG <<= pieceExists * 6;
        symmG += pieceExists * reflectA8H1[g & 63];
        g >>= pieceExists * 6;
    }

    return kings + symmG * TWO_KING_POSS;
}
