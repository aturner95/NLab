#include "../src/general.h"

#include "../src/nlab.h"

void test_map(void){

    map* varmap = map_init();
    assert(varmap);
    // assert letters get 'hashed' into their expected container
    // via the ASCII key
    char var1[3] = "$A";
    assert(map_get_keycode(var1) == 0);
    char var2[3] = "$M";
    assert(map_get_keycode(var2) == 12);
    char var3[3] = "$Z";
    assert(map_get_keycode(var3) == 25);

    // assert we can add unique keys only
    nlab_array* data1 = nlab_array_create_1d(3);
    assert(data1->rows == 1);
    assert(data1->cols == 1);
    assert(data1->array[0][0] == 3);
    assert(strlen(varmap->variablemap[0].key) == 0);
    assert(map_add(varmap, var1, data1));
    assert(strlen(varmap->variablemap[0].key) != 0);
    assert(map_get_key_value(varmap, "$A")->array[0][0] == 3);
    nlab_array* data2 = nlab_array_create_1d(5);

    // designed so that re-adding existing keys overwrites old value
    // by freeing old then adding new into map
    assert(map_add(varmap, "$A", data2));
    assert(map_add(varmap, "$F", data2));
    assert(map_get_key_value(varmap, "$F")->array[0][0] == 5);
    // assert we can add NULL values
    assert(!map_add(varmap, "$A", NULL));

    // assert can't add NULL keys - valid varname tokens are 
    assert(!map_add(varmap, NULL, data2));

    // assert values are defaulted to NULL
    assert(map_get_key_value(varmap, "$D") == NULL);

    nlab_array_free(data1);
    nlab_array_free(data2);

    map_free(varmap);

}
