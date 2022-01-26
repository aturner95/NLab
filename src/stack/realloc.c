#include "specific.h"

#define DOTFILE 5000

stack* stack_init(void){

   short num_of_stacks;
   stack *s;

   num_of_stacks = 1;

   s = (stack*) calloc(num_of_stacks, sizeof(stack));

   if(s == NULL){
      fprintf(stderr, "Memory error - cannot calloc space for stack\n");
      exit(EXIT_FAILURE);
   }

   s->a = (nlab_array*) calloc(FIXEDSIZE, sizeof(nlab_array));

   if(s->a == NULL){
      fprintf(stderr, "Memory error - cannot calloc space for stack\n");
      exit(EXIT_FAILURE);
   }

   s->size = 0;
   s->capacity= FIXEDSIZE;
   return s;
}

bool stack_push(stack* s, nlab_array* d){

   if(s == NULL || d == NULL){
       return false;
   }

   nlab_array* copy_d;

   if(s->size >= s->capacity){
      s->a = (nlab_array*) realloc(s->a, sizeof(nlab_array)*s->capacity*SCALEFACTOR);

      if(s->a == NULL){
         fprintf(stderr, "Memory error - cannot malloc() space for nlab array");
         exit(EXIT_FAILURE);
      }
      s->capacity = s->capacity*SCALEFACTOR;
   }

   // free the data already in the stack before pushing, otherwise the memory will leak
   copy_d = nlab_array_copy(d);
   for(unsigned  int y = 0; y < (s->a[s->size].rows); y++){
      FREE_AND_NULL(s->a[s->size].array[y]);
   }
   FREE_AND_NULL(s->a[s->size].array);

   s->a[s->size] = *copy_d;
   // copy_d dereferenced and assigned into fixed-sized array, so free the reference
   free(copy_d);
   s->size = s->size + 1;
   return true;
}

nlab_array* stack_pop(stack* s){
   if((s == NULL) || (s->size < 1)){
      return NULL;
   }

   s->size = s->size - 1;
   return &s->a[s->size];
}

nlab_array* stack_peek(stack* s){
   if((s==NULL) || (s->size <= 0)){
      return NULL;
   }

   return &s->a[s->size-1];
}


bool stack_free(stack* s){
   if(s==NULL){
      return false;
   }

   for(int i = 0; i < (s->capacity); i++){
      if(s->a[i].array != NULL){
         for(unsigned int y = 0; y < (s->a[i].rows); y++){
            FREE_AND_NULL(s->a[i].array[y]);
         }
      }
      FREE_AND_NULL(s->a[i].array);
   }
   FREE_AND_NULL(s->a);

   FREE_AND_NULL(s);
   return true;
}
