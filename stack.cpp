#include "stack.h"
#include "assert.h"
#include <math.h>
#include "dump.h"

#define StackDump(stk) StackDump(stk, __FILE__, __LINE__, __FUNCTION__)

int StackCtor(Stack* stk, size_t capacity, const char* func, size_t line, const char* file) {
	stk->data = (Elem_t*)calloc(capacity, sizeof(Elem_t));
	stk->capacity = capacity;
	stk->file = file;
	stk->func = func;
	stk->line = line;
	StackVerify(stk);
	return 0;
}
int StackDtor(Stack* stk) {
	free(stk->data);
	stk->data = nullptr;
	stk->capacity = 0;
	stk->size = 0;
	return 0;
}

int StackPush(Stack* stk, Elem_t value) {
	assert(stk != nullptr);
	assert(stk->data != nullptr);
	
	if (stk->size >= (stk->capacity / 2)) {
		StackRealloc(stk, 2 * stk->capacity);
	}
	stk->data[stk->size] = value;
	stk->size++;
	StackVerify(stk);
	return 0;
}
int StackPop(Stack* stk, Elem_t* Ret_value) {
	assert(stk != nullptr);
	assert(stk->data != nullptr);
	if (stk->size < (stk->capacity / 4)) {
		StackRealloc(stk, stk->capacity / 4 + 1);
	}
	*Ret_value = stk->data[stk->size-1];
	stk->data[stk->size - 1] = Poison;
	stk->size--;
	StackVerify(stk);
	return 0;
}

int StackRealloc(Stack* stk, size_t capacity) {
	stk->data = (Elem_t*)realloc(stk->data, capacity * sizeof(Elem_t));

	assert(stk->data != nullptr);

	stk->capacity = capacity;
	StackVerify(stk);
	return 0;
}

static int SetError(unsigned* all_errors, int error) {
	*all_errors |= (int)(pow(2, error));
	return 0;
}

int StackVerify(const Stack* stk) {
	unsigned error = 0;
	if (!stk) {
		SetError(&error, STK_NULLPTR);
		return error;
	}
	if (!stk->data) SetError(&error, DATA_NULLPTR);;
	if (stk->capacity < stk->size) SetError(&error, RANGE_ERROR);
	if (error != 0) {
		GetErrorInfo(error);
		StackDump(stk);
	}
	return error;
}


int GetErrorInfo(unsigned error) {
	if ((error & 1) == 1) printf("\nstack ptr is null\n");
	if ((error & 2) == 2) printf("\nstack->data ptr is null\n");;
	if ((error % 4) == 4) printf("\nSize > capacity\n");
	return 0;
};


