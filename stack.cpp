#include "stack.h"
#include "assert.h"
#include "hash.h"

#ifdef _DEBUG


StackLogger logger = {};
bool is_opened = false;

int StackLoggerCtor(StackLogger* logger_, const char* name) {
	logger_->name = name;
	fopen_s(&(logger_->file), logger_->name, "w");
	is_opened = true;
	return 0;
}

static int StackDump_(const Stack* stk, const char* file, size_t line, const char* func) {
	if (!is_opened) StackLoggerCtor(&logger, "dump.log");
	FILE* f = logger.file;
	fprintf(f, "____________DUMP____________\n");
	fprintf(f, "Stack[%p]\n", stk);
	fprintf(f, "Object %s created in %s() from %s (%zu line)\n",
			stk->info.obj_name, stk->info.func, stk->info.file, stk->info.line);
	fprintf(f, "Dump called in %s() from %s (%zu line)\n", func, file, line);
	fprintf(f, "\n");
	fprintf(f, "size = %zu\n"
		   "capacity = %zu\n"
		   "data[0x%X]\n"
		   "hash_sum = %ull\n", 
		    stk->size, 
			stk->capacity, 
			stk->data, 
			stk->hash_sum);
	fprintf(f, "{\n");
	fprintf(f, "%ull\n", ((Canary_t*)(stk->data))[-1]);
	for (size_t i = 0; i < stk->capacity; i++) {
		if (stk->data[i] == POISON) {
			fprintf(f, "*[%zu] = POISON\n", i);
		}
		else {
			fprintf(f, "*[%zu] = %d\n", i, stk->data[i]);
		}
	}
	fprintf(f, "%ull\n", (Canary_t)(stk->data[stk->capacity]));
	fprintf(f, "}\n");
	fprintf(f, "____________________________\n");
	// fclose(f);
	return 0;
}

static int SetError(unsigned* all_errors, int error) {
	*all_errors |= (1 << error);
	return 0;
}


// printError
static int PrintErrorInfo(unsigned error) {
	#define check(error_code) ( error & (1 << error_code) ) == (1 << error_code)
	if (check(STK_NULLPTR))  fprintf(logger.file, "\nstack ptr is null\n");
	if (check(DATA_NULLPTR)) fprintf(logger.file, "\nstack->data ptr is null\n");
	if (check(RANGE_ERROR))  fprintf(logger.file, "\nrange error\n");
	if (check(CANARY_ERROR)) fprintf(logger.file, "\ncanary error\n");
	if (check(HASH_ERROR))   fprintf(logger.file, "\nhash mismatch\n");
	#undef check
	return 0;
};

int StackVerify(Stack* stk) {
	unsigned error = 0;
	if (!is_opened) StackLoggerCtor(&logger, "dump.log");

	if (!stk) {
		SetError(&error, STK_NULLPTR);
		PrintErrorInfo(error);
		return error;
	}
	if (!stk->data) {
		SetError(&error, DATA_NULLPTR);
		PrintErrorInfo(error);
		return error;
	}
	if (stk->capacity < stk->size) SetError(&error, RANGE_ERROR);

	unsigned long long hash_sum = stk->hash_sum;
	stk->hash_sum = 0;
	if (hash_sum != HashStack(stk)) SetError(&error, HASH_ERROR);
	stk->hash_sum = hash_sum;
	Canary_t data_canary_l = ((Canary_t*)stk->data)[-1];
	Canary_t data_canary_r = *(Canary_t*)((char*)stk->data + stk->capacity * sizeof(Elem_t));

	
	if ((stk->left_canary != STRUCT_CANARY_L_VAL || stk->right_canary != STRUCT_CANARY_R_VAL) || 
		(data_canary_l != DATA_CANARY_L_VAL || data_canary_r != DATA_CANARY_R_VAL))
	{
		SetError(&error, CANARY_ERROR);
	}
	
	if (error != 0) {
		
		stk->status = BROKEN;
		PrintErrorInfo(error);
		StackDump(stk);
	}

	return error;
}

int StackCtor_(Stack* stk, size_t capacity, const char* obj_name, const char* func, size_t line, const char* file) {
	if (!is_opened) StackLoggerCtor(&logger, "dump.log");

	if (!stk) {
		fprintf(logger.file, "nullptr stack_ptr");
		return -1;}

	while (capacity * sizeof(Elem_t) % ALIGNMENT != 0)
		capacity++;

	stk->data = (Elem_t*)calloc(capacity * sizeof(Elem_t) + 2 * sizeof(Canary_t), sizeof(char));

	*((Canary_t*)(stk->data)) = 0xDEADDEAD;
	stk->data = (Elem_t*)((Canary_t*)stk->data + 1);

	stk->capacity = capacity;

	*(Canary_t*)((char*)stk->data + stk->capacity * sizeof(Elem_t)) = 0xDEADDEAD;


	stk->left_canary = STRUCT_CANARY_L_VAL;
	stk->right_canary = STRUCT_CANARY_R_VAL;
	
	stk->info.obj_name = obj_name;
	stk->info.file = file;
	stk->info.func = func;
	stk->info.line = line;
	stk->hash_sum = 0;
	stk->status = CONSTRUCTED;
	stk->hash_sum = HashStack(stk);
	if (StackVerify(stk) != 0) return -1;
	return 0;
}

int StackDtor(Stack* stk) {
	if (stk->status == CONSTRUCTED || stk->status == BROKEN) {
		free((char*)stk->data - sizeof(Canary_t));
		stk->hash_sum = 0;
		stk->data = nullptr;
		stk->capacity = 0;
		stk->size = 0;
		fclose(logger.file);
		return 0;
	}
	return -1;
}
int StackRealloc(Stack* stk, size_t capacity) {
	if (StackVerify(stk) != 0) return -1;
	
	while (capacity * sizeof(Elem_t) % ALIGNMENT != 0)
		capacity++;

	Elem_t * tmp = (Elem_t*)realloc((char*)stk->data - sizeof(Canary_t), capacity * sizeof(Elem_t) + 2 * sizeof(Canary_t));

	if (tmp != nullptr) {
		stk->data = tmp;
	}

	stk->data = (Elem_t*)((char*)stk->data + sizeof(Canary_t));
	
	stk->capacity = capacity;
	*(Canary_t*)((char*)stk->data + stk->capacity * sizeof(Elem_t)) = 0xDEADDEAD;

	
	stk->hash_sum = 0;
	stk->hash_sum = HashStack(stk);
	return 0;
}

int StackPush(Stack* stk, Elem_t value) {
	if (StackVerify(stk) != 0) return -1;

	if (stk->size >= stk->capacity / 2) {
		StackRealloc(stk, 2 * stk->capacity);
	}

	stk->data[stk->size] = value;
	stk->size++;

	stk->hash_sum = 0;
	stk->hash_sum = HashStack(stk);

	if (StackVerify(stk) != 0) return -1;
	return 0;
}
int StackPop(Stack* stk, Elem_t* Ret_value) {
	if (!Ret_value) {
		if (!is_opened) StackLoggerCtor(&logger, "dump.log");
		fprintf(logger.file, "Ret value null");
		return -1;
	}
	if (StackVerify(stk) != 0) return -1;

	if (stk->size < (stk->capacity / 4)) {
		StackRealloc(stk, stk->capacity / 4 + 1);
	}

	*Ret_value = stk->data[stk->size - 1];
	stk->data[stk->size - 1] = POISON;
	stk->size--;

	stk->hash_sum = 0;
	stk->hash_sum = HashStack(stk);

	if (StackVerify(stk) != 0) return -1;
	return 0;
}

#else
int StackCtor(Stack* stk, size_t capacity) {
	stk->data = (Elem_t*)calloc(capacity, sizeof(Elem_t));
	stk->capacity = capacity;
	return 0;
}

int StackDtor(Stack* stk) {
	free(stk->data);
	stk->data = nullptr;
	stk->capacity = 0;
	stk->size = 0;

	return 0;

}

int StackRealloc(Stack* stk, size_t capacity) {

	stk->data = (Elem_t*)calloc(capacity, sizeof(Elem_t));
	stk->capacity = capacity;

	return 0;
}

int StackPush(Stack* stk, Elem_t value) {
	if (stk->size >= stk->capacity / 2) {
		StackRealloc(stk, 2 * stk->capacity);
	}

	stk->data[stk->size] = value;
	stk->size++;
	return 0;
}

int StackPop(Stack* stk, Elem_t* Ret_value) {
	if (stk->size < (stk->capacity / 4)) {
		StackRealloc(stk, stk->capacity / 4 + 1);
	}

	*Ret_value = stk->data[stk->size - 1];
	stk->data[stk->size - 1] = POISON;
	stk->size--;
	return 0;
}

#endif










