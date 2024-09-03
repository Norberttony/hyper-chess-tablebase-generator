
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

#define WHITE_PIECES 1
//#define BLACK_PIECES 0

const int whitePieces[WHITE_PIECES] = { straddler };

#ifdef BLACK_PIECES
const int blackPieces[BLACK_PIECES] = { };
#endif

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

    U64 whiteBoard = 0ULL;
    U64 blackBoard = 0ULL;

    // extract king squares and remove them from the godel number
    int kingId = godel % TWO_KING_POSS;
    godel /= TWO_KING_POSS;

    int whiteKingSq = kingSquareLookup[kingId][0];
    int blackKingSq = kingSquareLookup[kingId][1];

    position[white + king] = 1ULL << whiteKingSq;
    whiteBoard = position[white + king];
    position[black + king] = 1ULL << blackKingSq;
    blackBoard = position[black + king];

    pieceList[whiteKingSq] = king;
    pieceList[blackKingSq] = king;

    // extract the squares of the white pieces
    int* transform = transforms[whiteKingSq][blackKingSq];
    int immSq = transform[godel];
    position[white + whitePieces[0]] = 1ULL << immSq;
    whiteBoard |= 1ULL << immSq;
    pieceList[immSq] = whitePieces[0];

    // update white and black boards
    position[white] = whiteBoard;
    position[black] = blackBoard;

    // return 1 if no pieces intersect and 0 if they do intersect
    return position[white + king] != position[black + king] && position[white + king] != position[white + whitePieces[0]] && position[black + king] != position[white + whitePieces[0]];
}

Godel getGodelNumber(void)
{
    int whiteKingSq = pop_lsb(position[white + king]);
    int blackKingSq = pop_lsb(position[black + king]);

    int* transform = transforms[whiteKingSq][blackKingSq];

    whiteKingSq = transform[whiteKingSq];
    blackKingSq = transform[blackKingSq];

    int pieceSq = pop_lsb(position[white + whitePieces[0]]);

    return kingsGodelLookup[wKingLookup[whiteKingSq]][blackKingSq] + transform[pieceSq] * TWO_KING_POSS;
}

Godel getRefGodelNumber(void)
{
    int whiteKingSq = pop_lsb(position[white + king]);
    int blackKingSq = pop_lsb(position[black + king]);

    int* transform = transforms[whiteKingSq][blackKingSq];

    whiteKingSq = transform[whiteKingSq];
    blackKingSq = transform[blackKingSq];

    int pieceSq = pop_lsb(position[white + whitePieces[0]]);

    return kingsGodelLookup[wKingLookup[whiteKingSq]][blackKingSq] + transform[pieceSq] * TWO_KING_POSS;

}
