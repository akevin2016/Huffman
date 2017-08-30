// bv.h — Bit Vector interface
// Author: Prof. Darrell Long

# ifndef _BVector
# define _BVector
# include <stdint.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>
# include "code.h"


typedef struct bitV {
	uint8_t *v;
	uint64_t l;
} bitV;

bitV *newVec(uint64_t);

void delVec(bitV *);

void oneVec(bitV *);

void setBit(bitV *, uint64_t);

void clrBit(bitV *, uint64_t);

uint8_t valBit(bitV *, uint64_t);

uint64_t lenVec(bitV *);

void appendCode(bitV *, code, uint64_t);
# endif
