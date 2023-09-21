#include "stack.h"
#define StackCtor(a, b) StackCtor(a, b, __FUNCTION__, __LINE__, __FILE__)
int main()
{
    //GetErrorInfo(StackVerify(nullptr));
    Stack stk = {};
    //GetErrorInfo(StackVerify(&stk));
    StackCtor(&stk, 3);
    StackPush(&stk, 102);
    StackPush(&stk, 103);
    StackPush(&stk, 104);
    StackPush(&stk, 105);
    Elem_t ret = 0;
    StackPop(&stk, &ret);
    //GetErrorInfo(StackVerify(&stk));
    int x = 0;
    StackPop(&stk, &x);
    //printf("%d", x);


}
