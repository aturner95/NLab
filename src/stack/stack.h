/*
    Thanks to Neill C for the ready-made stack code (https://github.com/csnwc/ADTs)
*/
#pragma once

#include "../nlab_array/nlab_array.h"
#include "../nlab_array/specific.h"

typedef struct stack stack;

stack* stack_init(void);
bool stack_push(stack* s, nlab_array* d);
nlab_array* stack_pop(stack* s);
bool stack_free(stack* s);
nlab_array* stack_peek(stack*s);
