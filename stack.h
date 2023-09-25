#ifndef STACK_DED
#define STACK_DED
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define StackDump(stk) StackDump_(stk, __FILE__, __LINE__, __FUNCTION__)
#define StackCtor(a, b) StackCtor_(a, b, #a, __FUNCTION__, __LINE__, __FILE__)

typedef unsigned long long Canary_t;
typedef int Elem_t;

const int POISON = INT_MAX;
const Canary_t STRUCT_CANARY_L_VAL = 0xFEE1DEAD;
const Canary_t STRUCT_CANARY_R_VAL = 0xFEE1DEAD;
const Canary_t DATA_CANARY_L_VAL = 0xDEADDEAD;
const Canary_t DATA_CANARY_R_VAL = 0xDEADDEAD;

enum Errors
{
	STK_NULLPTR = 0,
	DATA_NULLPTR = 1,
	RANGE_ERROR = 2,
	CANARY_ERROR = 3,
	HASH_ERROR = 4
};
enum Status {
	CONSTRUCTED = 0,
	DESTROYED = 1,
	BROKEN = -1
};

struct DumpInfo {
	size_t line;
	const char* obj_name;
	const char* file;
	const char* func;
};

struct Stack {
	Canary_t left_canary;
	Elem_t* data;
	size_t capacity;
	size_t size;
	int status;
	int hash_sum;
	DumpInfo info;
	Canary_t right_canary;
};

// Concepts
// DefineCtor(long);

int StackCtor_(Stack* stk, size_t capacity, const char* obj_name, const char* func, size_t line,const char* file );
int StackDtor(Stack* stk);

int StackPush(Stack* stk, Elem_t value);
int StackPop(Stack* stk, Elem_t* Ret_value);
int StackRealloc(Stack* stk, size_t capacity);

int StackVerify(const Stack* stk);


#endif // !STACK_DED

