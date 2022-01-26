#include "../src/nlab.h"

void test_stack(void){
    stack* s = stack_init();
    assert(s);

    // assert pushing valid int arrays onto stack
    nlab_array* two = nlab_array_create_1d(2);
    assert(stack_push(s, two));
    assert(s->a[s->size-1].array[0][0] == 2);
    nlab_array* peek2 = stack_peek(s);
    assert(peek2->array[0][0] == 2);
    assert(s->size == 1);

    // assert cannot push NULL stack frame
    assert(!stack_push(s, NULL));

    // assert pushing stack frame 3 on top of 2 and then pop
    nlab_array* three = nlab_array_create_1d(3);
    assert(stack_push(s, three));
    assert(s->a[s->size-1].array[0][0] == 3);
    assert(s->size == 2);
    assert(s->capacity == 16);
    assert(stack_peek(s)->array[0][0] == 3);

    nlab_array* pop3 = NULL;
    pop3 = stack_pop(s);
    assert(pop3 != NULL);
    assert(pop3->array[0][0] == 3);


    // assert top frame is 2 as expected after popping 3
    nlab_array* peek2_2 = NULL;
    peek2_2 = stack_peek(s);
    assert(peek2_2->array[0][0] == 2);

    // assert false returned when popping empty stack
    nlab_array* pop2 = NULL;
    pop2 = stack_pop(s);
    assert(pop2->array[0][0] == 2);
    nlab_array* pop_null = stack_pop(s);
    assert(pop_null == NULL);

    // try pushing more
    nlab_array* four = nlab_array_create_1d(4);
    assert(stack_push(s, four));
    nlab_array* five = nlab_array_create_1d(5);
    assert(stack_push(s, five));


    nlab_array_free(two);
    nlab_array_free(three);
    nlab_array_free(four);
    nlab_array_free(five);
    assert(stack_free(s));
}
