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

bitvector_t *bitvector_t_fromHexString(char *string, size_t length) {
  size_t i;
  char sc[2] = {0, 0};

  bitvector_t *bv = bitvector_t_alloc(length*4);
  if(bv == NULL) return NULL;

  for(i = 0; i < length; i++) {
    if(isxdigit(string[length-i - 1])) {
      sc[0] = string[length-i - 1];
      uint64_t digit = strtoul(sc, NULL, 16);
      bv->bits.pList[i>>4] |= digit << ((i&0xf)*4);
    } else break;
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
    snprintf(c, 2, "%llx", (bv->bits.pList[i>>4] >> ((i&0xf)*4)) & 0xf);
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

void bitvector_t_widen(bitvector_t *bv, uint32_t nBits) {
  if(nBits < bv->nBits) {
    fprintf(stderr, "Cannot widen a bitvector_t of %d bits to %d bits.\n", bv->nBits, nBits);
    return;
  }
  bv->nBits = nBits;
  size_t length = bv->bits.nLength;
  uint64_t_list_resize(&bv->bits, BITS_TO_WORDS(nBits));
  bv->bits.nLength = BITS_TO_WORDS(bv->nBits);

  memset((void *)(bv->bits.pList+length), 0, (bv->bits.nLength - length) * sizeof(uint64_t));
}

bitvector_t *bitvector_t_concat(bitvector_t *x, bitvector_t *y) {
  size_t start = y->nBits >> 6;
  size_t length = x->bits.nLength;
  size_t i;

  bitvector_t_cleanHighBits(y);
  bitvector_t *ret = bitvector_t_copy(y);
  bitvector_t_widen(ret, x->nBits + y->nBits);

  for(i = start; i < start + length; i++) {
    ret->bits.pList[i] |= x->bits.pList[i-start] << (y->nBits&0x3f);
    if(i+1 <= start+length) {
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

void bitvector_t_takeUpdate(bitvector_t *bv, uint32_t nBits) {
  if(nBits > bv->nBits) {
    fprintf(stderr, "Cannot take %u bits from a bitvector_t with only %u bits.\n", nBits, bv->nBits);
    return;
  }
  bv->nBits = nBits;
  bv->bits.nLength = BITS_TO_WORDS(nBits);
}

bitvector_t *bitvector_t_take(bitvector_t *bv, uint32_t nBits) {
  if(nBits > bv->nBits) {
    fprintf(stderr, "Cannot take %u bits from a bitvector_t with only %u bits.\n", nBits, bv->nBits);
    return NULL;
  }
  size_t nLength_old = bv->bits.nLength;
  uint32_t nBits_old = bv->nBits;
  bv->bits.nLength = BITS_TO_WORDS(nBits);
  bv->nBits = nBits;

  bitvector_t *ret = bitvector_t_copy(bv);

  bv->bits.nLength = nLength_old;
  bv->nBits = nBits_old;

  return ret;
}

void bitvector_t_setBit(bitvector_t *bv, uint32_t index) {
  if(bv == NULL) return;
  if(index >= bv->nBits) {
    fprintf(stderr, "Cannot set bit %u in a bitvector_t with only %u bits.\n", index, bv->nBits);
    return;
  }

  bv->bits.pList[index >> 6] |= ((uint64_t) 1) << (index & 0x3f);
}

void bitvector_t_unsetBit(bitvector_t *bv, uint32_t index) {
  if(bv == NULL) return;
  if(index >= bv->nBits) {
    fprintf(stderr, "Cannot unset bit %u in a bitvector_t with only %u bits.\n", index, bv->nBits);
    return;
  }

  bv->bits.pList[index >> 6] &= ~(((uint64_t) 1) << (index & 0x3f));
}

//take a Slice?
void bitvector_t_sliceUpdate(bitvector_t *slice, bitvector_t *x, uint32_t b0, uint32_t b1) {
  if(slice == NULL || x == NULL) return;
  if(b0 > b1) {
    fprintf(stderr, "End of slice is before the beginning.\n");
    return;
  }

  if(slice->nBits < (b1-b0)) {
    bitvector_t_widen(slice, b1-b0);
  } else if(slice->nBits > (b1-b0)) {
    bitvector_t_takeUpdate(slice, b1-b0);
  }

  if(x->nBits < (b1-b0)) {
    fprintf(stderr, "Cannot take a slice of size %u from a bitvector_t with only %u bits.\n", b1-b0, x->nBits);
    return;
  }
  
  size_t start  = b0 >> 6;
  size_t length = (b1-b0) >> 6;
  size_t *i;
  for(i = 0; i < length; i++) {
    slice->bits.pList[i] = x->bits.pList[start + i] >> (b0&0x3f);
    if(start + i + 1 < ) { //???
      ret->bits.pList[i] |= x->bits.pList[start + i + 1] << (64 - (b&0x3f)); //???
    }
  }  

}

//popcount?

//peek? getBit?

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

bitvector_t_zipWithUpdate(equ, ==)
bitvector_t_zipWith(equ, ==)

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
