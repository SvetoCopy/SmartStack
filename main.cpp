#include "stack.h"
#include "hash.h"


int main()
{
    
    Stack stk = {};
    StackCtor(&stk, 12);
    StackPush(&stk, 2);
    
    StackPush(&stk, 3);
    int a = 0;
    StackPop(&stk, &a);
    StackPush(&stk, 3);
    StackDtor(&stk);


}
