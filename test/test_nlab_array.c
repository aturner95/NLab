#include "../src/nlab.h"

void test_nlab_array(void){

    // test #1 - happy test
    nlab_array* arr1 = nlab_array_create_1d(3);
    assert(arr1->array[0][0] == 3);

    // test #2 -zero is also a valid variable
    nlab_array* arr2 = nlab_array_create_1d(0);
    assert(arr2->array[0][0] == 0);

    // test #3 - valid array of ones for CREATE instruction
    nlab_array* arr3 = nlab_array_create_ones(3, 4);
    assert(arr3->rows = 3);
    assert(arr3->cols = 4);
    for(unsigned int y = 0; y < arr3->rows; y++){
        for(unsigned int x = 0; x < arr3->cols; x++){
            assert(arr3->array[y][x] == 1);
        }
    }

    // test #4 - invalid ONES
    nlab_array* arr4 = nlab_array_create_ones(0, 4);
    assert(arr4 == NULL);

    // test #5 - copy a variable (1d array)
    nlab_array* arr5 = nlab_array_copy(arr1);
    assert(arr5->rows == arr1->rows);
    assert(arr5->cols == arr1->cols);
    assert(arr5->array[0][0] == arr1->array[0][0]);

    // test #6 - copy a ONES array (multi-dim array)
    nlab_array* arr6 = nlab_array_copy(arr3);
    assert(arr6->rows == arr3->rows);
    assert(arr6->cols == arr3->cols);
    for(unsigned int y = 0; y < arr6->rows; y++){
        for(unsigned int x = 0; x < arr6->cols; x++){
            assert(arr6->array[y][x] == arr3->array[y][x]);
        }
    }

    // test #7 - copy a NULL pointer 
    nlab_array* arr7 = nlab_array_copy(NULL);
    assert(arr7 == NULL);

    nlab_array_free(arr1);
    nlab_array_free(arr2);
    nlab_array_free(arr3);
    nlab_array_free(arr5);
    nlab_array_free(arr6);

}
