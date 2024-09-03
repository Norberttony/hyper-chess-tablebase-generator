
#include "transform.h"

int* transforms[64][64];

// no transformation
const int noTransform[64] = 
{
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

// reflect over a8 - h1 diagonal
const int reflectA8H1[64] = 
{
    a8, a7, a6, a5, a4, a3, a2, a1,
    b8, b7, b6, b5, b4, b3, b2, b1,
    c8, c7, c6, c5, c4, c3, c2, c1,
    d8, d7, d6, d5, d4, d3, d2, d1,
    e8, e7, e6, e5, e4, e3, e2, e1,
    f8, f7, f6, f5, f4, f3, f2, f1,
    g8, g7, g6, g5, g4, g3, g2, g1,
    h8, h7, h6, h5, h4, h3, h2, h1
};

// reflect over a1 - h8 diagonal
const int reflectA1H8[64] =
{
    h1, h2, h3, h4, h5, h6, h7, h8,
    g1, g2, g3, g4, g5, g6, g7, g8,
    f1, f2, f3, f4, f5, f6, f7, f8,
    e1, e2, e3, e4, e5, e6, e7, e8,
    d1, d2, d3, d4, d5, d6, d7, d8,
    c1, c2, c3, c4, c5, c6, c7, c8,
    b1, b2, b3, b4, b5, b6, b7, b8,
    a1, a2, a3, a4, a5, a6, a7, a8
};

// reflect horizontally
const int reflectHoriz[64] =
{
    h8, g8, f8, e8, d8, c8, b8, a8,
    h7, g7, f7, e7, d7, c7, b7, a7,
    h6, g6, f6, e6, d6, c6, b6, a6,
    h5, g5, f5, e5, d5, c5, b5, a5,
    h4, g4, f4, e4, d4, c4, b4, a4,
    h3, g3, f3, e3, d3, c3, b3, a3,
    h2, g2, f2, e2, d2, c2, b2, a2,
    h1, g1, f1, e1, d1, c1, b1, a1
};

// reflect vertically
const int reflectVert[64] =
{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

// rotate 90* CCW
const int rotateCCW[64] =
{
    a1, a2, a3, a4, a5, a6, a7, a8,
    b1, b2, b3, b4, b5, b6, b7, b8,
    c1, c2, c3, c4, c5, c6, c7, c8,
    d1, d2, d3, d4, d5, d6, d7, d8,
    e1, e2, e3, e4, e5, e6, e7, e8,
    f1, f2, f3, f4, f5, f6, f7, f8,
    g1, g2, g3, g4, g5, g6, g7, g8,
    h1, h2, h3, h4, h5, h6, h7, h8
};

// rotate 90* CC
const int rotateCC[64] =
{
    h8, h7, h6, h5, h4, h3, h2, h1,
    g8, g7, g6, g5, g4, g3, g2, g1,
    f8, f7, f6, f5, f4, f3, f2, f1,
    e8, e7, e6, e5, e4, e3, e2, e1,
    d8, d7, d6, d5, d4, d3, d2, d1,
    c8, c7, c6, c5, c4, c3, c2, c1,
    b8, b7, b6, b5, b4, b3, b2, b1,
    a8, a7, a6, a5, a4, a3, a2, a1
};

// flip the board 180 degrees
const int flip[64] =
{
    h1, g1, f1, e1, d1, c1, b1, a1,
    h2, g2, f2, e2, d2, c2, b2, a2,
    h3, g3, f3, e3, d3, c3, b3, a3,
    h4, g4, f4, e4, d4, c4, b4, a4,
    h5, g5, f5, e5, d5, c5, b5, a5,
    h6, g6, f6, e6, d6, c6, b6, a6,
    h7, g7, f7, e7, d7, c7, b7, a7,
    h8, g8, f8, e8, d8, c8, b8, a8
};

// first reflects horizontally, and then reflects over the a8 - h1 diagonal
const int reflectHorizThenA8H1[64] =
{
    a1, a2, a3, a4, a5, a6, a7, a8,
    b1, b2, b3, b4, b5, b6, b7, b8,
    c1, c2, c3, c4, c5, c6, c7, c8,
    d1, d2, d3, d4, d5, d6, d7, d8,
    e1, e2, e3, e4, e5, e6, e7, e8,
    f1, f2, f3, f4, f5, f6, f7, f8,
    g1, g2, g3, g4, g5, g6, g7, g8,
    h1, h2, h3, h4, h5, h6, h7, h8
};

// first reflects vertically, and then reflects over the a8 - h1 diagonal
const int reflectVertThenA8H1[64] =
{
    h8, h7, h6, h5, h4, h3, h2, h1,
    g8, g7, g6, g5, g4, g3, g2, g1,
    f8, f7, f6, f5, f4, f3, f2, f1,
    e8, e7, e6, e5, e4, e3, e2, e1,
    d8, d7, d6, d5, d4, d3, d2, d1,
    c8, c7, c6, c5, c4, c3, c2, c1,
    b8, b7, b6, b5, b4, b3, b2, b1,
    a8, a7, a6, a5, a4, a3, a2, a1
};


// 1 if the given square is under the diagonal and 0 if it isn't
const int isUnderDiagonal[64] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 0
};


void setTransform(int wSq, int* tran);
void combineTransforms(int* tran1, int* tran2);


void populateTransforms()
{
    // in this case, no transformation is necessary to take the white king and fit it into the
    // triangle
    // for now, ignore the a8 - h1 diagonal and just set these as no transform
    const int noTr[] = { d6, d7, d8, c7, c8, b8 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(noTr[i], (int*)&noTransform);
    }

    // in this case, rotating horizontally is the way to get the king into the triangle.
    const int horiz[] = { g8, f7, f8, e6, e7, e8 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(horiz[i], (int*)&reflectHoriz);
    }

    // vertical
    const int vert[] = { d3, d2, d1, c2, c1, b1 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(vert[i], (int*)&reflectVert);
    }

    // reflect over A1 - H8 diagonal
    const int a8h1[] = { h2, h3, h4, g3, g4, f4 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(a8h1[i], (int*)&reflectA1H8);
    }

    // reflect over A8 - H1 diagonal
    const int a1h8[] = { a7, a6, a5, b6, b5, c5 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(a1h8[i], (int*)&reflectA8H1);
    }

    // rotating 90* CC
    const int rotCC[] = { a4, a3, a2, b4, b3, c4 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(rotCC[i], (int*)&rotateCC);
    }

    // rotating 90* CCW
    const int rotCCW[] = { f5, g6, g5, h7, h6, h5 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(rotCCW[i], (int*)&rotateCCW);
    }

    // flip the board
    const int flipSq[] = { e3, e2, e1, f2, f1, g1 };
    for (int i = 0; i < 6; i++)
    {
        setTransform(flipSq[i], (int*)&flip);
    }
    
    // now to handle the diagonals
    // if the white king is already on the a8 - d5 diagonal, then we only have to worry about
    // reflecting the black king to be above the diagonal. Otherwise, no transformation.
    const int a8d5[] = { a8, b7, c6, d5 };
    for (int i = 0; i < 4; i++)
    {
        int wSq = a8d5[i];
        for (int j = 0; j < 64; j++)
        {
            if (isUnderDiagonal[j])
            {
                // put king above diagonal
                transforms[wSq][j] = (int*)&reflectA8H1;
            }
            else
            {
                // no action needed, king already above diagonal
                transforms[wSq][j] = (int*)&noTransform;
            }
        }
    }

    // for the e4 - h1 diagonal, we must always reflect to get the white king on the right diagonal
    // but if the black king is below the diagonal, we rotate the board in one swoop and get both
    // kings on their correct squares
    const int e4h1[] = { e4, f3, g2, h1 };
    for (int i = 0; i < 4; i++)
    {
        int wSq = e4h1[i];
        for (int j = 0; j < 64; j++)
        {
            if (isUnderDiagonal[reflectA1H8[j]])
            {
                transforms[wSq][j] = (int*)&flip;
            }
            else
            {
                transforms[wSq][j] = (int*)&reflectA1H8;
            }
        }
    }

    // here, we reflect the white king horizontally, but must make a special transformation
    // (reflect horizontally then over a8 - h1 diagonal) if the black king ends up below the diagonal
    const int e5h8[] = { e5, f6, g7, h8 };
    for (int i = 0; i < 4; i++)
    {
        int wSq = e5h8[i];
        for (int j = 0; j < 64; j++)
        {
            if (isUnderDiagonal[reflectHoriz[j]])
            {
                transforms[wSq][j] = (int*)&reflectHorizThenA8H1;
            }
            else
            {
                transforms[wSq][j] = (int*)&reflectHoriz;
            }
        }
    }

    // reflect the white king horizontally, but must make special transformation (reflect
    // horizontally then over )
    const int a1d4[] = { a1, b2, c3, d4 };
    for (int i = 0; i < 4; i++)
    {
        int wSq = a1d4[i];
        for (int j = 0; j < 64; j++)
        {
            if (isUnderDiagonal[reflectVert[j]])
            {
                transforms[wSq][j] = (int*)&reflectVertThenA8H1;
            }
            else
            {
                transforms[wSq][j] = (int*)&reflectVert;
            }
        }
    }
}

void setTransform(int wSq, int* tran)
{
    for (int j = 0; j < 64; j++)
    {
        transforms[wSq][j] = tran;
    }
}

void combineTransforms(int* tran1, int* tran2)
{
    // get the new transformation by running the original square through t1 and then t2
    for (int s = 0; s < 64; s++)
    {
        printf("%s, ", squareNames[tran2[tran1[s]]]);

        if (s % 8 == 7)
        {
            puts("");
        }
    }
}
