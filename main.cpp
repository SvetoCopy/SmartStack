#include "stack.h"
#include "hash.h"

// StackLogger stk_logger{ fopen("test.txt") };

int main()
{
    // 
    // LogStack(&stk_logger, &stk);
    Stack stk = {};
    StackCtor(&stk, 12);

    StackPush(&stk, 2);
    stk.capacity = 3;
    StackPush(0, 3);
    int a = 0;
    StackPop(&stk, &a);
    StackPush(&stk, 3);
    StackDtor(&stk);


}
