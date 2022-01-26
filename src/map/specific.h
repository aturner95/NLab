#include "map.h"

#pragma once

struct mapping {
    char key[CHARS_IN_VARNAME];
    nlab_array* value;
};

struct map {
   mapping* variablemap;
};
