# include <stdlib.h>
# include <stdio.h>
# include <inttypes.h>
# include <ctype.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

# include "stack.h"
# include "queue.h"
# include "code.h"
# include "bv.h"
# include "huffman.h"

ssize_t myRead(char *, int, uint8_t *, size_t);
ssize_t myWrite(char *, int, uint8_t *, size_t);

void printTree(treeNode *, int);

int main(int argc, char **argv)
{
    char *inName = NIL;
    char *outName = NIL;
    char c = 0;
    bool verbose = false;
    while((c = getopt(argc, argv, "i:o:v")) != -1)
    {
        switch(c)
        {
            case 'i':
                inName = optarg;
                break;
            case 'o':
                outName = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            default:
                printf("Unsupported option. Exiting...\n");
                exit(0);
        }
    }
    if(inName == NIL)
    {
        // Exit if no input file specified
        printf("ERROR: Must specify input file with -i. Exiting...\n");
        exit(1);
    }
    int sFile = open(inName, O_RDONLY);
    if(sFile == -1)
    {
        perror(argv[0]);
        exit(errno);
    }
  
    int oFile;
  
    uint64_t histogram[256] = {0};
  
    // add one 0x00 and one 0xFF to avoid empty file complications
    histogram[0]++;
    histogram[255]++;
  
    uint64_t totalBytes = 0;
    ssize_t bytesRead;
    uint8_t buf[1000];
    while((bytesRead = myRead(argv[0], sFile, buf, 1000)) > 0)
    {
        totalBytes += (uint64_t)bytesRead;
    
        // For each byte, add one to the counter histogram
        for(int i = 0; ((ssize_t) i) < bytesRead; i++)
        {
            //printf("%d\n", buf[i]);
            histogram[buf[i]] += 1;
        }
    }
  
    queue *q = newQueue(256);
    treeNode *n;               // Used to create new nodes and enqueue by value
    uint16_t leafCount = 0;    // Leaf Node count
    for(uint16_t i = 0; i <= 255; i++)
    {
        if(histogram[i] > 0)
        {
            n = newNode((uint8_t) i, true, histogram[i]);
            bool success = enqueue(q, *n);      // pass by value
            delNode(n);
            n = NIL;
            if(!success)
            {
                printf("Enqueue should have worked but did not.\n");
            }
            leafCount += 1;
        }
    }
    
    treeNode *first = NIL;
    treeNode *second = NIL;
    treeNode *joined = NIL;
  
    while(q->head != q->tail + 1)
    {
    
        first = newNode(0, false, 0);
        second = newNode(0, false, 0);

        dequeue(q, first);
        dequeue(q, second);
    
        joined = join(first, second);       // will be dequeued later, need to free
    
        if(!enqueue(q, *joined))
        {
            printf("Joined node failed to enqueue.\n");
            return -1;
        }
        delNode(joined);
    }  // Although their names disappear, the nodes remain
  
    // ROOT IS THE LAST NODE IN QUEUE
    treeNode *root = newNode(0, false, 0);
  
    if(!dequeue(q, root))
    {
        printf("Perceived root failed to dequeue.\n");
    }
  
    code table[256];
    for(uint16_t i = 0; i < 256; i++)
    {
        table[i] = newCode();
    }
    code s = newCode();
  
    buildCode(root, s, table);
   
    // move back to start of source file
    off_t position = lseek(sFile, 0, SEEK_SET);
    if(position == -1)
    {
        perror(argv[0]);
        exit(errno);
    }
  
    // prepare bit vector for output
    bitV *outVector;
    if(totalBytes < 9223372036854775807 / 64)
    {
        outVector = newVec((totalBytes * 8) * 8);    // bytes * 8 == bits
    }
    else
    {
        outVector = newVec(totalBytes * 8);
    }
    uint64_t vecIndex = 0;
    while((bytesRead = myRead(argv[0], sFile, buf, 1000)))
    {
        for(uint64_t i = 0; i < (uint64_t) bytesRead; i++)
        {
            appendCode(outVector, table[buf[i]], vecIndex);
            vecIndex += table[buf[i]].l;
        } 
    } 
  
    int closed = close(sFile);
    if(closed == -1)
    {
        perror(argv[0]);
        exit(errno);
    }
  
    // -------------------
    // BEGINNING OF OUTPUT
    // -------------------
  
    if(outName == NIL)
    {
        oFile = STDOUT_FILENO;
    }
    else
    {
        int result = unlink(outName);
        if(result == -1 && errno != ENOENT)
        {
            printf("Error in unlinking file with same name as output file.\n");
            printf("Defaulting to standard output, this is a bug.\n");
            oFile = STDOUT_FILENO;
        }
        else
        {
            oFile = open(outName, O_WRONLY | O_CREAT,
                          S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        }
    }
    if(sFile == -1)
    {
        perror(argv[0]);
        exit(errno);
    }
  
    // MAGIC NUMBER
    uint32_t magicNumber = 0xdeadd00d;
    buf[3] = 0xde;
    buf[2] = 0xad;
    buf[1] = 0xd0;
    buf[0] = 0x0d;
    ssize_t bytesWritten;
    bytesWritten = myWrite(argv[0], oFile, buf, sizeof(magicNumber));
  
    // SIZE OF INPUT FILE
    uint64_t mask = 0x00000000000000FF;
    uint64_t tB = totalBytes;
    for(int i = 0; i <= 7; i++)
    {
        buf[i] = (uint8_t)(mask & tB);
        tB = tB >> 8;
    }
    bytesWritten = myWrite(argv[0], oFile, buf, sizeof(totalBytes));
  
    // SIZE OF TREE
    uint16_t sizeOfTree = (3 * leafCount) - 1;
    uint16_t sot = sizeOfTree;          // store for later
    for(int i = 0; i <= 1; i++)
    {
        buf[i] = (uint8_t)(mask & sizeOfTree);
        sizeOfTree = sizeOfTree >> 8;
    }
    bytesWritten = myWrite(argv[0], oFile, buf, sizeof(sizeOfTree));
    //bytesWritten = myWrite(argv[0], oFile, &sizeOfTree, sizeof(sizeOfTree));
  
    dumpTree(root, oFile);
  
  
    // write bit vector to file
    size_t bytesToWrite = (size_t) ((vecIndex / 8) + (vecIndex % 8 ? 1 : 0));
    bytesWritten = myWrite(argv[0], oFile, outVector->v, bytesToWrite);
  
    if(verbose)
    {
        printf("%" PRIu64 " bytes input, compressed to %u tree bytes and %zd file bytes.\n", totalBytes, sot, bytesWritten); 
    }
  
  
    delVec(outVector);
    delTree(root);
    delNode(n);
    delQueue(q);
  
    closed = close(oFile);
    if(closed == -1)
    {
        perror(argv[0]);
        exit(errno);
    }
    return 0;
}

ssize_t myRead(char *name, int fd, uint8_t *buffer, size_t bytesToRead)
{
    ssize_t bytesRead = 0;
    bytesRead = read(fd, buffer, bytesToRead);
    if(bytesRead == -1)
    {
        perror(name);
        exit(errno);
    }
    return bytesRead;
}

// write() with added error checking
ssize_t myWrite(char *name, int fd, uint8_t *buffer, size_t bytesToWrite)
{
    ssize_t bytesWritten = 0;
    bytesWritten = write(fd, buffer, bytesToWrite);
    if(bytesWritten == -1)
    {
        perror(name);
        exit(errno);
    }
    return bytesWritten;
}


// SPACES() AND PRINTTREE() from DDEL
static inline void spaces(int c) { for (int i = 0; i < c; i += 1) { putchar(' '); } return; }

void printTree(treeNode *t, int depth)
{
    if (t) 
    {
        printTree(t->left, depth + 1);

        if (t->leaf)
        {
            if (isalnum(t->symbol))
            {
                spaces(4 * depth); printf("'%c' (%" PRIu64 ")\n", t->symbol, t->count);
            }
            else
            {
            spaces(4 * depth); printf("0x%X (%" PRIu64 ")\n", t->symbol, t->count);
            }
        }
        else
        {
            spaces(4 * depth); printf("$ (%" PRIu64 ")\n", t->count);
        }

        printTree(t->right, depth + 1); 
    }

    return;
}
