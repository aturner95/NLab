#include "stack.h"

#pragma once

#define FORMATSTR "%i"
#define ELEMSIZE 20

#define FIXEDSIZE 16
#define SCALEFACTOR 2

struct stack {
   nlab_array* a;
   int size;
   int capacity;
};
