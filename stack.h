#ifndef STACK_OL70 /*header gaurd*/
#define STACK_OL70
/*************************************************************
* The functions in this file handle a stack data structure.  *
*          													 *
*      														 *
* Written by: OL-70   										 *
* Last update: 09:00 , 29.06.2019                			 *
*************************************************************/
#include <stddef.h>

typedef struct stack_s stack_t;


stack_t *StackCreate(size_t num_of_elements , size_t element_size);
/********************************************************
*-Allocates the memory for the stack by calculating the  *
* required size by using num_of_elements and element_size*
* element_size is given in bytes.						 *
*-Uses malloc(). In order to free allocated memory,	 	 *
* StackDestroy must be called after work with the stack  *
* is done.											     *
*														 *
*-Return Value: A pointer to the stack.                  *
*														 *
*********************************************************/

void StackDestroy(stack_t *stack);
/*********************************************************
*-Releases the memory that has been allocated for the	 *
* stack													 *
*-Uses free().											 *
*  														 *
*														 *
*********************************************************/

void StackPush(stack_t *stack, const void *element);
/********************************************************
*-Pushes element to the top of the stack.				 *
* *element is a pointer to an element of any type to be	 *
* pushed according to stack type.						 *
*  														 *
*														 *
*********************************************************/

void StackPop(stack_t *stack);
/********************************************************
*-Releases the last element from the top of the stack.	 *
*  														 *
*														 *
*********************************************************/

void *StackPeek(stack_t *stack);
/********************************************************
*-Returns a pointer to the element on top of the stack   *
*  														 *
*  														 *
*********************************************************/

size_t StackSize(stack_t *stack);
/********************************************************
*-Returns the number of elements in the stack.  		 *
*  														 *
*														 *
*********************************************************/


#endif /*STACK_OL70*/
