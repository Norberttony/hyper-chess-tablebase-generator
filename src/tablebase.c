
#include "tablebase.h"

int fileSize = 0;

Godel possibilities;

uint32_t *whiteWins;
uint32_t *blackLoses;
uint32_t *blackTemp;

struct vector newWhiteWins;
struct vector newBlackTemp;
struct vector newBlackLoses;


const int pieceTypeMasks[] =
{
    0b0000000,
    0b0000001,
    0b0000010,
    0b0000100,
    0b0001000,
    0b0010000,
    0b0100000,
    0b1000000
};


// allocates memory for the tablebase
int allocTablebase()
{
    possibilities = getNumPossibilities();
    fileSize = (possibilities >> 5) + 1;

    printf("There are %llu possible positions.\n", possibilities);

    // arrays that will be indexed by the Godel number and return 1 if the position belongs in
    // the set of positions
    whiteWins  = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);
    blackLoses = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);
    blackTemp  = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);

    newWhiteWins = v_init(64);
    newBlackTemp = v_init(64);
    newBlackLoses = v_init(64);

    if (!whiteWins || !blackLoses || !blackTemp)
    {
        puts("Not enough memory to allocate tablebase. Check the number of pieces and possible positions.");
        return -1;
    }

   return 1;
}

// P1 for initialization. Creates B0 (BTP and lose). P1 loops through all G positions, and
// checks for mate. If mate, corresponding bit in B0 is set. B is initialized to B0. W is
// initialized to all zeroes and i (depth) is set to 0.
void initTablebase()
{
    puts("Initializing...");
    // initialize all to zero
    for (int i = 0; i < fileSize; i++)
    {
        whiteWins[i]  = 0;
        blackLoses[i] = 0;
        blackTemp[i]  = 0;
    }

    // Creates B file that determines mating positions
    toPlay = black;
    notToPlay = white;
    for (Godel i = 0; i < possibilities; i++)
    {
        int success = loadGodelNumber(i);

        toPlay = black;
        notToPlay = white;
        if (!success || isAttackingKing())
        {
            continue;
        }
        
        if (isCheckmate())
        {
            set_bit32_arr(blackLoses, i);
            v_pushBack(&newBlackLoses, (void*)i);

            Godel s = getThirdPieceSymmetryGodel(i);

            if (s)
            {
                set_bit32_arr(blackLoses, s);
                v_pushBack(&newBlackLoses, (void*)s);
            }
        }

        // check if there are any captures. if there are, reference previous DB files...
        toPlay = white;
        notToPlay = black;
        if (isAttackingKing())
        {
            continue;
        }

        toPlay = white;
        notToPlay = black;
        Move captures[MAX_CAPTURES];
        int size = generateMoves((Move*)captures, 1);
        for (int j = 0; j < size; j++)
        {
            Move c = captures[j];
            if (!isMoveLegal(c))
            {
                continue;
            }

            makeMove(c);

            uint32_t* ref = getEndgameRef(0);
            if (!ref)
            {
                puts("UNSUPPORTED ENDGAME REF");
                exit(0);
            }

            Godel g = getGodelNumber();
            if (get_bit32_arr(ref, g))
            {
                set_bit32_arr(whiteWins, i);
                v_pushBack(&newBlackLoses, (void*)i);
                unmakeMove(c);
                break;
            }

            unmakeMove(c);
        }
    }

    puts("Writing to file...");

    // write the results to a file
    FILE *fileB = fopen("B0.bin", "wb");
    fwrite(blackLoses, sizeof(uint32_t), fileSize, fileB);
    fclose(fileB);

    // free endgame ref cache for black's endgames.
    freeEndgameRefCache(0);

    puts("Initialization complete!");
}

int tablebaseStep(int depth)
{
    puts("Determining white wins from black loses positions");

    char fileName[1000];

    int newPosition = 0;

    toPlay = black;
    notToPlay = white;
    while(newBlackLoses.size)
    {
        Godel i = (Godel)v_popBack(&newBlackLoses);
        loadGodelNumber(i);

        // white has a mate in all predecessor positions
        toPlay = black;
        notToPlay = white;

        Unmove unmoves[MAX_MOVES];
        int size = generateUnmoves((Unmove*)unmoves);
        for (int j = 0; j < size; j++)
        {
            // don't unmake a move that leaves opponent king in check.
            toPlay = black;
            notToPlay = white;
            
            Unmove unmove = unmoves[j];
            makeUnmove(unmove);

            toPlay = white;
            notToPlay = black;
            if (isAttackingKing())
            {
                unmakeUnmove(unmove);
                continue;
            }

            // check if a new position was added
            Godel changed = getGodelNumber();

            if (!get_bit32_arr(whiteWins, changed))
            {
                newPosition = 1;
                v_pushBack(&newWhiteWins, (void*)changed);

                // mark this position as winnable!
                set_bit32_arr(whiteWins, changed);

                Godel s = getThirdPieceSymmetryGodel(changed);
                if (s)
                {
                    set_bit32_arr(whiteWins, s);
                    v_pushBack(&newWhiteWins, (void*)s);
                }
            }

            unmakeUnmove(unmove);
        }
    }

    snprintf(fileName, 1000, "W%d.bin", depth);
    FILE* fileW = fopen(fileName, "wb");
    fwrite(whiteWins, sizeof(uint32_t), fileSize, fileW);
    fclose(fileW);

    // clear temp
    for (int i = 0; i < fileSize; i++)
    {
        blackTemp[i] = 0;
    }

    puts("Determining black loses if black was trying to lose.");

    // P3 reads whiteWins, generates black predecessor positions and stores them in temp. These
    // positions are black to play and lose if black was trying to lose.
    while (newWhiteWins.size)
    {
        Godel i = (Godel)v_popBack(&newWhiteWins);
        loadGodelNumber(i);

        toPlay = white;
        notToPlay = black;

        Unmove unmoves[MAX_MOVES];
        int size = generateUnmoves((Unmove*)unmoves);
        for (int j = 0; j < size; j++)
        {
            // don't unmake a move that leaves opponent king in check.
            toPlay = white;
            notToPlay = black;
            
            Unmove unmove = unmoves[j];
            makeUnmove(unmove);

            toPlay = black;
            notToPlay = white;
            if (isAttackingKing())
            {
                unmakeUnmove(unmove);
                continue;
            }

            // mark this position as losable
            Godel g = getGodelNumber();
            set_bit32_arr(blackTemp, g);
            v_pushBack(&newBlackTemp, (void*)g);

            Godel s = getThirdPieceSymmetryGodel(g);
            if (s)
            {
                set_bit32_arr(blackTemp, s);
                v_pushBack(&newBlackTemp, (void*)s);
            }

            unmakeUnmove(unmove);
        }
    }

    puts("Determining which of these black to play and try to lose are actually losses.");

    // P4 goes through all temp positions, generates successor positions, and ensures that all
    // successor positions are in whiteWins. If so, that position is added to blackLoses
    toPlay = black;
    notToPlay = white;
    while (newBlackTemp.size)
    {
        Godel i = (Godel)v_popBack(&newBlackTemp);
        loadGodelNumber(i);
        Move moves[MAX_MOVES];
        int size = generateMoves((Move*)moves, 0);

        int alwaysWinning = 1;
        for (int j = 0; j < size; j++)
        {
            Move move = moves[j];
            makeMove(move);

            if (isAttackingKing())
            {
                unmakeMove(move);
                continue;
            }

            Godel changed = getGodelNumber();

            if (move & move_captMask)
            {
                // check the reference to see if this position is still winning for white.
                uint32_t* ref = getEndgameRef(1);
                if (!ref)
                {
                    prettyPrintBoard();
                    puts("MISSING ENDGAME FILE FOR CURRENT PIECE COMPOSITION");
                    prettyPrintMove(move);

                    unmakeMove(move);
                    puts("Previous position:");
                    prettyPrintBoard();

                    puts("Godel board:");
                    loadGodelNumber(i);
                    prettyPrintBoard();
                    printf("g: %llu\n", i);

                    exit(1);
                }

                if (!get_bit32_arr(ref, changed))
                {
                    alwaysWinning = 0;
                    unmakeMove(move);
                    break;
                }
            }
            else if (!get_bit32_arr(whiteWins, changed))
            {
                alwaysWinning = 0;
                unmakeMove(move);
                break;
            }

            unmakeMove(move);
        }

        if (alwaysWinning && (size > 0 || isAttackingKing()) && !get_bit32_arr(blackLoses, i))
        {
            newPosition = 1;
            set_bit32_arr(blackLoses, i);
            v_pushBack(&newBlackLoses, (void*)i);

            Godel s = getThirdPieceSymmetryGodel(i);
            if (s)
            {
                set_bit32_arr(blackLoses, s);
                v_pushBack(&newBlackLoses, (void*)s);
            }
        }
    }

    snprintf(fileName, 1000, "B%d.bin", depth);
    FILE* fileB = fopen(fileName, "wb");
    fwrite(blackLoses, sizeof(uint32_t), fileSize, fileB);
    fclose(fileB);

    puts("Depth complete.");

    // this function should no longer be called if there is no new position.
    // therefore, it seems good to clear all previous endgame caches.
    if (!newPosition)
    {
        freeEndgameRefCache(1);
    }

    return newPosition;
}

void createDTZFile(int maxDepth)
{
    char fileName[1000];

    // depth to mate in ply
    unsigned short int* depthToZeroW = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    unsigned short int* depthToZeroL = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    if (!depthToZeroW || !depthToZeroL)
    {
        puts("Could not allocate depth to mate array");
        return;
    }
    for (int i = 0; i < possibilities; i++)
    {
        depthToZeroW[i] = 0;
        depthToZeroL[i] = 65535;
    }


    uint32_t* wins = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);
    uint32_t* losses = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);
    if (!wins || !losses)
    {
        puts("Could not allocate wins/losses array");
        return;
    }

    FILE* fileB = fopen("B0.bin", "rb");
    fread(losses, sizeof(uint32_t), fileSize, fileB);
    fclose(fileB);
    for (Godel g = 0; g < possibilities; g++)
    {
        if (!loadGodelNumber(g))
        {
            continue;
        }
        if (depthToZeroL[g] == 65535 && get_bit32_arr(losses, g))
        {
            // first time encountering a black loss here
            depthToZeroL[g] = 0;
        }
    }

    for (int d = 1; d <= maxDepth; d++)
    {
        snprintf(fileName, 1000, "W%d.bin", d);
        FILE* fileW = fopen(fileName, "rb");
        fread(wins, sizeof(uint32_t), fileSize, fileW);
        fclose(fileW);

        snprintf(fileName, 1000, "B%d.bin", d);
        FILE* fileB = fopen(fileName, "rb");
        fread(losses, sizeof(uint32_t), fileSize, fileB);
        fclose(fileB);

        printf("Crunching depth %d...\n", d);

        for (Godel g = 0; g < possibilities; g++)
        {
            if (!loadGodelNumber(g))
            {
                continue;
            }
            if (depthToZeroW[g] == 0 && get_bit32_arr(wins, g))
            {
                // first time encountering a white win here
                depthToZeroW[g] = d * 2 - 1;
            }
            if (depthToZeroL[g] == 65535 && get_bit32_arr(losses, g))
            {
                // first time encountering a black loss here
                depthToZeroL[g] = d * 2;
            }
        }
    }

    free(wins);
    free(losses);

    // store depth to mate in a special file
    FILE* fileDTZW = fopen("DTZW.bin", "wb");
    fwrite(depthToZeroW, sizeof(unsigned short int), possibilities, fileDTZW);
    fclose(fileDTZW);

    FILE* fileDTZL = fopen("DTZL.bin", "wb");
    fwrite(depthToZeroL, sizeof(unsigned short int), possibilities, fileDTZL);
    fclose(fileDTZL);
}

int moveCapturePieceTypes(Move m)
{
    switch (m & move_typeMask)
    {
        case straddler:
            return
                pieceTypeMasks[(m & move_c1Mask) >> 15] +
                pieceTypeMasks[(m & move_c2Mask) >> 18] +
                pieceTypeMasks[(m & move_c3Mask) >> 21] +
                pieceTypeMasks[(m & move_c4Mask) >> 24];
        case retractor:
        case springer:
            return pieceTypeMasks[(m & move_c1Mask) >> 15];
        case coordinator:
            return
                pieceTypeMasks[(m & move_c1Mask) >> 15] +
                pieceTypeMasks[(m & move_c2Mask) >> 18];
        case immobilizer:
            return 0;
        case chameleon:
            return
                pieceTypeMasks[(m & move_cham_u_mask) > 0] +
                pieceTypeMasks[(m & move_cham_l_mask) > 0] +
                pieceTypeMasks[(m & move_cham_r_mask) > 0] +
                pieceTypeMasks[(m & move_cham_d_mask) > 0] +

                ((m & (move_cham_d1_mask | move_cham_d2_mask)) > 0) * pieceTypeMasks[coordinator] +
                ((m & move_cham_q_mask) > 0) * pieceTypeMasks[retractor] +
                ((m & move_cham_n_mask) > 0) * pieceTypeMasks[springer];
        case king:
            return
                pieceTypeMasks[(m & move_c1Mask) >> 15] +
                pieceTypeMasks[(m & move_c2Mask) >> 18] +
                pieceTypeMasks[(m & move_c3Mask) >> 21] +
                ((m & move_kingcmask) > 0) * pieceTypeMasks[coordinator];
    }
}

// 9p 3c 3n 2q 2u 2r
uint32_t *endgameRefW[PIECE_COMP_POSS] = { 0 };
uint32_t *endgameRefB[PIECE_COMP_POSS] = { 0 };

// returns the number of possible positions given that symmetries are removed.
Godel getNumPossibilities(void)
{
    Godel p = TWO_KING_POSS;
    for (int i = 1; i < 7; i++)
    {
        p *= _64Cr[whitePieces[i]] * _64Cr[blackPieces[i]];
    }
    return p;
}

// fetches the proper endgame reference for the current piece configuration.
uint32_t* getEndgameRef(int isWhite)
{
    // a unique number representing the current piece composition
    int hash =
        whitePieces[1] + whitePieces[2] * 9 + whitePieces[3] * 12 + whitePieces[4] * 15 + whitePieces[5] * 17 + whitePieces[6] * 19 +
        blackPieces[1] * 21 + whitePieces[2] * 30 + whitePieces[3] * 33 + whitePieces[4] * 36 + whitePieces[5] * 38 + whitePieces[6] * 40;

    // check if this endgame reference has already been opened and cached
    uint32_t* cachedRef = isWhite ? endgameRefW[hash] : endgameRefB[hash];
    if (cachedRef)
    {
        return cachedRef;
    }

    // otherwise, try to open this file.
    #define FN_SIZE 256
    char fileName[FN_SIZE] = "K";
    int idx = 1;
    for (int i = 1; i < 7; i++)
    {
        for (int j = 0; j < whitePieces[i]; j++)
        {
            snprintf(&fileName[idx], FN_SIZE - idx, "%c", pieceFEN[i]);
            idx++;
        }
    }

    snprintf(&fileName[idx], FN_SIZE - idx, "vK");
    idx += 2;

    for (int i = 1; i < 7; i++)
    {
        for (int j = 0; j < blackPieces[i]; j++)
        {
            snprintf(&fileName[idx], FN_SIZE - idx, "%c", pieceFEN[i]);
        }
    }
    
    snprintf(&fileName[idx], FN_SIZE - idx, "-%c99.bin", isWhite ? 'w' : 'b');

    // now that we finally extracted the file name, let's open up the file!
    FILE* endgameFile = fopen(fileName, "rb");

    if (!endgameFile)
    {
        puts("Error opening up endgame file!");
        printf("The name of the endgame file was %s\n", fileName);
        return 0;
    }

    // now read from the file
    Godel possibilities = getNumPossibilities();
    Godel elemCount = (possibilities >> 5) + 1;
    
    uint32_t* reference = (uint32_t*)malloc(sizeof(uint32_t) * elemCount);
    fread(reference, sizeof(uint32_t), elemCount, endgameFile);
    fclose(endgameFile);

    // cache and then return the contents of the file
    if (isWhite)
    {
        endgameRefW[hash] = reference;
    }
    else
    {
        endgameRefB[hash] = reference;
    }
    return reference;
}

void freeEndgameRefCache(int isWhite)
{
    if (isWhite)
    {
        for (int i = 0; i < PIECE_COMP_POSS; i++)
        {
            if (endgameRefW[i])
            {
                free(endgameRefW[i]);
                endgameRefW[i] = 0;
            }
        }
    }
    else
    {
        for (int i = 0; i < PIECE_COMP_POSS; i++)
        {
            if (endgameRefB[i])
            {
                free(endgameRefB[i]);
                endgameRefB[i] = 0;
            }
        }
    }
}
