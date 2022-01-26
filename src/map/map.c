#include "specific.h"

map* map_init(void){

    short num_of_maps;

    num_of_maps = 1;

    map* m = (map*) calloc(sizeof(map), num_of_maps);
    
    if(m == NULL){
        fprintf(stderr, "Memory error - cannot calloc space for map\n");
        exit(EXIT_FAILURE);
    }

    m->variablemap = (mapping*) calloc(sizeof(mapping), NUM_OF_VARS);
    
    if(m->variablemap == NULL){
        fprintf(stderr, "Memory error - cannot calloc space for map\n");
        exit(EXIT_FAILURE);
    }

    return m;
}

bool map_add(map* map, char* key, nlab_array* value){
    
    short code;

    if(map != NULL && key != NULL && value != NULL){
        if(map_contains_key(map, key)){
            nlab_array_free(map_get_key_value(map, key));
        }

        nlab_array* copy_val = nlab_array_copy(value);

        code = map_get_keycode(key);

        strcpy(map->variablemap[code].key, key);
        map->variablemap[code].value = copy_val;
        return true;
    }

    return false;
}

bool map_contains_key(map* map,  char* key){

    short code, strlen_of_var;
    strlen_of_var = 2;

    if(map != NULL && key != NULL){
        if(((short) strlen(key) == strlen_of_var) && (key[0] == '$')){
        code = map_get_keycode(key);

            if((int) strlen(map->variablemap[code].key) != 0){
                return true;
            }
        }
    }
    return false;
}

nlab_array* map_get_key_value(map* map,  char* key){

    short code;

    if(map_contains_key(map, key)){
        code = map_get_keycode(key);
        return map->variablemap[code].value;
    }
    return NULL;
}

bool map_free(map* map){

    if(map == NULL){
        return false;
    }

    for(int i = 0; i < NUM_OF_VARS; i++){
        if(map->variablemap[i].value != NULL){
            for(unsigned int j = 0; j < map->variablemap[i].value->rows; j++){
                if(map->variablemap[i].value->array[j] != NULL){
                    FREE_AND_NULL(map->variablemap[i].value->array[j]);
                }
            }
            FREE_AND_NULL(map->variablemap[i].value->array);
        }
        FREE_AND_NULL(map->variablemap[i].value);
    }

    FREE_AND_NULL(map->variablemap);
    FREE_AND_NULL(map);
    return true;
}


short map_get_keycode(char variable[2]){

    char letter;
    short code, position_of_letter;

    position_of_letter = 1;

    letter = variable[position_of_letter];

    code = (short) letter - 'A';
    return code;
}
