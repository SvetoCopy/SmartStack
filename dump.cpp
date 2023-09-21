#include "dump.h"

int StackDump(const Stack* stk, const char* file, size_t line,const char* func) {
	printf("____________DUMP____________\n");
	printf("Stack[%x]\n", stk);
	printf("Object created in %s() from %s (%zu line)\n", stk->func, stk->file, stk->line);
	printf("Dump called in %s() from %s (%zu line)\n", func, file, line);
	printf("\n");
	printf("size = %zu\ncapacity = %zu\ndata[%x]\n", stk->size, stk->capacity, stk->data);
	printf("{\n");
	for (size_t i = 0; i < stk->capacity; i++) {
		if (stk->data[i] == Poison) {
			printf("*[%zu] = POISON\n", i);
			continue;
		}
		printf("*[%zu] = %d\n", i, stk->data[i]);
	}
	printf("}\n");
	printf("____________________________\n");
	return 0;
}
