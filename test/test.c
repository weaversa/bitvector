#include "bitvector.h"

int main() {

  char *tmp;
  
  bitvector_t *x = bitvector_t_fromHexString("ABCDABCDABCDABCD0123", 20);
  bitvector_t *y = bitvector_t_fromHexString("0123", 4);

  fprintf(stderr, "x : [%d] = %s\n", x->nBits, tmp=bitvector_t_toHexString(x)); free(tmp);
  fprintf(stderr, "y : [%d] = %s\n", y->nBits, tmp=bitvector_t_toHexString(y)); free(tmp);

  bitvector_t *z = bitvector_t_concat(x, y);
  fprintf(stderr, "z : [%d] = x # y = %s\n", z->nBits, tmp=bitvector_t_toHexString(z)); free(tmp);

  bitvector_t_widen(z, 121);
  fprintf(stderr, "z : [%d] = %s\n", z->nBits, tmp=bitvector_t_toHexString(z)); free(tmp);

  bitvector_t *z2 = bitvector_t_concat(z, z);

  fprintf(stderr, "z2 : [%d] = z # z = %s\n", z2->nBits, tmp=bitvector_t_toHexString(z2)); free(tmp);

  bitvector_t *t = bitvector_t_take(z2, 121);

  fprintf(stderr, "t : [%d] = take`{back=121} z2 = %s\n", t->nBits, tmp=bitvector_t_toHexString(t)); free(tmp);

  bitvector_t_takeUpdate(z2, 121);

  fprintf(stderr, "z2 : [%d] = take`{back=121} z2 = %s\n", z2->nBits, tmp=bitvector_t_toHexString(z2)); free(tmp);

  bitvector_t *xor = bitvector_t_xor(t, t);

  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);

  bitvector_t_setBit(xor, 0);
  
  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);

  bitvector_t_setBit(xor, 63);
  
  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);

  bitvector_t_setBit(xor, 120);
  bitvector_t_unsetBit(xor, 63);
  
  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);
  
  uint8_t equal = bitvector_t_equal(t, z);
  
  fprintf(stderr, "t == z ? %u\n", equal);

  bitvector_t *slice = bitvector_t_alloc(102);

  bitvector_t_sliceUpdate(slice, z2, 16, 84);

  fprintf(stderr, "slice : [%d] = z2!![84--16] = %s\n", slice->nBits, tmp=bitvector_t_toHexString(slice)); free(tmp);

  equal = bitvector_t_equal(slice, z2);
  
  fprintf(stderr, "t == z ? %u\n", equal);

  fprintf(stderr, "slice!0 == %u\n", bitvector_t_getBit(slice, 0));

  fprintf(stderr, "popcount(slice) == %u\n", bitvector_t_popcount(slice));

  bitvector_t *slice2 = bitvector_t_slice(z, 43, 81);

  fprintf(stderr, "z!![81..43] = %s\n", tmp=bitvector_t_toHexString(slice2)); free(tmp);
  
  bitvector_t_free(x);
  bitvector_t_free(y);
  bitvector_t_free(z);
  bitvector_t_free(z2);
  bitvector_t_free(slice);
  bitvector_t_free(slice2);
  bitvector_t_free(t);
  bitvector_t_free(xor);
  
  return 0;
}
