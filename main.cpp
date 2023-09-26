#include "stack.h"
#include "hash.h"


int main()
{
    
    Stack stk = {};
    // stk.capacity = 0;
    StackCtor(&stk, 12);
    
    StackPush(&stk, 1);
    StackPush(&stk, 2);
    StackPush(&stk, 3);
    stk.capacity = 5;
    StackPush(&stk, 3);
    printf("%d", sizeof(stk));
    StackDtor(&stk);


}
