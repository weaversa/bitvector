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
void bitvector_t_negate_update(bitvector_t *bv);
bitvector_t *bitvector_t_negate(bitvector_t *bv);


//Binary Boolean operators
#define bitvector_t_zipWith_includes(NAME)                        \
void bitvector_t_##NAME##_update(bitvector_t *x, bitvector_t *y); \
bitvector_t *bitvector_t_##NAME(bitvector_t *x, bitvector_t *y);  \

bitvector_t_zipWith_includes(xor)
bitvector_t_zipWith_includes(equ)
bitvector_t_zipWith_includes(or)
bitvector_t_zipWith_includes(and)

bitvector_t *bitvector_t_concat(bitvector_t *x, bitvector_t *y);
void bitvector_t_take_update(bitvector_t *bv, uint32_t nBits);
bitvector_t *bitvector_t_take(bitvector_t *bv, uint32_t nBits);

uint8_t bitvector_t_equal(bitvector_t *x, bitvector_t *y);

#endif
