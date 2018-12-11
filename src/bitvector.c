#include "bitvector.h"

create_c_list_type(uint64_t_list, uint64_t)

#define BITS_TO_WORDS(nBits) ((uint32_t) ((((int32_t)nBits) - 1) >> 6) + 1)

bitvector_t *bitvector_t_alloc(uint32_t nBits) {
  bitvector_t *bv = malloc(1 * sizeof(bitvector_t));
  bv->nBits = nBits;

  uint8_t ret = uint64_t_list_init(&bv->bits, BITS_TO_WORDS(bv->nBits), 0);
  if(ret != NO_ERROR) return NULL;

  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);
  bitvector_t_zeroize(bv);
  return bv;
}

void bitvector_t_free(bitvector_t *bv) {
  uint64_t_list_free(&bv->bits, NULL);
  free(bv);
}

void bitvector_t_zeroize(bitvector_t *bv) {
  memset((void *)bv->bits.pList, 0, bv->bits.nLength * sizeof(uint64_t));
}

void bitvector_t_cleanHighBits(bitvector_t *bv) {
  bv->bits.pList[bv->bits.nLength-1] &= (~(uint64_t)0) >> (64 - (bv->nBits&0x3f));
}

uint64_t hexchar_to_digit(char c) {
  if(c >= 'a' && c <= 'f')      return (c - 'a') + 10;
  else if(c >= 'A' && c <= 'F') return (c - 'A') + 10;
  else if(c >= '0' && c <= '9') return c - '0';
  else return 16;
}

bitvector_t *bitvector_t_fromHexString(char *string, size_t length) {
  size_t i;

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

char *bitvector_t_toHexString(bitvector_t *bv) {
  size_t i;
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

bitvector_t *bitvector_t_copy(bitvector_t *bv) {
  bitvector_t *ret = bitvector_t_alloc(bv->nBits);
  if(uint64_t_list_copy(&ret->bits, &bv->bits) != NO_ERROR) {
    fprintf(stderr, "bitvector_t copy failed.\n");
    bitvector_t_free(ret);
    return NULL;
  }
  return ret;
}

void bitvector_t_widen(bitvector_t *bv, uint32_t nBitsToAdd) {
  if(nBitsToAdd + bv->nBits < bv->nBits) {
    fprintf(stderr, "Widening a vector with %u bits by %u bits caused an overflow\n", bv->nBits, nBitsToAdd);
    return;
  }
  
  bitvector_t_cleanHighBits(bv);

  bv->nBits += nBitsToAdd;
  size_t old_length = bv->bits.nLength;
  uint8_t ret = uint64_t_list_resize(&bv->bits, BITS_TO_WORDS(bv->nBits));
  if(ret != NO_ERROR) return;
  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);

  memset((void *)(bv->bits.pList+old_length), 0, (bv->bits.nLength - old_length) * sizeof(uint64_t));
}

bitvector_t *bitvector_t_concat(bitvector_t *x, bitvector_t *y) {
  size_t start = y->nBits >> 6;
  size_t length = x->bits.nLength;
  size_t i;

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

void bitvector_t_negateUpdate(bitvector_t *bv) {
  size_t i;
  for(i = 0; i < bv->bits.nLength; i++) {
    bv->bits.pList[i] = ~bv->bits.pList[i];
  }
}

bitvector_t *bitvector_t_negate(bitvector_t *bv) {
  bitvector_t *ret = bitvector_t_copy(bv);
  bitvector_t_negateUpdate(ret);
  return ret;
}

void bitvector_t_dropUpdate(bitvector_t *bv, uint32_t nBitsToDrop) {
  if(nBitsToDrop > bv->nBits) {
    fprintf(stderr, "Cannot drop %u bits from a bitvector_t with only %u bits.\n", nBitsToDrop, bv->nBits);
    return;
  }
  bv->nBits -= nBitsToDrop;
  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);
}

bitvector_t *bitvector_t_drop(bitvector_t *bv, uint32_t nBitsToDrop) {
  if(nBitsToDrop > bv->nBits) {
    fprintf(stderr, "Cannot drop %u bits from a bitvector_t with only %u bits.\n", nBitsToDrop, bv->nBits);
    return NULL;
  }
  size_t nLength_old = bv->bits.nLength;
  uint32_t nBits_old = bv->nBits;
  bv->nBits -= nBitsToDrop;
  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);

  bitvector_t *ret = bitvector_t_copy(bv);

  bv->bits.nLength = nLength_old;
  bv->nBits = nBits_old;

  return ret;
}

uint8_t bitvector_t_getBit(bitvector_t *bv, uint32_t index) {
  if(bv == NULL) return 0;
  if(index >= bv->nBits) {
    fprintf(stderr, "Cannot get bit %u in a bitvector_t with only %u bits.\n", index, bv->nBits);
    return 0;
  }

  return (uint8_t) ((uint64_t) 1) & (bv->bits.pList[index >> 6] >> (index & 0x3f));
}

void bitvector_t_setBit(bitvector_t *bv, uint32_t index, uint8_t value) {
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

uint32_t bitvector_t_popcount(bitvector_t *bv) {
  bitvector_t_cleanHighBits(bv);

  size_t i;
  uint32_t ret = 0;

  for(i = 0; i < bv->bits.nLength; i++) {
    ret += __builtin_popcountll(bv->bits.pList[i]);
  }

  return ret;
}

void bitvector_t_sliceUpdate(bitvector_t *slice, bitvector_t *bv, uint32_t b0, uint32_t b1) {
  if(slice == NULL || bv == NULL) return;
  if(b0 > b1) {
    fprintf(stderr, "End of slice is before the beginning.\n");
    return;
  }

  if(slice->nBits < (b1-b0)) {
    bitvector_t_widen(slice, (b1-b0) - slice->nBits);
  } else if(slice->nBits > (b1-b0)) {
    bitvector_t_dropUpdate(slice, slice->nBits - (b1-b0));
  }

  if(bv->nBits < (b1-b0)) {
    fprintf(stderr, "Cannot take a slice of size %u from a bitvector_t with only %u bits.\n", b1-b0, bv->nBits);
    return;
  }
  
  size_t start  = b0 >> 6;
  size_t length = ((b1-b0) >> 6) + 1;
  size_t i;
  for(i = 0; i < length; i++) {
    slice->bits.pList[i] = bv->bits.pList[start + i] >> (b0&0x3f);
    if((start + i + 1 < bv->bits.nLength) && ((b0&0x3f) != 0)) {
      slice->bits.pList[i] |= bv->bits.pList[start + i + 1] << (64 - (b0&0x3f));
    }
  }
}

bitvector_t *bitvector_t_slice(bitvector_t *bv, uint32_t b0, uint32_t b1) {
  bitvector_t *slice = bitvector_t_alloc(b1-b0);
  bitvector_t_sliceUpdate(slice, bv, b0, b1);
  return slice;
}

#define bitvector_t_zipWithUpdate(NAME, OP)                           \
void bitvector_t_##NAME##Update(bitvector_t *x, bitvector_t *y) {     \
  size_t i;                                                           \
  if(x == NULL || y == NULL) return;                                  \
  if(x->nBits != y->nBits) {                                          \
    fprintf(stderr, "Cannot NAME two vectors of different sizes.\n"); \
    return;                                                           \
  }                                                                   \
  for(i = 0; i < x->bits.nLength; i++) {                              \
    x->bits.pList[i] = x->bits.pList[i] OP y->bits.pList[i];          \
  }                                                                   \
}                                                                     \

#define bitvector_t_zipWith(NAME, OP)                                 \
bitvector_t *bitvector_t_##NAME(bitvector_t *x, bitvector_t *y) {     \
  if(x == NULL || y == NULL) return NULL;                             \
  if(x->nBits != y->nBits) {                                          \
    fprintf(stderr, "Cannot XOR two vectors of different sizes.\n");  \
    return NULL;                                                      \
  }                                                                   \
  bitvector_t *ret = bitvector_t_copy(x);                             \
  bitvector_t_##NAME##Update(ret, y);                                 \
  return ret;                                                         \
}                                                                     \

bitvector_t_zipWithUpdate(xor, ^)
bitvector_t_zipWith(xor, ^)

bitvector_t_zipWithUpdate(or, |)
bitvector_t_zipWith(or, |)

bitvector_t_zipWithUpdate(and, &)
bitvector_t_zipWith(and, &)

uint8_t bitvector_t_equal(bitvector_t *x, bitvector_t *y) {
  size_t i;
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
