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

#include "../lib/c_list_types/include/c_list_types.h"

#define BITS_TO_WORDS(nBits) ((uint32_t) ((((int32_t)nBits) - 1) >> 6) + 1)

create_c_list_headers(uint64_t_list, uint64_t)

typedef struct bitvector_t {
  uint32_t nBits;
  uint64_t_list bits;
} bitvector_t;

inline void bitvector_t_zeroize(bitvector_t *bv) {
  memset((void *)bv->bits.pList, 0, bv->bits.nLength * sizeof(uint64_t));
}

inline bitvector_t *bitvector_t_alloc(uint32_t nBits) {
  bitvector_t *bv = malloc(1 * sizeof(bitvector_t));
  bv->nBits = nBits;

  uint8_t ret = uint64_t_list_init(&bv->bits, BITS_TO_WORDS(bv->nBits));
  if(ret != C_LIST_NO_ERROR) return NULL;

  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);
  bitvector_t_zeroize(bv);
  return bv;
}

inline void bitvector_t_free(bitvector_t *bv) {
  if(bv == NULL) {
    fprintf(stderr, "Cannot free NULL bitvector_t\n");
    return;
  }
  uint64_t_list_free(&bv->bits, NULL);
  free(bv);
}

inline void bitvector_t_cleanHighBits(bitvector_t *bv) {
  bv->bits.pList[bv->bits.nLength-1] &= (~(uint64_t)0) >> (64 - (bv->nBits&0x3f));
}

inline void bitvector_t_widen(bitvector_t *bv, uint32_t nBitsToAdd) {
  if(nBitsToAdd + bv->nBits < bv->nBits) {
    fprintf(stderr, "Widening a vector with %u bits by %u bits caused an overflow\n", bv->nBits, nBitsToAdd);
    return;
  }
  
  bitvector_t_cleanHighBits(bv);

  bv->nBits += nBitsToAdd;
  uint32_t old_length = (uint32_t) bv->bits.nLength;
  uint8_t ret = uint64_t_list_resize(&bv->bits, BITS_TO_WORDS(bv->nBits));
  if(ret != C_LIST_NO_ERROR) return;
  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);

  memset((void *)(bv->bits.pList+old_length), 0, (bv->bits.nLength - old_length) * sizeof(uint64_t));
}

inline uint64_t hexchar_to_digit(char c) {
  uint64_t cu = (uint64_t) c;
  if(c >= 'a' && c <= 'f')      return cu - 97 + 10;
  else if(c >= 'A' && c <= 'F') return cu - 65 + 10;
  else if(c >= '0' && c <= '9') return cu - 48;
  else return 16;
}

inline bitvector_t *bitvector_t_fromHexString(char *string, uint32_t length) {
  uint32_t i;

  bitvector_t *bv = bitvector_t_alloc(length*4);
  if(bv == NULL) return NULL;

  for(i = 0; i < length; i++) {
    uint64_t digit = hexchar_to_digit(string[(length - i) - 1]);
    if(digit == 16) break;
    bv->bits.pList[i>>4] |= digit << ((i&0xf)*4);
  }

  if(i != length) {
    fprintf(stderr, "Malformed string. Cannot convert to bitvector.\n");
    bitvector_t_free(bv);
    return NULL;
  }

  return bv;
}

inline char *bitvector_t_toHexString(bitvector_t *bv) {
  uint32_t i;
  if(bv == NULL) return NULL;

  bitvector_t_cleanHighBits(bv);
  
  uint32_t length = bv->nBits/4 + ((bv->nBits%4) != 0);

  char *string = malloc(length + 1);
  string[length] = 0;

  for(i = 0; i < length; i++) {
    char c[2];
    snprintf(c, 2, "%" PRIx64, (bv->bits.pList[i>>4] >> ((i&0xf)*4)) & 0xf);
    string[length-i - 1] = c[0];
  }

  return string;
}

inline void bitvector_t_copyUpdate(bitvector_t *dst, bitvector_t *src) {
  if(uint64_t_list_copy(&dst->bits, &src->bits) != C_LIST_NO_ERROR) {
    fprintf(stderr, "bitvector_t copy failed.\n");
    return;
  }
}

inline bitvector_t *bitvector_t_copy(bitvector_t *bv) {
  bitvector_t *ret = bitvector_t_alloc(bv->nBits);
  bitvector_t_copyUpdate(ret, bv);
  return ret;
}

inline void bitvector_t_dropUpdate(bitvector_t *bv, uint32_t nBitsToDrop) {
  if(nBitsToDrop > bv->nBits) {
    fprintf(stderr, "Cannot drop %u bits from a bitvector_t with only %u bits.\n", nBitsToDrop, bv->nBits);
    return;
  }
  bv->nBits -= nBitsToDrop;
  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);
}

inline bitvector_t *bitvector_t_drop(bitvector_t *bv, uint32_t nBitsToDrop) {
  if(nBitsToDrop > bv->nBits) {
    fprintf(stderr, "Cannot drop %u bits from a bitvector_t with only %u bits.\n", nBitsToDrop, bv->nBits);
    return NULL;
  }
  uint32_t nLength_old = (uint32_t) bv->bits.nLength;
  uint32_t nBits_old = bv->nBits;
  bv->nBits -= nBitsToDrop;
  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);

  bitvector_t *ret = bitvector_t_copy(bv);

  bv->bits.nLength = nLength_old;
  bv->nBits = nBits_old;

  return ret;
}

inline void bitvector_from_bytesUpdate(bitvector_t *bv, uint8_t *bytes, uint32_t nBytes) {
  if(bv == NULL) return;
  if(bytes == NULL) return;
  
  if(bv->nBits < nBytes*8) {
    bitvector_t_widen(bv, (nBytes*8) - bv->nBits);
  } else if(bv->nBits > nBytes*8) {
    bitvector_t_dropUpdate(bv, bv->nBits - (nBytes*8));
  }

  uint32_t i;
  for(i = 0; i < nBytes; i++) {
    bv->bits.pList[i>>3] |= ((uint64_t) bytes[i])<<((i&0x7) * 8);
  }
}

inline bitvector_t *bitvector_from_bytes(uint8_t *bytes, uint32_t nBytes) {
  if(bytes == NULL) return NULL;
  bitvector_t *bv = bitvector_t_alloc(nBytes*8);
  bitvector_from_bytesUpdate(bv, bytes, nBytes);
  return bv;
}

inline void bitvector_to_bytesUpdate(uint8_t *bytes, bitvector_t *bv) {
  if(bv == NULL) return;
  if(bytes == NULL) return;
  if(bv->nBits % 8 != 0) {
    fprintf(stderr, "Cannot create a byte array from a bitvector_t with number of bits not a multiple of 8.\n");
    return;
  }

  uint32_t nBytes = bv->nBits / 8;
  uint32_t i;
  for(i = 0; i < nBytes; i++) {
    bytes[i] = (uint8_t) (bv->bits.pList[i>>3]>>((i&0x7) * 8));
  }
}

inline uint8_t *bitvector_to_bytes(bitvector_t *bv) {
  if(bv == NULL) return NULL;
  if(bv->nBits % 8 != 0) {
    fprintf(stderr, "Cannot create a byte array from a bitvector_t with number of bits not a multiple of 8.\n");
    return NULL;
  }
  
  uint8_t *bytes = (uint8_t *)malloc((bv->nBits / 8) * sizeof(uint8_t));
  bitvector_to_bytesUpdate(bytes, bv);
  return bytes;
}

inline bitvector_t *bitvector_t_concat(bitvector_t *x, bitvector_t *y) {
  uint32_t start = y->nBits >> 6;
  uint32_t length = (uint32_t) x->bits.nLength;
  uint32_t i;

  bitvector_t *ret = bitvector_t_copy(y);
  bitvector_t_cleanHighBits(ret);
  bitvector_t_widen(ret, x->nBits);

  for(i = start; i < start + length; i++) {
    ret->bits.pList[i] |= x->bits.pList[i-start] << (y->nBits&0x3f);
    if(i + 1 < ret->bits.nLength) {
      ret->bits.pList[i+1] = x->bits.pList[i-start] >> (64 - (y->nBits&0x3f));
    }
  }

  return ret;
}

inline bitvector_t *bitvector_t_join(bitvector_t **slice, uint32_t parts) {
  if(slice == NULL) return NULL;

  bitvector_t *ret = bitvector_t_copy(slice[0]);
  if(parts == 1) return ret;
  
  uint32_t i;
  for(i = 1; i < parts; i++) {
    bitvector_t *tmp = bitvector_t_concat(ret, slice[i]);
    bitvector_t_free(ret);
    ret = tmp;
  }

  return ret;
}

inline void bitvector_t_negateUpdate(bitvector_t *bv) {
  uint32_t i;
  for(i = 0; i < bv->bits.nLength; i++) {
    bv->bits.pList[i] = ~bv->bits.pList[i];
  }
}

inline bitvector_t *bitvector_t_negate(bitvector_t *bv) {
  bitvector_t *ret = bitvector_t_copy(bv);
  bitvector_t_negateUpdate(ret);
  return ret;
}

inline uint8_t bitvector_t_getBit(bitvector_t *bv, uint32_t index) {
  if(bv == NULL) return 0;
  if(index >= bv->nBits) {
    fprintf(stderr, "Cannot get bit %u in a bitvector_t with only %u bits.\n", index, bv->nBits);
    return 0;
  }

  return (uint8_t) ((uint64_t) 1) & (bv->bits.pList[index >> 6] >> (index & 0x3f));
}

inline void bitvector_t_setBit(bitvector_t *bv, uint32_t index, uint8_t value) {
  if(bv == NULL) return;
  if(index >= bv->nBits) {
    fprintf(stderr, "Cannot set bit %u in a bitvector_t with only %u bits.\n", index, bv->nBits);
    return;
  }

  if(value) {
    bv->bits.pList[index >> 6] |= ((uint64_t) 1) << (index & 0x3f);
  } else {
    bv->bits.pList[index >> 6] &= ~(((uint64_t) 1) << (index & 0x3f));
  }
}

inline uint32_t bitvector_t_popcount(bitvector_t *bv) {
  bitvector_t_cleanHighBits(bv);

  uint32_t i;
  uint32_t ret = 0;

  for(i = 0; i < bv->bits.nLength; i++) {
    ret += (uint32_t) __builtin_popcountll(bv->bits.pList[i]);
  }

  return ret;
}

inline void bitvector_t_sliceUpdate(bitvector_t *slice, bitvector_t *bv, uint32_t start, uint32_t length) {
  if(slice == NULL || bv == NULL) return;

  if(slice->nBits < length) {
    bitvector_t_widen(slice, length - slice->nBits);
  } else if(slice->nBits > length) {
    bitvector_t_dropUpdate(slice, slice->nBits - length);
  }

  if(bv->nBits < start + length) {
    fprintf(stderr, "Cannot take a slice of length %u starting at bit %u from a bitvector_t with only %u bits.\n", length, start, bv->nBits);
    return;
  }
  
  uint32_t startW  = start >> 6;
  uint32_t lengthW = (length >> 6) + 1;
  uint32_t i;
  for(i = 0; i < lengthW; i++) {
    slice->bits.pList[i] = bv->bits.pList[startW + i] >> (start&0x3f);
    if((startW + i + 1 < bv->bits.nLength) && ((start&0x3f) != 0)) {
      slice->bits.pList[i] |= bv->bits.pList[startW + i + 1] << (64 - (start&0x3f));
    }
  }
}

inline bitvector_t *bitvector_t_slice(bitvector_t *bv, uint32_t start, uint32_t length) {
  bitvector_t *slice = bitvector_t_alloc(length);
  bitvector_t_sliceUpdate(slice, bv, start, length);
  return slice;
}

inline bitvector_t **bitvector_t_split(bitvector_t *bv, uint32_t parts) {
  if(bv == NULL) return NULL;
  if((bv->nBits == 0) || (bv->nBits % parts != 0)) {
    fprintf(stderr, "Cannot split a bitvector_t of length %u into %u equal sized parts.\n", bv->nBits, parts);
    return NULL;
  }

  bitvector_t **slices = (bitvector_t **)malloc(parts * sizeof(bitvector_t *));

  uint32_t nBits = bv->nBits / parts;
    
  uint32_t i;
  for(i = 0; i < parts; i++) {
    slices[i] = bitvector_t_slice(bv, ((parts-1) - i)*nBits, nBits);
  }

  return slices;
}

inline uint8_t bitvector_t_equal(bitvector_t *x, bitvector_t *y) {
  uint32_t i;
  if(x == y) return 1;
  if(x == NULL || y == NULL) return 0;
  if(x->nBits != y->nBits) return 0;

  bitvector_t_cleanHighBits(x);
  bitvector_t_cleanHighBits(y);
  
  for(i = 0; i < x->bits.nLength; i++) {
    if(x->bits.pList[i] != y->bits.pList[i]) {
      return 0;
    }
  }

  return 1;
}

#define bitvector_t_zipWithIncludes(NAME, OP)                            \
inline void bitvector_t_##NAME##Update(bitvector_t *x, bitvector_t *y) { \
  uint32_t i;                                                            \
  if(x == NULL || y == NULL) return;                                     \
  if(x->nBits != y->nBits) {                                             \
    fprintf(stderr, "Cannot NAME two vectors of different sizes.\n");    \
    return;                                                              \
  }                                                                      \
  for(i = 0; i < x->bits.nLength; i++) {                                 \
    x->bits.pList[i] = x->bits.pList[i] OP y->bits.pList[i];             \
  }                                                                      \
}                                                                        \
                                                                         \
inline bitvector_t *bitvector_t_##NAME(bitvector_t *x, bitvector_t *y) { \
  if(x == NULL || y == NULL) return NULL;                                \
  if(x->nBits != y->nBits) {                                             \
    fprintf(stderr, "Cannot XOR two vectors of different sizes.\n");     \
    return NULL;                                                         \
  }                                                                      \
  bitvector_t *ret = bitvector_t_copy(x);                                \
  bitvector_t_##NAME##Update(ret, y);                                    \
  return ret;                                                            \
}                                                                        \

bitvector_t_zipWithIncludes(xor, ^)
bitvector_t_zipWithIncludes(or, |)
bitvector_t_zipWithIncludes(and, &)


#endif
