#include <stdio.h>
#include <stdlib.h>
#include "callStack.h"

//node *top = NULL;
functionData callstack[25]; 
short callstackIndex = -1;

void callStack_initialize()
{
}

//void callStack_push(functionData value)
void callStack_push()
{
	callstackIndex++;
	if (callstackIndex > 24 )
	{
		printf("Stack overflow");
	}

}

functionData callStack_pop()
{
	functionData data = callstack[callstackIndex];
	if (callstackIndex < 0 )
	{
		printf("Stack underflow");
	}
	callstackIndex--;
	return data;
}

functionData* callStack_top()
{
    return callstack + callstackIndex;
}

int callStack_isEmpty()
{
    return (callstackIndex == -1);
}

short callStack_Size()
{
    return callstackIndex + 1;
}