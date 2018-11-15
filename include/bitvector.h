/***************************************************

  Bitvector: A library for manipulating bitvectors

****************************************************/

#ifndef BITVECTOR_H
#define BITVECTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "c_list_types/include/c_list_types.h"

create_c_list_headers(uint64_t_plist, uint64_t *)

typedef struct bitvector_t {
  uint64_t nBits;
  uint64_t_plist pBits;
} bitvector_t;


#endif
