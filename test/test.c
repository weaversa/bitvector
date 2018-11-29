#include "bitvector.h"

int main() {

  bitvector_t *x = bitvector_t_fromHexString("ABCDABCDABCDABCD0123", 20);
  bitvector_t *y = bitvector_t_fromHexString("0123", 4);

  fprintf(stderr, "x : [%d] = %s\n", x->nBits, bitvector_t_toHexString(x));
  fprintf(stderr, "y : [%d] = %s\n", y->nBits, bitvector_t_toHexString(y));

  bitvector_t *z = bitvector_t_concat(x, y);
  fprintf(stderr, "z : [%d] = x # y = %s\n", z->nBits, bitvector_t_toHexString(z));

  bitvector_t_widen(z, 121);
  fprintf(stderr, "z : [%d] = %s\n", z->nBits, bitvector_t_toHexString(z));

  bitvector_t *z2 = bitvector_t_concat(z, z);

  fprintf(stderr, "z2 : [%d] = z # z = %s\n", z2->nBits, bitvector_t_toHexString(z2));

  bitvector_t *t = bitvector_t_take(z2, 121);

  fprintf(stderr, "t : [%d] = take`{121} z2 = %s\n", t->nBits, bitvector_t_toHexString(t));

  bitvector_t_take_update(z2, 121);

  fprintf(stderr, "z2 : [%d] = take`{121} z2 = %s\n", z2->nBits, bitvector_t_toHexString(z2));






  bitvector_t_free(x);
  bitvector_t_free(y);
  bitvector_t_free(z);
  bitvector_t_free(z2);

  return 0;
}
