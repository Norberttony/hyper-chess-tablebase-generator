#ifndef VECTOR_HEADER
#define VECTOR_HEADER


#include <stdlib.h>


struct vector
{
    void** arr;
    int size;
    int cap;
};


struct vector v_init(int cap);
void v_pushBack(struct vector*, void*);
void* v_popBack(struct vector* v);

#endif