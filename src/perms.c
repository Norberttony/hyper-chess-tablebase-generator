
#include "perms.h"

U64 perms1[64] = { 0 };
U64 perms2[2016] = { 0 };

U64 *perms[MAX_PERMS + 1] =
{
    0, perms1, perms2
};

void initPerms(void)
{
    for (int i = 1; i <= MAX_PERMS; i++)
    {
        printf("Initializing perm %d\n", i);

        U64 start = 0ULL;
        for (int j = 0; j < i; j++)
        {
            start = (start << 1) + 1;
        }

        printBitboard(start);

        int idx = 0;
        while (start)
        {
            if (getPerm2Index(start) != idx)
            {
                puts("ERROR SETTING UP PERM 2");
            }
            perms[i][idx++] = start;
            start = nextPerm(start);
        }
    }
}

int getPerm2Index(U64 board)
{
    int sq1 = pop_lsb(board);
    int sq2 = pop_lsb(board & (board - 1)) - 1;
    return sq1 + sq2 * (sq2 + 1) / 2;
}

// try to shift first available bit to the right. if not possible, return 0
// all bits that were not shifted end up at the start
U64 nextPerm(U64 current)
{
    U64 start = 0ULL;

    while (current)
    {
        // can I shift this bit?
        int lsb = pop_lsb(current);

        // bit is already at the very end
        if (lsb == 63)
        {
            break;
        }

        U64 nextSpot = 1ULL << lsb + 1;

        if (current & nextSpot)
        {
            // no, I cannot. add this bit to the start and remove it from current.
            start = (start << 1) + 1;
            current ^= 1ULL << lsb;
        }
        else
        {
            // hurray, it is possible!
            current ^= nextSpot | 1ULL << lsb;

            return start | current;
        }
    }

    return 0ULL;
}
