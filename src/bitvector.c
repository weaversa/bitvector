#include "bitvector.h"

create_c_list_type(uint64_t_plist, uint64_t *)

bitvector_t *bitvector_t_alloc(uint64_t nBits, uint8_t bZeroize) {
  bitvector_t *bv = malloc(1 * sizeof(bitvector_t));
  bv->nBits = (uint64_t) ((((int64_t)nBits) - 1) >> 6) + 1;

  uint64_t_plist_init(&bv->pBits, bv->nBits, 0);
  if(bZeroize)
    memset((void *)&bv->pBits.pList, 0, bv->pBits.nLength_max * sizeof(uint64_t));
  return bv;
}

void bitvector_t_free(bitvector_t *bv) {
  uint64_t_plist_free(&bv->pBits, NULL);
  free(bv);
}
