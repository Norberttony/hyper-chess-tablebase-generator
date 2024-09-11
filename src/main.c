
#include "transform.h"
#include "godel.h"
#include "tablebase.h"

#include <stdio.h>


void initMoveGen(void);

void userDemo();

unsigned short int* DTMW;
unsigned short int* DTML;

int main(void)
{
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

    createDepthToMateFile(depth);

    // read the DTMW file
    DTMW = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    FILE* DTMWFile = fopen("DTMW.bin", "rb");
    fread(DTMW, sizeof(unsigned short int), possibilities, DTMWFile);
    fclose(DTMWFile);

    // read the DTML file
    DTML = (unsigned short int*)malloc(sizeof(unsigned short int) * possibilities);
    FILE* DTMLFile = fopen("DTML.bin", "rb");
    fread(DTML, sizeof(unsigned short int), possibilities, DTMLFile);
    fclose(DTMLFile);

    loadFEN("3k4/8/8/8/8/8/8/R3K3 w -");

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
            printf("DTM: %hu\n", DTMW[thisGodel]);
        }
        else if (toPlay == black)
        {
            printf("DTM: %hu\n", DTML[thisGodel]);
        }

        // show move results
        puts("Move results:");
        Move moves[MAX_MOVES];
        int size = generateMoves(&moves[0]);
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
                printf("DTM: %d\n", DTMW[getGodelNumber()]);
            }
            else if (toPlay == black)
            {
                printf("DTM: %d\n", DTML[getGodelNumber()]);
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
