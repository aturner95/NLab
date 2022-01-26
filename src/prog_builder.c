#include "nlab.h"

/*
    SETUP TEST HELPER FUNCTIONS
*/

Program* program_builder_init(void){
    Program* p = (Program*) calloc(NUM_OF_PROGRAMS, sizeof(Program));
    if(p == NULL){
        fprintf(stderr, "Memory error - unable to create memory for program\n.");
        exit(EXIT_FAILURE);
    }

    p->tokens = (char**) calloc(MAX_NUM_OF_TOKENS, sizeof(char*));
    if(p->tokens == NULL){
        fprintf(stderr, "Memory error - unable to create memory for program\n.");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < MAX_NUM_OF_TOKENS; i++){
        p->tokens[i] = (char*) calloc(MAX_TOKEN_SIZE, sizeof(char));
            
        if(p->tokens[i] == NULL){
            fprintf(stderr, "Memory error - unable to create memory for program\n.");
            exit(EXIT_FAILURE);
        }
    }
    p->error_state = error_none;
    p->polish_stack = stack_init();
    p->variable_map = map_init();
    

    return p;
}

bool program_builder_add(Program* prog, char* token){

    if(prog == NULL || prog->tokens == NULL){
        return false;
    }

    if(prog->num_of_tokens >= MAX_NUM_OF_TOKENS){
        fprintf(stderr, "IO error - program exceeds maximum size (1,000)\n.");
        exit(EXIT_FAILURE);
    }

    if(token == NULL || strlen(token) > MAX_TOKEN_SIZE){
        return false;
    }

    char* returnedstr;
    
    returnedstr = strcpy(prog->tokens[prog->num_of_tokens], token);

    if(!(strcmp(returnedstr, token) == 0)){
        return false;
    }
    prog->num_of_tokens++;
    return true;
}

void program_builder_free(Program* prog){
    if(prog != NULL){
        for(int i = 0; i < MAX_NUM_OF_TOKENS; i++){
            FREE_AND_NULL(prog->tokens[i]);
            }
        
        FREE_AND_NULL(prog->tokens);

        if(prog->variable_map != NULL){
            map_free(prog->variable_map);
            prog->variable_map = NULL;
        }

        if(prog->polish_stack != NULL){
            stack_free(prog->polish_stack);
            prog->polish_stack = NULL;
        }

        FREE_AND_NULL(prog);
        prog = NULL;
    }
}
