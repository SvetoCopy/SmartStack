#ifndef DED_HASH
#define DED_HASH

#include "stack.h"

// typedef
unsigned long long Hash(const void* arr, size_t len);
unsigned long long HashStack(Stack* stk);
unsigned long long GavGavHash(char* data, size_t size);

#endif