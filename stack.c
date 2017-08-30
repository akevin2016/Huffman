# include "stack.h"

// PASSED ALL TESTS AND CFLAGS

stack *newStack()
{
    stack *s = malloc(sizeof(stack));
    s->size = 1;
    s->top = 0;               // points to space above top-most element
    s->entries = calloc(1 + 1, sizeof(item));  // +1 for top
    return s;
}
void delStack(stack *s)
{
    free(s->entries);
    free(s);
}


item pop(stack *s)
{
    if(!empty(s))
    {
        s->top--;
        item toPop = s->entries[s->top];
        return toPop;
    }
    item *invalid = newNode(0, false, 0);
    return *invalid;
    // following line for uint32_t
    //return INVALID;
}
void push(stack *s, item new)
{
    if(full(s))
    {
        item *ent = s->entries;
        s->entries = realloc(s->entries, sizeof(item) * ((2*(s->size)) + 1));
        s->size = 2 * (s->size);
    
        if(!(s->entries))
        {
            printf("Error on realloc {s->entries} [%d: %s]\n", errno, strerror(errno));
            free(s->entries);
            s->entries = ent;
            exit(errno);
        }
    }
    s->entries[s->top] = new;
    s->top++;
}


bool empty(stack *s)
{
    return (s->top == 0);
}
bool full(stack *s)
{
    return (s->top == s->size);
}
