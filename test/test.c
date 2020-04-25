#include "bitvector.h"

int main() {

  char *tmp;
  
  bitvector_t *x = bitvector_t_fromHexString("ABCDABCDABCDABCD0123");
  bitvector_t *y = bitvector_t_fromHexString("0123");

  fprintf(stderr, "x : [%d] = %s\n", x->nBits, tmp=bitvector_t_toHexString(x)); free(tmp);
  fprintf(stderr, "y : [%d] = %s\n", y->nBits, tmp=bitvector_t_toHexString(y)); free(tmp);

  bitvector_t *z = bitvector_t_concat(x, y);
  fprintf(stderr, "z : [%d] = x # y = %s\n", z->nBits, tmp=bitvector_t_toHexString(z)); free(tmp);

  bitvector_t_widenUpdate(z, 121);
  fprintf(stderr, "z : [%d] = %s\n", z->nBits, tmp=bitvector_t_toHexString(z)); free(tmp);

  bitvector_t *z2 = bitvector_t_concat(z, z);

  fprintf(stderr, "z2 : [%d] = z # z = %s\n", z2->nBits, tmp=bitvector_t_toHexString(z2)); free(tmp);

  bitvector_t *t = bitvector_t_drop(z2, 121);

  fprintf(stderr, "t : [%d] = drop`{121} z2 = %s\n", t->nBits, tmp=bitvector_t_toHexString(t)); free(tmp);

  bitvector_t_dropUpdate(z2, 121);

  fprintf(stderr, "z2 : [%d] = drop`{121} z2 = %s\n", z2->nBits, tmp=bitvector_t_toHexString(z2)); free(tmp);

  bitvector_t *xor = bitvector_t_xor(t, t);

  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);

  bitvector_t_setBit(xor, 0, 1);
  
  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);

  bitvector_t_setBit(xor, 63, 1);
  
  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);

  bitvector_t_setBit(xor, 120, 1);
  bitvector_t_setBit(xor, 63, 0);
  
  fprintf(stderr, "xor : [%d] = t ^ t = %s\n", xor->nBits, tmp=bitvector_t_toHexString(xor)); free(tmp);
  
  uint8_t equal = bitvector_t_equal(t, z);
  
  fprintf(stderr, "t == z ? %u\n", equal);

  bitvector_t *slice = bitvector_t_alloc(102);

  bitvector_t_sliceUpdate(slice, z2, 16, 50);

  fprintf(stderr, "slice : [%d] = z2!![??] = %s\n", slice->nBits, tmp=bitvector_t_toHexString(slice)); free(tmp);

  equal = bitvector_t_equal(slice, z2);
  
  fprintf(stderr, "t == z ? %u\n", equal);

  fprintf(stderr, "slice!0 == %u\n", bitvector_t_getBit(slice, 0));

  fprintf(stderr, "popcount(slice) == %u\n", bitvector_t_popcount(slice));

  bitvector_t *slice2 = bitvector_t_slice(z, 43, 24);

  fprintf(stderr, "z!![??] = %s\n", tmp=bitvector_t_toHexString(slice2)); free(tmp);

  uint8_t *slice2_clone = bitvector_t_to_bytes(slice2);
  fprintf(stderr, "z!![??] = ");
  uint32_t i;  
  for(i = 0; i < slice2->nBits/8; i++) {
    fprintf(stderr, "%.2x", slice2_clone[i]);
  }
  fprintf(stderr, "\n");
  
  bitvector_t *bucket = bitvector_t_from_bytes(slice2_clone, slice2->nBits/8);
  fprintf(stderr, "bucket = %s\n", tmp=bitvector_t_toHexString(bucket)); free(tmp);
  

  sequence_t *zsequence = bitvector_t_split(z, 31);

  fprintf(stderr, "z sequence = [");
  for(i = 0; i < zsequence->nLength; i++) {
    fprintf(stderr, "%s, ", tmp=bitvector_t_toHexString(&zsequence->pList[i])); free(tmp);
  }
  fprintf(stderr, "]\n");

  bitvector_t *zjoin = sequence_t_join(zsequence);
  fprintf(stderr, "zjoin : [%d] = %s\n", z->nBits, tmp=bitvector_t_toHexString(zjoin)); free(tmp);

  equal = bitvector_t_equal(z, zjoin);
  
  fprintf(stderr, "z == zjoin ? %u\n", equal);

  sequence_t_free(zsequence, bitvector_t_free_inner);
  bitvector_t_free(zjoin);
  bitvector_t_free(x);
  bitvector_t_free(y);
  bitvector_t_free(z);
  bitvector_t_free(z2);
  bitvector_t_free(slice);
  bitvector_t_free(slice2);
  free(slice2_clone);
  bitvector_t_free(t);
  bitvector_t_free(xor);
  
  return 0;
}
