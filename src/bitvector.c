#include "bitvector.h"

create_c_list_type(uint64_t_list, uint64_t)

extern void bitvector_t_zeroize(bitvector_t *bv);
extern bitvector_t *bitvector_t_alloc(uint32_t nBits);
extern void bitvector_t_free(bitvector_t *bv);
extern void bitvector_t_cleanHighBits(bitvector_t *bv);
extern void bitvector_t_widen(bitvector_t *bv, uint32_t nBitsToAdd);
extern uint64_t hexchar_to_digit(char c);
extern bitvector_t *bitvector_t_fromHexString(char *string, uint32_t length);
extern char *bitvector_t_toHexString(bitvector_t *bv);
extern void bitvector_t_copyUpdate(bitvector_t *dst, bitvector_t *src);
extern bitvector_t *bitvector_t_copy(bitvector_t *bv);
extern void bitvector_t_dropUpdate(bitvector_t *bv, uint32_t nBitsToDrop);
extern bitvector_t *bitvector_t_drop(bitvector_t *bv, uint32_t nBitsToDrop);
extern void bitvector_from_bytesUpdate(bitvector_t *bv, uint8_t *bytes, uint32_t nBytes);
extern bitvector_t *bitvector_from_bytes(uint8_t *bytes, uint32_t nBytes);
extern void bitvector_to_bytesUpdate(uint8_t *bytes, bitvector_t *bv);
extern uint8_t *bitvector_to_bytes(bitvector_t *bv);
extern bitvector_t *bitvector_t_concat(bitvector_t *x, bitvector_t *y);
extern bitvector_t *bitvector_t_join(bitvector_t **slice, uint32_t parts);
extern void bitvector_t_negateUpdate(bitvector_t *bv);
extern bitvector_t *bitvector_t_negate(bitvector_t *bv);
extern uint8_t bitvector_t_getBit(bitvector_t *bv, uint32_t index);
extern void bitvector_t_setBit(bitvector_t *bv, uint32_t index, uint8_t value);
extern uint32_t bitvector_t_popcount(bitvector_t *bv);
extern void bitvector_t_sliceUpdate(bitvector_t *slice, bitvector_t *bv, uint32_t start, uint32_t length);
extern bitvector_t *bitvector_t_slice(bitvector_t *bv, uint32_t start, uint32_t length);
extern bitvector_t **bitvector_t_split(bitvector_t *bv, uint32_t parts);
extern uint8_t bitvector_t_equal(bitvector_t *x, bitvector_t *y);

#define bitvector_t_zipWith(NAME)                                       \
extern void bitvector_t_##NAME##Update(bitvector_t *x, bitvector_t *y); \
extern bitvector_t *bitvector_t_##NAME(bitvector_t *x, bitvector_t *y); \

bitvector_t_zipWith(xor)
bitvector_t_zipWith(or)
bitvector_t_zipWith(and)
