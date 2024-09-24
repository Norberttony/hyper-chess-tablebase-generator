
#include "transform.h"
#include "godel.h"
#include "tablebase.h"
#include "perms.h"

#include <stdio.h>


void initMoveGen(void);

void test(void);

void userDemo();

unsigned short int* DTZW;
unsigned short int* DTZL;

int main(void)
{
    initPerms();
    initMoveGen();
    populateTransforms();
    populateGodelLookups();

    if (!allocTablebase())
    {
        return -1;
    }

    initTablebase();


    int depth = 0;
    while (tablebaseStep(++depth))
    {
        printf("Depth %d done.\n", depth);
    }

    createDTZFile(depth);

    // read the DTZW file
    DTZW = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    FILE* DTZWFile = fopen("DTZW.bin", "rb");
    fread(DTZW, sizeof(unsigned short int), possibilities, DTZWFile);
    fclose(DTZWFile);

    // read the DTML file
    DTZL = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    FILE* DTZLFile = fopen("DTZL.bin", "rb");
    fread(DTZL, sizeof(unsigned short int), possibilities, DTZLFile);
    fclose(DTZLFile);

    loadFEN("b3k3/8/8/8/8/8/8/PRK5 w -");
    printf("%llu\n", getGodelNumber());

    userDemo();

    return 0;
}

void initMoveGen(void)
{
    // Initialization!
    generateZobristHashes();
    populateKingMoves();
    populateRanksAndFiles(); // in order to use genDeathSquares (used by populateDeathSquares)
    populateDeathSquares();
    populateSpringerLeaps();
    populateSpringerCaptures();
    populateRetractorCaptures();
    initMagicBitboards(0); // rook magic bitboards
    initMagicBitboards(1); // bishop magic bitboards
}


void userDemo()
{
    while (1)
    {
        Godel thisGodel = getGodelNumber();

        prettyPrintBoard();
        printf("Godel number: %lu\n", thisGodel);

        if (toPlay == white)
        {
            printf("DTZ: %hu\n", DTZW[thisGodel]);
        }
        else if (toPlay == black)
        {
            printf("DTZ: %hu\n", DTZL[thisGodel]);
        }

        // show move results
        puts("Move results:");
        Move moves[MAX_MOVES];
        int size = generateMoves(&moves[0], 0);
        for (int i = 0; i < size; i++)
        {
            Move m = moves[i];
            if (!isMoveLegal(m))
            {
                continue;
            }
            printMove(m);
            makeMove(m);
            if (toPlay == white)
            {
                printf("DTZ: %d\n", DTZW[getGodelNumber()]);
            }
            else if (toPlay == black)
            {
                printf("DTZ: %d\n", DTZL[getGodelNumber()]);
            }
            unmakeMove(m);
        }
        
        puts("Your turn! Type in a move:");
        char move[10]; // yes, risks buffer overflow, I know...
        scanf("%s", &move);

        // get coordinates of from square and to square
        int fromF = move[0] - 'a';
        int fromR = 7 - (move[1] - '1');

        int toF = move[2] - 'a';
        int toR = 7 - (move[3] - '1');

        int from = fromR * 8 + fromF;
        int to = toR * 8 + toF;

        // try to play the move...
        Move chosenMove = chooseMove(from, to);
        if (!chosenMove)
        {
            puts("That's not a legal move");
            prettyPrintBoard();
            continue;
        }
    }
}
