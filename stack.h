#include <stdio.h>
#include <stdlib.h>

//https://www.techiedelight.com/stack-implementation/

// Data structure for stack
struct stack
{
	int maxsize;	// define max capacity of stack
	int top;		
	short *items;
};


// Utility function to initialize stack
struct stack* stack_new(int capacity);

// Utility function to return the size of the stack
int stack_size(struct stack *pt);

// Utility function to check if the stack is empty or not
int stack_isEmpty(struct stack *pt);

// Utility function to check if the stack is full or not
int stack_isFull(struct stack *pt);

// Utility function to add an element x in the stack
void stack_push(struct stack *pt, short x);

// Utility function to return top element in a stack
short stack_peek(struct stack *pt);

// Utility function to pop top element from the stack
short stack_pop(struct stack *pt);
