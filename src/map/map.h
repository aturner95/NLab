#pragma once

#include "../nlab_array/nlab_array.h"
#include "../nlab_array/specific.h"

 // $ + <letter> + '\0'
#define CHARS_IN_VARNAME 3
#define NUM_OF_VARS 26

typedef struct mapping mapping;

typedef struct map map;

map* map_init(void);
bool map_add(map* map, char* key, nlab_array* value);
bool map_contains_key(map* map,  char* key);
struct nlab_array* map_get_key_value(map* map,  char* key);
bool map_free(map* map);
short map_get_keycode(char* key);
