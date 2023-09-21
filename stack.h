#ifndef STACK_DED
#define STACK_DED
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef int Elem_t;
const int Poison = INT_MAX;

enum Errors
{
	STK_NULLPTR = 0,
	DATA_NULLPTR = 1,
	RANGE_ERROR = 2
};

struct Stack {
	Elem_t* data;
	size_t capacity;
	size_t size;

	size_t line;
	const char* file;
	const char* func;
};

int StackCtor(Stack* stk, size_t capacity, const char* func, size_t line,const char* file );
int StackDtor(Stack* stk);

int StackPush(Stack* stk, Elem_t value);
int StackPop(Stack* stk, Elem_t* Ret_value);
int StackRealloc(Stack* stk, size_t capacity);

int StackVerify(const Stack* stk);
int GetErrorInfo(unsigned error);


#endif // !STACK_DED

