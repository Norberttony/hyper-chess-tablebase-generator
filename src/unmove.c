
#include "unmove.h"

const int unmove_typeMask   = 0b000000000000111;
const int unmove_fromMask   = 0b000000111111000;
const int unmove_toMask     = 0b111111000000000;

// some important rules to consider for unmove generation:
// - if an enemy piece exists on the death square of the unmoving side, then the unmoving side
//  cannot unmove neither the coordinator nor the king (as that would imply that the piece would
//  have been captured on the next move).
// - same thing for straddler death squares, neither straddler may perform a valid unmove.
// - cannot make an unmove that leaves the piece in an immobilizer's influence
// - cannot make a retractor one-move towards an enemy piece
// - same stuff applies for chameleons for all four previously mentioned instances.

int generateUnmoves(Unmove* list)
{
    int size = 0;

    // if it's white-to-play, we're undoing black's move

    U64 opponent = position[toPlay];
    U64 totalBoard = opponent | position[notToPlay];

    int oppImmSq = pop_lsb(position[toPlay + immobilizer]);
    U64 oppImmInfl = (position[toPlay + immobilizer] > 0) * kingMoves[oppImmSq];

    int myImmSq = pop_lsb(position[notToPlay + immobilizer]);
    U64 myImmInfl = (position[notToPlay + immobilizer] > 0) * kingMoves[myImmSq];

    U64 oppKing = position[toPlay + king] & ~myImmInfl;
    int oppKingSq = pop_lsb(oppKing);

    // undo king's move
    U64 kingBoard = position[notToPlay + king];
    int kingSq = pop_lsb(kingBoard);

    U64 coordBoard = position[notToPlay + coordinator];
    int coordSq = pop_lsb(coordBoard);

    U64 chamBoard = position[notToPlay + chameleon];
    U64 cham1Board = 1ULL << pop_lsb(chamBoard);
    int cham1Sq = pop_lsb(cham1Board);
    U64 cham2Board = chamBoard & (chamBoard - 1);
    int cham2Sq = pop_lsb(cham2Board);

    U64 kingCoordDS = (coordBoard > 0) * (deathSquares[kingSq][coordSq][0] | deathSquares[kingSq][coordSq][1]);
    U64 kingChamDS1 = (cham1Board > 0) * (deathSquares[kingSq][cham1Sq][0] | deathSquares[kingSq][cham1Sq][1]);
    U64 kingChamDS2 = (cham2Board > 0) * (deathSquares[kingSq][cham2Sq][0] | deathSquares[kingSq][cham2Sq][1]);

    int allowKingCoordMoves = coordBoard == 0 || !(kingCoordDS & position[toPlay]);

    int allowKingChamMoves = chamBoard == 0 || !(kingChamDS1 & position[toPlay + coordinator]);


    if (allowKingCoordMoves && allowKingChamMoves)
    {
        U64 kMoves = kingMoves[kingSq] & ~(totalBoard | (kingMoves[oppKingSq] * (oppKing > 0)) | oppImmInfl);
        size += extractMoves((kingSq << 3) | king, kMoves, &list[size]);
    }
    U64 straddlers = position[notToPlay + straddler];

    // undoing a straddler move and then playing it again can cause different results.
    U64 frozenStraddlers =
        straddlers & (opponent << 8) & (straddlers << 16) |
        straddlers & (opponent << 1) & (straddlers << 2) |
        straddlers & (opponent >> 1) & (straddlers >> 2) |
        straddlers & (opponent >> 8) & (straddlers >> 16);
    
    U64 activeStraddlers = straddlers & ~frozenStraddlers;
    while (activeStraddlers)
    {
        int fromSq = pop_lsb(activeStraddlers);
        U64 rookMoves = get_rook_attacks(fromSq, totalBoard) & ~(totalBoard | oppImmInfl);

        size += extractMoves(fromSq << 3 | straddler, rookMoves, &list[size]);

        activeStraddlers &= activeStraddlers - 1;
    }

    U64 immBoard = position[notToPlay + immobilizer];
    if (immBoard)
    {
        int fromSq = pop_lsb(immBoard);
        U64 moves = (get_rook_attacks(fromSq, totalBoard) | get_bishop_attacks(fromSq, totalBoard)) & ~(totalBoard | oppImmInfl);

        size += extractMoves(fromSq << 3 | immobilizer, moves, &list[size]);
    }

    // to-do: do not run with > 1 chameleon and a coordinator on the board, will give illegal moves.
    while (allowKingChamMoves && chamBoard)
    {
        int fromSq = pop_lsb(chamBoard);
        U64 moves = (get_rook_attacks(fromSq, totalBoard) | get_bishop_attacks(fromSq, totalBoard)) & ~(totalBoard | oppImmInfl);

        size += extractMoves(fromSq << 3 | chameleon, moves, &list[size]);
        chamBoard &= chamBoard - 1;
    }

    U64 springerBoard = position[notToPlay + springer];
    while (springerBoard)
    {
        int fromSq = pop_lsb(springerBoard);
        U64 moves = (get_rook_attacks(fromSq, totalBoard) | get_bishop_attacks(fromSq, totalBoard)) & ~(totalBoard | oppImmInfl);

        size += extractMoves(fromSq << 3 | springer, moves, &list[size]);
        springerBoard &= springerBoard - 1;
    }

    if (coordBoard && allowKingCoordMoves)
    {
        int fromSq = pop_lsb(coordBoard);
        U64 moves = (get_rook_attacks(fromSq, totalBoard) | get_bishop_attacks(fromSq, totalBoard)) & ~(totalBoard | oppImmInfl);

        size += extractMoves(fromSq << 3 | coordinator, moves, &list[size]);
    }

    U64 retractorBoard = position[notToPlay + retractor];
    if (retractorBoard)
    {
        int fromSq = pop_lsb(retractorBoard);
        U64 moves = (get_rook_attacks(fromSq, totalBoard) | get_bishop_attacks(fromSq, totalBoard)) & ~(totalBoard | oppImmInfl);

        size += extractMoves(fromSq << 3 | retractor, moves, &list[size]);
    }

    return size;
}

int extractMoves(Unmove fromType, U64 board, Unmove* list)
{
    int size = 0;

    while (board)
    {
        list[size++] = fromType | pop_lsb(board) << 9;
        board &= board - 1;
    }

    return size;
}

void makeUnmove(Unmove unmove)
{
    int type = unmove & unmove_typeMask;
    int from = (unmove & unmove_fromMask) >> 3;
    int to = (unmove & unmove_toMask) >> 9;

    // move piece
    U64 toggle = (1ULL << from) | (1ULL << to);
    position[notToPlay + type] ^= toggle;
    position[notToPlay] ^= toggle;

    // update piece list
    pieceList[to] = pieceList[from];
    pieceList[from] = 0;

    // toggle turn
    toPlay = !toPlay * 8;
    notToPlay = !notToPlay * 8;
}

void unmakeUnmove(Unmove unmove)
{
    int type = unmove & unmove_typeMask;
    int from = (unmove & unmove_fromMask) >> 3;
    int to = (unmove & unmove_toMask) >> 9;

    // move piece
    U64 toggle = (1ULL << from) | (1ULL << to);
    position[toPlay + type] ^= toggle;
    position[toPlay] ^= toggle;

    // update piece list
    pieceList[from] = pieceList[to];
    pieceList[to] = 0;

    // toggle turn
    toPlay = !toPlay * 8;
    notToPlay = !notToPlay * 8;
}

void printUnmove(Unmove unmove)
{
    int type = unmove & unmove_typeMask;
    int from = (unmove & unmove_fromMask) >> 3;
    int to = (unmove & unmove_toMask) >> 9;

    printf("%s%s of type %c\n", squareNames[from], squareNames[to], pieceFEN[type]);
}
