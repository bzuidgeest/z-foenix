#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>



struct functionData
{
    short locals[16];
    unsigned short returnAddress;
    unsigned short returnValueStorage;
    unsigned short returnStackSize;

};
typedef struct functionData functionData;

struct node
{
    functionData data;
    struct node *next;
    short nodeCount;
};
typedef struct node node;

extern node *top;

void callStack_initialize();
void callStack_push();
functionData callStack_pop();
functionData* callStack_top();
int callStack_isEmpty();

short callStack_Size(void);
