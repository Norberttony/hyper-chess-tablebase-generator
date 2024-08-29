
#include "transform.h"
#include "godel.h"

#include <stdio.h>

int main(void)
{
    populateTransforms();
    populateGodelLookups();

    int godel = 3495;
    
    loadGodelNumber(godel);

    // print the board
    prettyPrintBoard();

    printf("%lu", getGodelNumber());

    return 0;
}
