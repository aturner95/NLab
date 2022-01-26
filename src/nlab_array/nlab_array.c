#include "specific.h"

nlab_array* nlab_array_create_1d(unsigned int val){
    return _nlab_array_create(1, 1, val);
}

nlab_array* nlab_array_create_ones(unsigned int rows, unsigned int cols){
    return _nlab_array_create(rows, cols, 1);
}

nlab_array* _nlab_array_create(unsigned int rows, unsigned int cols, unsigned int val){
    short num_maps;

    if(rows == 0 || cols == 0){
        return NULL;
    }

    num_maps = 1;
    
    nlab_array* nlab = (nlab_array*) calloc(sizeof(nlab_array), num_maps);
    
    if(nlab == NULL){
        fprintf(stderr, "Memory error - cannot calloc space for nlab array\n");
        exit(EXIT_FAILURE);
    }

    nlab->cols = cols;
    nlab->rows = rows;
    nlab->array = (int**) calloc(sizeof(int*), (nlab->cols * nlab->rows));

    if(nlab->array == NULL){
        fprintf(stderr, "Memory error - cannot calloc space for nlab array\n");
        exit(EXIT_FAILURE);
    }


    for(unsigned int y = 0; y < nlab->rows; y++){
        nlab->array[y] = (int*) calloc(sizeof(int), nlab->cols);
            if(nlab->array[y] == NULL){
                fprintf(stderr, "Memory error - cannot calloc space for nlab array\n");
                exit(EXIT_FAILURE);
            }
        for(unsigned int x = 0; x < nlab->cols; x++){
            nlab->array[y][x] = val;
        }
    }

    return nlab;
}

/*
    These nlab_arry data structures will be pass-by-value rather than reference,
    otherwise the pointers get tangled up (SEGV) when trying to free both maps 
    and stacks as they point to the same memory addresses.
*/
nlab_array* nlab_array_copy(nlab_array* d){
    
    short num_arrays;
    nlab_array* copy_d;

    if(d == NULL){
        return NULL;
    }

    num_arrays = 1;

    copy_d = (nlab_array*) calloc(sizeof(nlab_array), num_arrays);

    if(copy_d == NULL){
        fprintf(stderr, "Memory error - cannot malloc() space for nlab array");
        exit(EXIT_FAILURE);
    }

    copy_d->cols = d->cols;
    copy_d->rows = d->rows;
    copy_d->array = (int**) calloc(sizeof(int*), (d->cols * d->rows));
    if(copy_d->array == NULL){
        fprintf(stderr, "Memory error - cannot malloc() space for nlab array");
        exit(EXIT_FAILURE);
    }

    for(unsigned int y = 0; y < copy_d->rows; y++){
        copy_d->array[y] = (int*) calloc(sizeof(int), (d->cols));
        for(unsigned int x = 0; x < copy_d->cols; x++){
            copy_d->array[y][x] = d->array[y][x];
        }
    }
    return copy_d;
}


void nlab_array_free(nlab_array* narr){
    for(unsigned int y = 0; y < narr->rows; y++){
        FREE_AND_NULL(narr->array[y]);
    }
    FREE_AND_NULL(narr->array);
    FREE_AND_NULL(narr);
}
