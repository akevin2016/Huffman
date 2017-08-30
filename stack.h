# ifndef _STACK_H
# define _STACK_H

# ifndef NIL 
# define NIL (void *) 0
# endif

# ifndef INVALID
# define INVALID 0xDeadD00d
# endif

# include <stdlib.h>
# include <stdint.h>
# include <stdbool.h>
# include <errno.h>
# include <string.h>
# include <stdio.h>

# include "huffman.h"

typedef treeNode item;

typedef struct stack
{
  uint32_t size;
  uint32_t top;
  item *entries;
} stack;

stack *newStack();           // Constructor
void delStack(stack *);      // Deconstructor

item pop(stack *);           // Returns the top item
void push(stack *, item);    // Adds an item to the top

bool empty(stack *);         // Is it empty?
bool full(stack *);          // Is it full?

# endif

