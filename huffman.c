# include "huffman.h"
# include <stdio.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>

# include "stack.h"

// New node, with symbols, leaf or not, a count associated with it
treeNode *newNode(uint8_t s, bool l, uint64_t c)
{
    treeNode *new = malloc(sizeof(treeNode));
    new->symbol = s;
    new->leaf = l;
    new->count = c;
    new->left = NIL;
    new->right = NIL;
    return new;
}

// Delete a single node
void delNode(treeNode *t)
{
    free(t);
}

// Delete a tree
void delTree(treeNode *t)
{
    if(t->left != NIL)
    {
        delTree(t->left);
    }
    if(t->right != NIL)
    {
        delTree(t->right);
    }
    delNode(t);
}

// Dump a Huffman tree onto a file
void dumpTree(treeNode *t, int file)
{
    uint8_t buf[2] = {0};
    size_t count = 0;
    ssize_t bytesWritten = 0;
    if(t->left != NIL)
    {
        dumpTree(t->left, file);
    }
    if(t->right != NIL)
    {
        dumpTree(t->right, file);
    }
    if(t->leaf)
    {
        buf[0] = 'L';
        buf[1] = t->symbol;
        count = 2;
        bytesWritten = write(file, buf, count);
        if(bytesWritten == -1)
        {
            perror("encode");
            exit(errno);
        }
    }
    else
    {
        buf[0] = 'I';
        count = 1;
        bytesWritten = write(file, buf, count);
        if(bytesWritten == -1)
        {
            perror("encode");
            exit(errno);
        }
    }
}

// Build a tree from the saved tree
treeNode *loadTree(uint8_t savedTree[], uint16_t treeBytes)
{
    stack *tStack = newStack();
    treeNode *second = NIL;
    treeNode *first = NIL;
    treeNode *joined = NIL;
    for(int i = 0; i < treeBytes; i++)
    {
        if(savedTree[i] == 'I')
        {
            second = newNode(0, false, 0);
            first = newNode(0, false, 0);
            *second = pop(tStack);
            *first = pop(tStack);
            joined = join(first, second);
      
            push(tStack, *joined);
            delNode(joined);  // becomes permanent when joined
        }
        else if(savedTree[i] == 'L')
        {
            i += 1;
            first = newNode(savedTree[i], true, 0);  // count not needed for decode
            push(tStack, *first);
            delNode(first);  // becomes permanent when joined
        }
        else
        {
            printf("Error in loadTree, neither I nor L.\n");
            exit(0);
        }
    }
    first = newNode(0, false, 0);
    *first = pop(tStack);        // ROOT
    delStack(tStack);
    return first;
}

// Step through a tree following the code
// code = 0 or 1
int32_t stepTree(treeNode *root, treeNode **t, uint32_t code)
{
    if(code == 1)
    {
        *t = (*t)->right;
    }
    else
    {
        *t = (*t)->left;
    }
    if((*t)->leaf)
    {
        int32_t retVal = (int32_t) ((*t)->symbol);
        *t = root;
        return retVal;
    }
    return -1;
}

// Parse a Huffman tree to build codes
void buildCode(treeNode *t, code s, code table[256])
{
    uint32_t k = 0;
    if(t->left != NIL)
    {
        k = 0;
        pushCode(&s, k);
        buildCode(t->left, s, table);
        popCode(&s, &k);
    }
    if(t->right != NIL)
    {
        k = 1;
        pushCode(&s, k);
        buildCode(t->right, s, table);
        popCode(&s, &k);
    }
    if(t->leaf)
    {
        table[t->symbol] = s;
    }
}

/*static inline void delNode(treeNode *h)
{
    free(h);
    return;
}*/

treeNode *join(treeNode *l, treeNode *r)  // Join two subtrees
{
    treeNode *new = newNode('$', false, l->count + r->count);
    new->left = l;
    new->right = r;
    return new;
}
