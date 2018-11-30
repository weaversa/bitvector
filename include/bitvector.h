/***************************************************

  Bitvector: A library for manipulating bitvectors

****************************************************/

#ifndef BITVECTOR_H
#define BITVECTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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
void bitvector_t_widen(bitvector_t *bv, uint32_t nBits);
void bitvector_t_negateUpdate(bitvector_t *bv);
bitvector_t *bitvector_t_negate(bitvector_t *bv);
void bitvector_t_setBit(bitvector_t *bv, uint32_t index);
void bitvector_t_unsetBit(bitvector_t *bv, uint32_t index);

//Binary Boolean operators
#define bitvector_t_zipWithIncludes(NAME)                        \
void bitvector_t_##NAME##Update(bitvector_t *x, bitvector_t *y); \
bitvector_t *bitvector_t_##NAME(bitvector_t *x, bitvector_t *y); \

bitvector_t_zipWithIncludes(xor)
bitvector_t_zipWithIncludes(equ)
bitvector_t_zipWithIncludes(or)
bitvector_t_zipWithIncludes(and)

bitvector_t *bitvector_t_concat(bitvector_t *x, bitvector_t *y);
void bitvector_t_takeUpdate(bitvector_t *bv, uint32_t nBits);
bitvector_t *bitvector_t_take(bitvector_t *bv, uint32_t nBits);

uint8_t bitvector_t_equal(bitvector_t *x, bitvector_t *y);

#endif
