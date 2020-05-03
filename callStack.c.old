#include <stdio.h>
#include <stdlib.h>
#include "callStack.h"

node *top = NULL;

void callStack_initialize()
{
	//printf("zzzzzzz");
    top = NULL;
}

//void callStack_push(functionData value)
void callStack_push()
{
    node *tmp;

    tmp = malloc(sizeof(node));
	if (tmp == NULL)
	{
		printf("malloc fail");
	}
	
    //tmp -> data = value;
    tmp -> next = top;
    if (top != NULL)
    {
		//printf("XXXXXXXXX");
        tmp -> nodeCount = top -> nodeCount + 1;
    }
    else
    {
		//printf("yyyyyyyyyyy");
        tmp -> nodeCount = 1;
    }
    
    top = tmp;
}

functionData callStack_pop()
{
    node *tmp;
    functionData n;
    tmp = top;
    n = tmp->data;
    top = top->next;
    free(tmp);
    return n;
}

functionData* callStack_top()
{
    return &(top->data);
}

int callStack_isEmpty()
{
    return top==NULL;
}

short callStack_Size()
{
    return top -> nodeCount;
}