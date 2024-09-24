
#include "vector.h"

struct vector v_init(int cap)
{
    struct vector v = { 0, 0, cap };

    v.arr = malloc(sizeof(void*) * cap);

    return v;
}

void v_pushBack(struct vector* v, void* item)
{
    // resize array if necessary
    if (v->size == v->cap)
    {
        v->cap *= 2;

        void** newArr = malloc(sizeof(void*) * v->cap);

        if (!newArr)
        {
            puts("ERROR when resizing vector");
            printf("Attempted to create a vector of size %d.\n", v->cap);
            exit(-1);
        }

        // copy values from old array
        for (int i = 0; i < v->size; i++)
        {
            newArr[i] = v->arr[i];
        }
        
        // set new array
        free(v->arr);
        v->arr = newArr;
    }

    // add item
    v->arr[v->size++] = item;
}

void* v_popBack(struct vector* v)
{
    if (v->size * 4 <= v->cap)
    {
        // decrease size of the vector by one half
        v->cap /= 2;

        void** newArr = malloc(sizeof(void*) * v->cap);

        if (!newArr)
        {
            puts("ERROR when resizing vector down");
            exit(-1);
        }

        // copy values from old array
        for (int i = 0; i < v->size; i++)
        {
            newArr[i] = v->arr[i];
        }

        free(v->arr);
        v->arr = newArr;
    }

    return v->arr[--v->size];
}
