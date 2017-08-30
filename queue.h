# ifndef _QUEUE_H
# define _QUEUE_H

# include <stdint.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>

# include "huffman.h"

typedef treeNode item;  // defined in huffman.h

typedef struct queue
{
    uint32_t size;
    uint32_t head, tail;
    item *Q;
} queue;

queue *newQueue(uint32_t);
void delQueue(queue *);

bool qEmpty(queue *);
bool qFull(queue *);

bool enqueue(queue *, item);
bool dequeue(queue *, item *);

# endif


