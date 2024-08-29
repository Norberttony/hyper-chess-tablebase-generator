
#include "transform.h"

#include <stdio.h>

int main(void)
{
    populateTransforms();

    loadFEN("8/8/8/k7/8/8/6K1/8 w -");
    prettyPrintBoard();

    int whiteSq = pop_lsb(position[white + king]);
    int blackSq = pop_lsb(position[black + king]);

    // print the transformations
    int* transform = transforms[whiteSq][blackSq];
    puts("White king:");
    printf("%s -> %s\n", squareNames[whiteSq], squareNames[transform[whiteSq]]);

    puts("Black king:");
    printf("%s -> %s\n", squareNames[blackSq], squareNames[transform[blackSq]]);

    return 0;
}
