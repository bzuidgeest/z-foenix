#include <stdio.h>
#include <stdlib.h>
#include "callStack.h"

node *top;

void callStack_initialize()
{
    top = NULL;
}

void callStack_push(functionData value)
{
    node *tmp;
    tmp = malloc(sizeof(node));
    tmp -> data = value;
    tmp -> next = top;
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

functionData callStack_top()
{
    return top->data;
}

int callStack_isEmpty()
{
    return top==NULL;
}

