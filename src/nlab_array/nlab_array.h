#pragma once

#include "../general.h"

typedef struct nlab_array nlab_array;

nlab_array* nlab_array_create_1d(unsigned int val);
nlab_array* nlab_array_create_ones(unsigned int rows, unsigned int cols);
/* _nlab_array_create() considered private - just a helper function*/
nlab_array* _nlab_array_create(unsigned int rows, unsigned int cols, unsigned int val);
nlab_array* nlab_array_copy(nlab_array* d);
void nlab_array_free(nlab_array* narr);
