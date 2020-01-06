#include "stack.h"

// Utility function to initialize stack
struct stack* stack_new(int capacity)
{
	struct stack *pt = (struct stack*)malloc(sizeof(struct stack));

	pt->maxsize = capacity;
	pt->top = -1;
	pt->items = (short*)malloc(sizeof(short) * capacity);

	return pt;
}

// Utility function to return the size of the stack
int stack_size(struct stack *pt)
{
	return pt->top + 1;
}

// Utility function to check if the stack is empty or not
int stack_isEmpty(struct stack *pt)
{
	return pt->top == -1;	// or return size(pt) == 0;
}

// Utility function to check if the stack is full or not
int stack_isFull(struct stack *pt)
{
	return pt->top == pt->maxsize - 1;	// or return size(pt) == pt->maxsize;
}

// Utility function to add an element x in the stack
void stack_push(struct stack *pt, short x)
{
	// check if stack is already full. Then inserting an element would 
	// lead to stack overflow
	if (stack_isFull(pt))
	{
		printf("OverFlow\nProgram Terminated\n");
		exit(EXIT_FAILURE);
	}

	// add an element and increments the top index
	pt->items[++pt->top] = x;
}

// Utility function to return top element in a stack
short stack_peek(struct stack *pt)
{
	// check for empty stack
	if (!stack_isEmpty(pt))
		return pt->items[pt->top];
	else
		exit(EXIT_FAILURE);
}

// Utility function to pop top element from the stack
short stack_pop(struct stack *pt)
{
	// check for stack underflow
	if (stack_isEmpty(pt))
	{
		printf("UnderFlow\nProgram Terminated\n");
		exit(EXIT_FAILURE);
	}

	// decrement stack size by 1 and (optionally) return the popped element
	return pt->items[pt->top--];
}

/*
// main function
int main()
{
	// create a stack of capacity 5
	struct stack *pt = stack_new(5);

	push(pt, 1);
	push(pt, 2);
	push(pt, 3);

	printf("Top element is %d\n", stack_peek(pt));
	printf("Stack size is %d\n", stack_size(pt));

	stack_pop(pt);
	stack_pop(pt);
	stack_pop(pt);

	if (stack_isEmpty(pt))
		printf("Stack is empty");
	else
		printf("Stack is not empty");

	return 0;
}
*/
