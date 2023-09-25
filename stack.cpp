#include "stack.h"
#include "assert.h"
#include "hash.h"



#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(code, ...) code, __VA_ARGS__
#else
#define ON_DEBUG(code) 
#endif

static int StackDump_(const Stack* stk, const char* file, size_t line, const char* func) {
	printf("____________DUMP____________\n");
	printf("Stack[0x%X]\n", stk);
	printf("Object %s created in %s() from %s (%zu line)\n", stk->info.obj_name, stk->info.func, stk->info.file, stk->info.line);
	printf("Dump called in %s() from %s (%zu line)\n", func, file, line);
	printf("\n");
	printf("size = %zu\ncapacity = %zu\ndata[0x%X]\nhash_sum = %ull\noffset = %d", stk->size, stk->capacity, stk->data, stk->hash_sum, stk->offset);
	printf("{\n");
	for (size_t i = 0; i < stk->capacity; i++) {
		if (stk->data[i] == POISON) {
			printf("*[%zu] = POISON\n", i);
			continue;
		}
		printf("*[%zu] = %d\n", i, stk->data[i]);
	}
	printf("}\n");
	printf("____________________________\n");
	return 0;
}


static int SetError(unsigned* all_errors, int error) {
	*all_errors |= (int)(1 << error);
	return 0;
}

static int GetErrorInfo(unsigned error) {
	#define check(error_code) ( error & (1 << error_code) ) == (1 << error_code)

	if (check(STK_NULLPTR))  printf("\nstack ptr is null\n");
	if (check(DATA_NULLPTR)) printf("\nstack->data ptr is null\n");
	if (check(RANGE_ERROR))  printf("\nrange error\n");
	if (check(CANARY_ERROR)) printf("\ncanary error\n");
	if (check(HASH_ERROR))   printf("\nhash mismatch\n");
	return 0;

	#undef check
};

int StackVerify(Stack* stk) {
	unsigned error = 0;

	if (!stk) {
		SetError(&error, STK_NULLPTR);
		return error;
	}
	if (!stk->data) SetError(&error, DATA_NULLPTR);
	if (stk->capacity < stk->size) SetError(&error, RANGE_ERROR);
	if (*(stk->hash_sum) != HashStack(stk)) SetError(&error, HASH_ERROR);

	Canary_t data_canary_l = ((Canary_t*)stk->data)[-1];
	Canary_t data_canary_r = *((Canary_t*)(stk->data + stk->capacity));

	if ((stk->left_canary != STRUCT_CANARY_L_VAL || stk->right_canary != STRUCT_CANARY_R_VAL) 
		|| ((data_canary_l != DATA_CANARY_L_VAL) || (data_canary_r != DATA_CANARY_R_VAL)))
	{
		SetError(&error, CANARY_ERROR);
	}
	
	if (error != 0) {
		stk->status = BROKEN;
		GetErrorInfo(error);
		StackDump(stk);
	}
	return error;
}

int StackCtor_(Stack* stk, size_t capacity, const char* obj_name, const char* func, size_t line, const char* file) {
	stk->offset = (8 - (sizeof(Elem_t) * capacity) % 8) % 8;
	stk->data = (Elem_t*)calloc(capacity * sizeof(Elem_t) + 2 * sizeof(Canary_t) + stk->offset, sizeof(char));
	stk->hash_sum = (unsigned long long*)calloc(1, sizeof(unsigned long long));

	*((Canary_t* )(stk->data)) = 0xDEADDEAD;
	stk->data = (Elem_t*)((Canary_t*)stk->data + 1);

	stk->capacity = capacity;

	*(Canary_t*)((char*)stk->data + stk->capacity * sizeof(Elem_t) + stk->offset) = 0xDEADDEAD;
	
	stk->left_canary = STRUCT_CANARY_L_VAL;
	stk->right_canary = STRUCT_CANARY_R_VAL;
	
	StackDump(stk);
	stk->info.obj_name = obj_name;
	stk->info.file = file;
	stk->info.func = func;
	stk->info.line = line;
	*(stk->hash_sum) = HashStack(stk);
	ON_DEBUG(StackVerify(stk));
	stk->status = CONSTRUCTED;
	return 0;
}

int StackDtor(Stack* stk) {
	if (stk->status == CONSTRUCTED || stk->status == BROKEN) {
		free((char*)stk->data - sizeof(Canary_t));
		free(stk->hash_sum);
		stk->data = nullptr;
		stk->capacity = 0;
		stk->size = 0;
		
		return 0;
	}
	return -1;
}

int StackPush(Stack* stk, Elem_t value) {
	ON_DEBUG(StackVerify(stk));
	if (stk->size >= stk->capacity / 2) {
		StackRealloc(stk, 2 * stk->capacity);
	}

	stk->data[stk->size] = value;
	stk->size++;
	*(stk->hash_sum) = HashStack(stk);

	ON_DEBUG(StackVerify(stk));
	return 0;
}
int StackPop(Stack* stk, Elem_t* Ret_value) {
	ON_DEBUG(StackVerify(stk));

	if (stk->size < (stk->capacity / 4)) {
		StackRealloc(stk, stk->capacity / 4 + 1);
	}

	*Ret_value = stk->data[stk->size-1];
	stk->data[stk->size - 1] = POISON;
	stk->size--;
	*(stk->hash_sum) = HashStack(stk);

	ON_DEBUG(StackVerify(stk));
	return 0;
}

int StackRealloc(Stack* stk, size_t capacity) {
	ON_DEBUG(StackVerify(stk));
	Elem_t* tmp = (Elem_t*)realloc((char*)stk->data - sizeof(Canary_t), capacity * sizeof(Elem_t) + 2 * sizeof(Canary_t) + stk->offset);
	if (tmp != nullptr) stk->data = tmp;
	stk->data = (Elem_t*)((char*)stk->data + sizeof(Canary_t));

	stk->capacity = capacity;

	ON_DEBUG(StackVerify(stk));
	return 0;
}






