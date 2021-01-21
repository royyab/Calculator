/*******************************************************************

Name: 		Roy Yablonka
Reviewer:	Shlomo Solomont
Version:	Sent
Title:		Vector Functions File
*******************************************************************/

#include <stdlib.h> /*free*/
#include <string.h> /*memmove*/
#include <assert.h>
#include "stack.h"

struct stack_s
{
	size_t element_size;
	void *stack_end;
	void *stack_base;
	void *current_element;
};

/* creates stack datatype */
stack_t *StackCreate(size_t num_of_elements , size_t element_size)
{
	size_t stack_size = sizeof(stack_t) + num_of_elements * element_size;

	stack_t *stack = (stack_t *)malloc(stack_size);

	if (NULL == stack)
	{
		return NULL;
	}
	stack->element_size = element_size;
	stack->stack_base = (char *)stack + sizeof(stack_t);
	stack->stack_end = (char *)stack + stack_size;
	stack->current_element = stack->stack_base;

	return (stack);
}

/* releases stack datatype from memory */
void StackDestroy(stack_t *stack)
{
	assert(NULL != stack);

	free(stack);

	stack = NULL;
}

/* pushes value to top of stack */
void StackPush(stack_t *stack, const void *element)
{
	assert(NULL != stack);

	if (stack->current_element  < stack->stack_end)
	{
		memmove(stack->current_element, element, stack->element_size);
		stack->current_element = (char *)stack->current_element + stack->element_size;
	}
}

/* removes value of out top of stack */
void StackPop(stack_t *stack)
{
	assert(NULL != stack);

	if (stack->stack_base != stack->current_element)
	{
		stack->current_element = (char *)stack->current_element - stack->element_size;
	}
}

/* peeks at value at top of stack without removing it */
void *StackPeek(stack_t *stack)
{
	void *element_ptr = NULL;

	assert (NULL != stack);

	if (stack->stack_base != stack->current_element)
	{
		element_ptr = (char *)stack->current_element - stack->element_size;
		return (element_ptr);
	}
	else
	{
		return (NULL);
	}
}

/* returns stack current used size */
size_t StackSize(stack_t *stack)
{
	size_t size = 0;

	assert (NULL != stack);

	size = ((size_t)stack->current_element - (size_t)stack->stack_base) / (stack->element_size);

	return (size);
}
