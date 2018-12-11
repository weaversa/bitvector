/***************************************************

  Bitvector: A library for manipulating bitvectors

****************************************************/

#ifndef BITVECTOR_H
#define BITVECTOR_H

#define __STDC_FORMAT_MACROS

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#include "c_list_types/include/c_list_types.h"

create_c_list_headers(uint64_t_list, uint64_t)

typedef struct bitvector_t {
  uint32_t nBits;
  uint64_t_list bits;
} bitvector_t;

bitvector_t *bitvector_t_alloc(uint32_t nBits);
void bitvector_t_free(bitvector_t *bv);

//Unary operators
void bitvector_t_zeroize(bitvector_t *bv);
bitvector_t *bitvector_t_fromHexString(char *string, size_t length);
char *bitvector_t_toHexString(bitvector_t *bv);
bitvector_t *bitvector_t_copy(bitvector_t *bv);
void bitvector_t_widen(bitvector_t *bv, uint32_t nBitsToAdd);
void bitvector_t_negateUpdate(bitvector_t *bv);
bitvector_t *bitvector_t_negate(bitvector_t *bv);
uint8_t bitvector_t_getBit(bitvector_t *bv, uint32_t index);
void bitvector_t_setBit(bitvector_t *bv, uint32_t index, uint8_t value);
uint32_t bitvector_t_popcount(bitvector_t *bv);
  
//Binary Boolean operators
#define bitvector_t_zipWithIncludes(NAME)                        \
void bitvector_t_##NAME##Update(bitvector_t *x, bitvector_t *y); \
bitvector_t *bitvector_t_##NAME(bitvector_t *x, bitvector_t *y); \

bitvector_t_zipWithIncludes(xor)
bitvector_t_zipWithIncludes(or)
bitvector_t_zipWithIncludes(and)

bitvector_t *bitvector_t_concat(bitvector_t *x, bitvector_t *y);
void bitvector_t_dropUpdate(bitvector_t *bv, uint32_t nBitsToDrop);
bitvector_t *bitvector_t_drop(bitvector_t *bv, uint32_t nBitsToDrop);
uint8_t bitvector_t_equal(bitvector_t *x, bitvector_t *y);
void bitvector_t_sliceUpdate(bitvector_t *slice, bitvector_t *bv, uint32_t b0, uint32_t b1);
bitvector_t *bitvector_t_slice(bitvector_t *bv, uint32_t b0, uint32_t b1);

#endif
