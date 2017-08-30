# include <stdlib.h>
# include <stdio.h>
# include <inttypes.h>
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
        printf("ERROR: Must specify input file with -i. Exiting...\n");
        exit(0);
    }
    int sFile = open(inName, O_RDONLY);
    if(sFile == -1)
    {
        perror(argv[0]);
        exit(errno);
    }

    int oFile;
  
    if(sFile == -1)
    {
        perror(argv[0]);
        exit(errno);
    }
  
    uint8_t buf[1000];
    uint32_t number;
    myRead(argv[0], sFile, buf, sizeof(number));
    number = (((uint64_t) buf[3]) << 24) + (((uint64_t) buf[2]) << 16) +
             (((uint64_t) buf[1]) <<  8) + (((uint64_t) buf[0]) << 0);
    // CHECKING MAGIC NUMBER
    if(number != 0xdeadd00d)
    {
        printf("File not encoded correctly. Exiting...");
        close(sFile);
        exit(0);
    }
  
    // GETTING ORIGINAL SIZE
    uint64_t exSize;
    myRead(argv[0], sFile, buf, sizeof(exSize));
    exSize = (((uint64_t) buf[7]) << 56) + (((uint64_t) buf[6]) << 48) +
             (((uint64_t) buf[5]) << 40) + (((uint64_t) buf[4]) << 32) +
             (((uint64_t) buf[3]) << 24) + (((uint64_t) buf[2]) << 16) +
             (((uint64_t) buf[1]) <<  8) + (((uint64_t) buf[0]) <<  0);
    /*if(verbose)
    {
        printf("Original file size was %ld\n", exSize);
    }*/
  
    // GETTING TREE SIZE
    uint16_t treeSize = 0;
    myRead(argv[0], sFile, buf, sizeof(treeSize));
    treeSize = (((uint16_t) buf[1]) << 8) + (uint16_t) buf[0];
    /*if(verbose)
    {
        printf("Tree size is %u bytes.\n", treeSize);
    }*/
  
    // LOADING TREE
    uint8_t savedTree[treeSize];
    if(myRead(argv[0], sFile, savedTree, (size_t)treeSize) != (ssize_t)treeSize)
    {
        printf("Error reading the tree. Exiting...\n");
        close(sFile);
        exit(0);
    }
    /*for(int i = treeSize - 6; i < treeSize; i++)
    {
        printf("%c", savedTree[i]);
    }
    printf("\n");*/
    treeNode *root = loadTree(savedTree, treeSize);
  
    bitV *inVector;
    if(exSize < 9223372036854775807 / 8)
    {
        inVector = newVec((exSize * 2) * 8);    // bytes * 8 == bits
    }
    else
    {
        inVector = newVec(exSize * 8);
    }
  
    // read in encoded file
    uint64_t compressedBytesIn = 0;
    size_t bytesRead;
    ssize_t numBytes = (ssize_t)(inVector->l / 8);
    if((bytesRead = myRead(argv[0], sFile, inVector->v, numBytes)))
    {
        compressedBytesIn += (uint64_t) bytesRead;
    } 
  
    close(sFile);
    if(outName == NIL)
    {
        oFile = STDOUT_FILENO;
    }
    else
    {
        int result = unlink(outName);
        if(result == -1 && errno == ENOENT && verbose)
        {
            //printf("File %s was not found, so it was created.\n", outName);
        }
        oFile = open(outName, O_WRONLY | O_CREAT,
                          S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH);
    }
  
    // use tree to decode rest of file into bit vector
    uint8_t out[exSize];
    uint64_t oIndex = 0;
    int32_t result;
    treeNode *node = root;
    uint64_t totalBits = compressedBytesIn * 8;
    for(uint64_t i = 0; i < totalBits && oIndex < exSize; i++)
    {
        result = stepTree(root, &node, valBit(inVector, i));
        if(result != -1)
        {
            out[oIndex] = result;
            oIndex++;
        }
    }
  
    // write bit vector to file
    myWrite(argv[0], oFile, out, exSize);
  
    if(verbose)
    {
        printf("%" PRIu64 " file bytes and %u tree bytes, decompressed to %" PRIu64 " bytes.\n",
                 compressedBytesIn, treeSize, oIndex);
    }
  
    delVec(inVector);
    delTree(root);
    close(oFile);
    return 0;
}

// read() with added error checking
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
