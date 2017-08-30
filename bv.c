// bv.c — Bit Vector implementation

// PASSED ALL TESTS AND CFLAGS

# include <stdint.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>
# include "bv.h"

bitV *newVec(uint64_t length)
{
    uint64_t numBytes;
    for(numBytes = 0; numBytes*8 < length; numBytes++)
    {
        // no code needed in loop
    }
    bitV *bv = malloc(sizeof(bitV));
    bv->v = calloc(numBytes, sizeof(uint8_t));
    bv->l = length;
    return bv;
}

void delVec(bitV *bv)
{
    free(bv->v);
    free(bv);
}

void oneVec(bitV *bv)
{
    for(uint64_t i = 0; i < (bv->l / 8) + (bv->l%8>0? 1: 0); i++)
    {
        bv->v[i] |= 0xFF;
    }
}

void setBit(bitV *bv, uint64_t pos)
{
    uint64_t byteNum = pos / 8;
    uint64_t bitNum = pos % 8;
    //bv->v[byteNum] |= (0x1 << (7 - bitNum));
    bv->v[byteNum] |= (0x1 << (bitNum));
}

void clrBit(bitV *bv, uint64_t pos)
{
    uint64_t byteNum = pos / 8;
    uint64_t bitNum = pos % 8;
    //bv->v[byteNum] &= ~((0x1) << (7 - bitNum));
    bv->v[byteNum] &= ~((0x1) << (bitNum));
}

uint8_t valBit(bitV *bv, uint64_t pos)
{
    uint64_t byteNum = pos / 8;
    uint64_t bitNum = pos % 8;
    //return (uint8_t)(((0x1 << (7 - bitNum)) & bv->v[byteNum]) != 0);
    return (uint8_t)(((0x1 << (bitNum)) & bv->v[byteNum]) != 0);
}

uint64_t lenVec(bitV *bv)
{
    return bv->l;
}

// bitV *bv   : bitVector to append to
// code c     : bit code to append to bv
// uint64_t i : index to begin overwriting     
void appendCode(bitV *bv, code c, uint64_t i)
{
    uint32_t k = 0;
    uint64_t chomp = i + c.l;
    for(; chomp > i; chomp--)
    {
        popCode(&c, &k);
        if(k)
        {
            setBit(bv, chomp - 1);
        }
        else
        {
            clrBit(bv, chomp - 1);
        }
    }
}

