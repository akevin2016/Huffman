# include "queue.h"

queue *newQueue(uint32_t size)
{
    queue *q = malloc(sizeof(queue));
    q->size = size;
    q->head = 0;
    q->tail = 0;
    q->Q = calloc(size + 1, sizeof(item));
    return q;
}
void delQueue(queue *q)
{
    free(q->Q);
    free(q);
}


bool qEmpty(queue *q)
{
    return (q->head == q->tail);
}
bool qFull(queue *q)
{
    //printf("FULL DEBUG: head:%u, tail:%u, size:%u\n", q->head, q->tail,  q->size);
    return ((q->head + 1) % (q->size + 1) == q->tail);
}


bool enqueue(queue *q, item i)
{
    if(!qFull(q))
    {
        // PRIORITY QUEUE IF STATEMENT, CHANGE THIS FOR TREENODE
        /*if(!empty(q) && i < q->Q[q->tail])
        {
            //printf("Hello");
            q->tail = (q->tail + q->size) % (q->size + 1);
            q->Q[q->tail] = i;
            return true;
        }
        else
        {
            q->Q[q->head] = i;
            q->head = (q->head + 1) % (q->size + 1);
            return true;
        }*/
        for(uint32_t index = q->tail; index != q->head; index = (index+1) % (q->size+1))
        {
            //if(compare(&i, &(q->Q[index])) <= 0)
            if(i.count <= q->Q[index].count)
            {
                // shift all greater than or equal to i up 1 index
                for(uint32_t j = q->head; j != index; j = (j + q->size) % (q->size + 1))
                {
                    q->Q[j] = q->Q[(j + q->size) % (q->size + 1)];
                }
                q->Q[index] = i;
                q->head = (q->head + 1) % (q->size + 1);
                return true;
            }
        }
        q->Q[q->head] = i;
        q->head = (q->head + 1) % (q->size + 1);
        return true;
    }
    return false;
}
bool dequeue(queue *q, item *i)
{
    if(!qEmpty(q) && i != 0)
    {
        *i = q->Q[q->tail];
        //i->symbol = q->Q[q->tail].symbol;
        //i->leaf   = q->Q[q->tail].leaf;
        //i->count  = q->Q[q->tail].count;
        //i->left   = q->Q[q->tail].left;
        //i->right  = q->Q[q->tail].right;
    
        q->tail = (q->tail + 1) % (q->size + 1);
        return true;
    }
    return false;
}


