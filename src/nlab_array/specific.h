#pragma once

#include "nlab_array.h"

struct nlab_array {
    unsigned int rows;
    unsigned int cols;
    int** array;
};
