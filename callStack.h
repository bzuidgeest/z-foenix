#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>

struct functionData
{
    short *locals;
    u_int16_t returnAddress;
    u_int16_t returnValueStorage;
    u_int16_t returnStackSize;
};
typedef struct functionData functionData;

struct node
{
    functionData data;
    struct node *next;
};
typedef struct node node;

node *top;

void callStack_initialize();
void callStack_push(functionData value);
functionData callStack_pop();
functionData callStack_top();
int callStack_isEmpty();
