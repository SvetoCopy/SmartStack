#include "hash.h"
#include <string.h>
unsigned long long HashStack(Stack* stk) {
    char* struct_ptr = (char*)stk;
    return GavGavHash((char*)stk->data, stk->size * sizeof(Elem_t)) + GavGavHash(struct_ptr, sizeof(Stack));
}

unsigned long long GavGavHash(char* data, size_t size)
{
    int m = 995;
    int seed = 0;
    int r = 24;
    int h = seed ^ size;

    char* arr = data;
    int k = 0;

    while (size >= 4)
    {
        k = arr[0];
        k |= arr[1] << 8;
        k |= arr[2] << 16;
        k |= arr[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        arr += 4;
        size -= 4;
    }

    switch (size)
    {
    case 3:
        h ^= arr[2] << 16;
    case 2:
        h ^= arr[1] << 8;
    case 1:
        h ^= arr[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}