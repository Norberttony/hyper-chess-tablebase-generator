
#include "tablebase.h"

int fileSize = 0;
Godel possibilities = 528 * 64;

uint32_t *whiteWins;
uint32_t *blackLoses;
uint32_t *blackTemp;

// allocates memory for the tablebase
int allocTablebase()
{
    fileSize = (possibilities >> 5) + 1;

    // arrays that will be indexed by the Godel number and return 1 if the position belongs in
    // the set of positions
    whiteWins  = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);
    blackLoses = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);
    blackTemp  = (uint32_t*)malloc(sizeof(uint32_t) * fileSize);

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

        if (!success || isAttackingKing())
        {
            clr_bit32_arr(blackLoses, i);
            continue;
        }
        
        if (isCheckmate())
        {
            set_bit32_arr(blackLoses, i);
            if (i == 21248)
            {
                prettyPrintBoard();
                puts("The board above is checkmate for white.");
            }
        }
        else
        {
            clr_bit32_arr(blackLoses, i);
        }
    }

    // write the results to a file
    FILE *fileB = fopen("B0.bin", "wb");
    fwrite(blackLoses, sizeof(uint32_t), fileSize, fileB);
    fclose(fileB);

    puts("Initialization complete!");
}

int tablebaseStep(int depth)
{
    puts("Determining white wins from black loses positions");

    char fileName[1000];

    int newPosition = 0;

    toPlay = black;
    notToPlay = white;
    for (Godel i = 0; i < possibilities; i++)
    {
        if (get_bit32_arr(blackLoses, i) && loadGodelNumber(i))
        {
            // white has a mate in all predecessor positions

            toPlay = black;
            notToPlay = white;

            Unmove unmoves[MAX_MOVES];
            int size = generateUnmoves((Unmove*)unmoves);
            for (int j = 0; j < size; j++)
            {
                loadGodelNumber(i);

                // don't unmake a move that leaves opponent king in check.
                toPlay = black;
                notToPlay = white;
                
                Unmove unmove = unmoves[j];
                makeUnmove(unmove);

                toPlay = white;
                notToPlay = black;
                if (isAttackingKing())
                {
                    continue;
                }

                // check if a new position was added
                Godel changed = getGodelNumber();
                newPosition = newPosition || !get_bit32_arr(whiteWins, changed);

                // mark this position as winnable!
                set_bit32_arr(whiteWins, changed);
            }
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
    for (Godel i = 0; i < possibilities; i++)
    {
        if (get_bit32_arr(whiteWins, i) && loadGodelNumber(i))
        {
            toPlay = white;
            notToPlay = black;

            Unmove unmoves[MAX_MOVES];
            int size = generateUnmoves((Unmove*)unmoves);
            for (int j = 0; j < size; j++)
            {
                loadGodelNumber(i);

                // don't unmake a move that leaves opponent king in check.
                toPlay = white;
                notToPlay = black;
                
                Unmove unmove = unmoves[j];
                makeUnmove(unmove);

                toPlay = black;
                notToPlay = white;
                if (isAttackingKing())
                {
                    continue;
                }

                // mark this position as losable
                set_bit32_arr(blackTemp, getGodelNumber());
            }
        }
    }

    puts("Determining which of these black to play and try to lose are actually losses.");

    // P4 goes through all temp positions, generates successor positions, and ensures that all
    // successor positions are in whiteWins. If so, that position is added to blackLoses
    toPlay = black;
    notToPlay = white;
    for (Godel i = 0; i < possibilities; i++)
    {
        if (get_bit32_arr(blackTemp, i) && loadGodelNumber(i))
        {

            Move moves[MAX_MOVES];
            int size = generateMoves((Move*)moves);

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

                unmakeMove(move);

                // to-do: should probe sub-databases in order to determine win/loss.
                if (move & move_captMask)
                {
                    // alright, so it's just this one piece that got captured. this is a draw then.
                    alwaysWinning = 0;
                    break;
                }

                if (!get_bit32_arr(whiteWins, changed))
                {
                    alwaysWinning = 0;
                    break;
                }
            }

            if (alwaysWinning && (size > 0 || isAttackingKing()))
            {
                newPosition = newPosition || !get_bit32_arr(blackLoses, i);
                set_bit32_arr(blackLoses, i);
            }
        }
    }

    snprintf(fileName, 1000, "B%d.bin", depth);
    FILE* fileB = fopen(fileName, "wb");
    fwrite(blackLoses, sizeof(uint32_t), fileSize, fileB);
    fclose(fileB);

    puts("Depth complete.");

    return newPosition;
}

void createDepthToMateFile(int maxDepth)
{
    char fileName[1000];

    // depth to mate in ply
    unsigned short int* depthToMateW = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    unsigned short int* depthToMateL = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    if (!depthToMateW || !depthToMateL)
    {
        puts("Could not allocate depth to mate array");
        return;
    }
    for (int i = 0; i < possibilities; i++)
    {
        depthToMateW[i] = 0;
        depthToMateL[i] = 1;
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
        if (depthToMateL[g] == 1 && get_bit32_arr(losses, g))
        {
            // first time encountering a black loss here
            depthToMateL[g] = 0;
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
            if (depthToMateW[g] == 0 && get_bit32_arr(wins, g))
            {
                // first time encountering a white win here
                depthToMateW[g] = d * 2 - 1;
            }
            if (depthToMateL[g] == 1 && get_bit32_arr(losses, g))
            {
                // first time encountering a black loss here
                depthToMateL[g] = d * 2;
            }
        }
    }

    for (Godel g = 0; g < possibilities; g++)
    {
        if (depthToMateL[g] == 1)
        {
            depthToMateL[g] = 0;
        }
    }

    free(wins);
    free(losses);

    // store depth to mate in a special file
    FILE* fileDTMW = fopen("DTMW.bin", "wb");
    fwrite(depthToMateW, sizeof(unsigned short int), possibilities, fileDTMW);
    fclose(fileDTMW);

    FILE* fileDTML = fopen("DTML.bin", "wb");
    fwrite(depthToMateL, sizeof(unsigned short int), possibilities, fileDTML);
    fclose(fileDTML);
}
