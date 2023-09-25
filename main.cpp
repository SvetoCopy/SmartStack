#include "stack.h"
#include "hash.h"


int main()
{
    
    Stack stk = {};
    StackCtor(&stk, 12);
    
    StackPush(&stk, 1);
    StackPush(&stk, 2);
    stk.data[0] = 3;
    StackPush(&stk, 3);

    StackPush(&stk, 3);
    StackDtor(&stk);


}
