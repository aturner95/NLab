#include "nlab.h"

#ifdef TESTMODE
int main(void){

    test();
    return EXIT_SUCCESS;
    }

#else
int main(int argc, char* argv[]){

    short prog_arg, file_arg, num_of_cmd_line_args;
    Program* prog;

    prog_arg = 0;
    file_arg = 1;
    num_of_cmd_line_args = 2;
    
    if(argc != num_of_cmd_line_args){
        fprintf(stderr, "IO error - usage: %s <filename.nlb>\n.", argv[prog_arg]);
        exit(EXIT_FAILURE);
    }

    prog = program_builder_init();

    readfile(argv[file_arg], prog);

    if(program(prog)){
        program_builder_free(prog);
        exit(EXIT_SUCCESS);
    } else{
        if(strlen(prog->error_msg) != 0){
            process_error_msg(prog, prog->error_msg);
        } else{
            prog->error_state = error_unknown;
           process_error_msg(prog, "an unknown error has occured");
        }
        program_builder_free(prog);
        exit(EXIT_FAILURE);
    }
}
#endif

#ifdef TESTMODE
void test(void){
    test_nlab();
    test_stack();
    test_map();
    test_nlab_array();
}
#endif

void readfile(char* filename, Program* prog){
    
    FILE* fp;
    short single_word;
    char string[MAX_TOKEN_SIZE];
    
    fp = fopen(filename, "rt");

    if(fp == NULL){
        prog->error_state = error_io;
        process_error_msg(prog, "unable to open NLab file");
        program_builder_free(prog);
        exit(EXIT_FAILURE);
    }

    if(!_is_correct_file_extention(filename, ".nlb")){
        prog->error_state = error_io;
        process_error_msg(prog, "expected file ext is .nlb");
        program_builder_free(prog);
        exit(EXIT_FAILURE);
    }
    
    single_word = 1;

    while(fscanf(fp, "%s", string) == single_word){
        if(prog->num_of_tokens > MAX_NUM_OF_TOKENS){
            prog->error_state = error_io;
            process_error_msg(prog, "program exceeds max size of tokens (1000)");
            program_builder_free(prog);
            exit(EXIT_FAILURE);
        }
        program_builder_add(prog, string);
    }

    fclose(fp);
}

bool set_error_msg(Program* prog, const char* msg){
    CHECK_PROG_FOR_NULL(prog);

    char* returnedstr;

    if(strlen(prog->error_msg) == 0){

        if(msg == NULL || strlen(msg) == 0){
            return false;
        }

        returnedstr = strcpy(prog->error_msg, msg);

        if(STRINGS_EQUAL(returnedstr, msg)){
            return true;
        }
    }
    return false;
}

void process_error_msg(Program* prog, char* message){

    char dummystr[MAX_STRING_LENGTH];
    dummystr[0] = '\0';

    switch(prog->error_state){
        case error_io:
            strcat(dummystr, "IO error - ");
            strcat(dummystr, message);
            strcat(dummystr, ".\n");
            #ifndef TESTMODE
            fprintf(stderr, "%s", dummystr);
            #endif
            break;
        case error_parse:
            strcat(dummystr, "Parsing error - expected grammar: ");
            strcat(dummystr, message);
            strcat(dummystr, ".\n");
            #ifndef TESTMODE
            fprintf(stderr, "%s", dummystr);
            #endif
            break;
        case error_interp:
            strcat(dummystr, "Interpreter error - ");
            strcat(dummystr, message);
            strcat(dummystr, ".\n");
            #ifndef TESTMODE
            fprintf(stderr, "%s", dummystr);
            #endif
            break;
        default:
            #ifndef TESTMODE
            fprintf(stderr, "Unknown error - an unknown error has occured.\n");
            #endif
            break;
    }
}

int word_to_integer(char* word){
    int index;
    short error = -1;

    if(word == NULL){
        return error;
    }

    index = (strlen(word) - 1);

    while(index >= 0){
        if(!isdigit(word[index])){
            return error;
        }
        index--;
    }

    return atoi(word);
}

bool _is_correct_file_extention(char* filename, char* extension){
    
    short offset_zero_index;
    unsigned int filename_index, extension_index;

    if(filename == NULL || extension == NULL){
        return false;
    }

    offset_zero_index = 1;
    filename_index = strlen(filename) - offset_zero_index;
    extension_index = strlen(extension) - offset_zero_index;

    if(extension_index > filename_index){
        return false;
    }

    while(extension_index != 0){

        if(filename[filename_index] != extension[extension_index]){
            return false;
        }

        filename_index--;
        extension_index--;
    }

    return true;
}

bool _format_filename(char* filename){

    unsigned int num_chars, pos_of_opening_quot, pos_of_closing_quot;

    if(filename == NULL){
        return false;
    }

    num_chars = strlen(filename);
    pos_of_opening_quot = 0;
    pos_of_closing_quot = num_chars - 1;

    if(filename[pos_of_opening_quot] == '\"' && filename[pos_of_closing_quot] == '\"'){
        for(unsigned int i = 0; i < num_chars; i++){
            filename[i] = filename[i+1];
        }
        filename[pos_of_closing_quot-1] = '\0';
        return true;
    }
    return false;
}


/* --- GRAMMAR FUNCTIONS --- */

// <PROG> ::= "BEGIN" "{" <INSTRCLIST>
bool program(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(STRINGS_EQUAL(CURRENT_WORD, "BEGIN")){
        INCR_CURRENT_WORD;
        if(STRINGS_EQUAL(CURRENT_WORD, LBRACE)){
            INCR_CURRENT_WORD;
            if(instrc_list(prog)){
                return true;
            }
        }
    }
    SET_ERROR_STATE(error_parse);
    set_error_msg(prog, "\"BEGIN\" \"{\" <INSTRCLIST>");
    return false;
}


// <INSTRCLIST> ::= "}" | <INSTRC> <INSTRCLIST>
bool instrc_list(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(STRINGS_EQUAL(CURRENT_WORD, RBRACE)){
        INCR_CURRENT_WORD;
        return true;
    } else if(instrc(prog)){
        if(instrc_list(prog)){
            return true;
        }
    }
    SET_ERROR_STATE(error_parse);
    set_error_msg(prog, "<INSTRCLIST> ::= \"}\" | <INSTRC> <INSTRCLIST>");
    return false;
}

// <INSTRC> ::= <PRINT> | <SET> | <CREATE> | <LOOP>
bool instrc(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(print(prog)){
        return true;
    } else if(set(prog)){
        return true;
    } else if(create(prog)){
        return true;
    } else if(loop(prog)){        
        return true;
    }

    return false;
}


// <PRINT> ::= "PRINT" <VARNAME> | "PRINT" <STRING>
bool print(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(STRINGS_EQUAL(CURRENT_WORD, "PRINT")){
        INCR_CURRENT_WORD;

        #ifdef INTERP
        char* temp;
        #endif

        if(varname(prog)){

            #ifdef INTERP
            char message[MAX_STRING_LENGTH];
            
            // varname() increments word counter, so look back one
            temp = interp_print_variable(prog, LOOK_AT_PREV_WORD);
            strcpy(message, temp);
            if(prog->error_state == error_interp){
                strcat(message, "illegal use of uninitialized variable: \'");
                strcat(message, LOOK_AT_PREV_WORD);
                strcat(message, "\'");
                set_error_msg(prog, message);
                free(temp);
                return false; 
            } else{
                #ifndef TESTMODE
                printf("%s", message);
                #endif
                free(temp);
            }
            #endif

            return true;
        } else if(string(prog)){

            #ifdef INTERP
            // string() increments word counter, so look back one    
            temp = interp_print_string(LOOK_AT_PREV_WORD);

            #ifndef TESTMODE
            printf("%s\n", temp);
            #endif

            free(temp);
            #endif

            return true;
        } 

        SET_ERROR_STATE(error_parse);
        set_error_msg(prog, "<PRINT> ::= \"PRINT\" <VARNAME> | \"PRINT\" <STRING>");
        return false;
    }
    
    return false;
}


// <VARNAME> ::= "$"[A-Z] # e.g. $A, $B, $Z etc.
bool varname(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    short var_len, pos_of_dollar, pos_of_letter;

    if(CURRENT_WORD == NULL){
        return false;
    }

    var_len = 2;

    if((short) strlen(CURRENT_WORD) != var_len){
        return false;
    }

    pos_of_dollar = 0;
    pos_of_letter = 1;

    if(CURRENT_WORD[pos_of_dollar] != '$'){
        return false;
    }

    if(!isalpha(CURRENT_WORD[pos_of_letter]) || islower(CURRENT_WORD[pos_of_letter])){
        return false;
    }

    INCR_CURRENT_WORD;
    return true;
}

// <STRING> :: Double-quoted string e.g. "../../doof.arr", "Hello!" etc.
bool string(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    int pos_of_opening_quot_marks, pos_of_closing_quot_marks;

    if(CURRENT_WORD == NULL){
        return false;
    }

    if(strlen(CURRENT_WORD) >= MAX_TOKEN_SIZE){
        return false;
    }

    pos_of_opening_quot_marks = 0;
    pos_of_closing_quot_marks = (int) strlen(CURRENT_WORD) - 1;

    if(CURRENT_WORD[pos_of_opening_quot_marks] != '\"' 
    || CURRENT_WORD[pos_of_closing_quot_marks] != '\"'){
        return false;
    }

    if(strchr(CURRENT_WORD, ' ')){
        return false;
    }

    INCR_CURRENT_WORD;
    return true;
}

// <SET> ::= "SET" <VARNAME> ":=" <POLISHLIST>
bool set(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    #ifdef INTERP
        char* variable_context;
    #endif

    if(STRINGS_EQUAL(CURRENT_WORD, "SET")){
        INCR_CURRENT_WORD;
        if(varname(prog)){
            #ifdef INTERP
                variable_context = LOOK_AT_PREV_WORD;
            #endif

            if(STRINGS_EQUAL(CURRENT_WORD, ":=")){
                INCR_CURRENT_WORD;
                if(polish_list(prog)){

                    short stack_sz_only_with_result = 1;
                    
                    if(prog->polish_stack->size == stack_sz_only_with_result){
                        char* var = _interp_get_var_context(prog);
                        nlab_array* top = stack_peek(prog->polish_stack);
                        map_add(prog->variable_map, var, top);
                        stack_pop(prog->polish_stack);
                    }

                return true;
                }
            }
        }
        SET_ERROR_STATE(error_parse;);
        set_error_msg(prog, "<SET> ::= \"SET\" <VARNAME> \":=\" <POLISHLIST>");
        return false;
    }

    return false;
}

// <POLISHLIST> ::= <POLISH><POLISHLIST> | ";"
bool polish_list(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(polish(prog)){
        if(polish_list(prog)){
            return true;
        }
    }
    if(STRINGS_EQUAL(CURRENT_WORD, SEMICOLON)){
        if(prog->error_state == error_none){
            INCR_CURRENT_WORD;
            return true;
        }
    }

    return false;
}

// <POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP>
bool polish(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(pushdown(prog)){
        return true;
    } else if(unaryop(prog)){

        #ifdef INTERP

        if(!interp_set(prog)){
            SET_ERROR_STATE(error_interp);
            char dummy[MAX_STRING_LENGTH];
            dummy[0] = '\0';
            strcat(dummy, "unable to interpret expression during SET");
            set_error_msg(prog, dummy);
            return false;
        }
        #endif

        return  true;
    } else if(binaryop(prog)){

        #ifdef INTERP

        if(!interp_set(prog)){
            SET_ERROR_STATE(error_interp);
            char dummy[MAX_STRING_LENGTH];
            dummy[0] = '\0';
            strcat(dummy, "unable to interpret expression during SET");
            set_error_msg(prog, dummy);
            return false;
        }
        #endif
        return true;
    }

    return false;
}

// <PUSHDOWN> ::== <VARNAME> | <INTEGER>
bool pushdown(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(varname(prog)){

        #ifdef INTERP
            if(interp_pushdown_variable(prog)){
                return true;
            } else{
                return false;
            }
        #endif

        return true;

    } else if(integer(prog)){

        #ifdef INTERP
        nlab_array* arr = nlab_array_create_1d(word_to_integer(LOOK_AT_PREV_WORD));
        if(stack_push(prog->polish_stack, arr)){
            // pass-by-value, so free the pointer on this side of the pushdown
            nlab_array_free(arr);
            return true;
        }
        #endif

        return true;
    }

    return false;
}

// # A non-negative integer
// <INTEGER> ::= [0-9]+ # e.g. 1, 250, 3
bool integer(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    int num;

    num = word_to_integer(CURRENT_WORD);

    if(num < 0){
        return false;
    }

    INCR_CURRENT_WORD;
    return true;
}


// <UNARYOP> ::= "U-NOT" | "U-EIGHTCOUNT"
bool unaryop(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(STRINGS_EQUAL(CURRENT_WORD, "U-NOT")){
        INCR_CURRENT_WORD;
        return true;

    } else if (STRINGS_EQUAL(CURRENT_WORD, "U-EIGHTCOUNT")){
        INCR_CURRENT_WORD;
        return true;
    }
    #ifdef EXTENSION
    else if (STRINGS_EQUAL(CURRENT_WORD, "U-TRACE")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "U-TRANSPOSE")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "U-SUBMATRIX")){
        INCR_CURRENT_WORD;
        return true;
    }
    #endif
    
    return false;
}


// <BINARYOP> :: "B-AND" | "B-OR" | "B-GREATER" | "B-LESS" | "B-ADD" | "B-TIMES" | "B-EQUALS"
bool binaryop(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(STRINGS_EQUAL(CURRENT_WORD, "B-AND")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "B-OR")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "B-GREATER")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "B-LESS")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "B-ADD")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "B-TIMES")){
        INCR_CURRENT_WORD;
        return true;
    } else if (STRINGS_EQUAL(CURRENT_WORD, "B-EQUALS")){
        INCR_CURRENT_WORD;
        return true;
    }
    #ifdef EXTENSION
    else if(STRINGS_EQUAL(CURRENT_WORD,"B-DOTPRODUCT")){
        INCR_CURRENT_WORD;
        return true;
    }
    else if(STRINGS_EQUAL(CURRENT_WORD,"B-POWER")){
        INCR_CURRENT_WORD;
        return true;
    }
    #endif
    
    return false;
}

// <CREATE> ::= "ONES" <ROWS> <COLS> <VARNAME> | "READ" <FILENAME> <VARNAME>
bool create(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(STRINGS_EQUAL(CURRENT_WORD, "ONES")){
        INCR_CURRENT_WORD;

        #ifdef INTERP
        unsigned int num_rows, num_cols;
        nlab_array* ones = NULL;
        char* variable_context;
        #endif

        if(rows(prog)){

            #ifdef INTERP
            num_rows = atoi(LOOK_AT_PREV_WORD);
            #endif

            if(cols(prog)){

                #ifdef INTERP
                num_cols = atoi(LOOK_AT_PREV_WORD);
                ones = nlab_array_create_ones(num_rows, num_cols);
                #endif

                if(varname(prog)){

                    // varname() incr. word count, so have to look back one word
                    #ifdef INTERP
                    variable_context = LOOK_AT_PREV_WORD;
                    if(interp_create_ones(prog, LOOK_AT_PREV_WORD, ones)){
                        return true;
                    }
                    #else
                    return true;
                    #endif
                }

                #ifdef INTERP
                if(ones != NULL){
                    nlab_array_free(ones);
                }
                #endif
            }
        }
        SET_ERROR_STATE(error_parse);
        set_error_msg(prog, "<CREATE> ::= \"ONES\" <ROWS> <COLS> <VARNAME>");
        return false;

    } else if(STRINGS_EQUAL(CURRENT_WORD, "READ")){
        INCR_CURRENT_WORD;

        #ifdef INTERP
        char* fname, *variable_context;
        nlab_array* read_arr = NULL;
        #endif

        if(filename(prog)){
            #ifdef INTERP
            fname = LOOK_AT_PREV_WORD;
            #endif

            if(varname(prog)){
                #ifdef INTERP
                variable_context = LOOK_AT_PREV_WORD;

                if(!interp_create_read(prog, variable_context, read_arr, fname)){
                    SET_ERROR_STATE(error_io);
                    
                    char errmsg[MAX_STRING_LENGTH];
                    errmsg[0] = '\0';
                    strcat(errmsg,"unable to open file ");
                    strcat(errmsg, fname);
                    set_error_msg(prog, errmsg);
                    process_error_msg(prog, errmsg);
                    return false;
                }
                #endif

                return true;
            }
        }
        SET_ERROR_STATE(error_parse);
        set_error_msg(prog, "<CREATE> ::= \"READ\" <FILENAME> <VARNAME>");
        return false;
    }
    return false;
}

// <ROWS> ::= <INTEGER>
bool rows(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(integer(prog)){
        return true;
    }

    return false;
}

// <COLS> ::= <INTEGER>
bool cols(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(integer(prog)){
        return true;
    }

    return false;
}

// <FILENAME> ::= <STRING>
bool filename(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    if(string(prog)){
        return true;
    }
    return false;
}

// <LOOP> ::= "LOOP" <VARNAME> <INTEGER> "{" <INSTRCLIST>
bool loop(Program* prog){
    CHECK_PROG_FOR_NULL(prog);

    int condition_int;
    char* condition_str;

    #ifdef INTERP
    int jump_to;
    char* variablename;
    nlab_array* counter_arr = NULL;
    #endif

    if(STRINGS_EQUAL(CURRENT_WORD, "LOOP")){
        INCR_CURRENT_WORD;
        if(varname(prog)){

            #ifdef INTERP
            variablename = LOOK_AT_PREV_WORD;
            jump_to = 0;

            if(map_contains_key(prog->variable_map, variablename)){
                SET_ERROR_STATE(error_interp);
                char dummy[MAX_STRING_LENGTH];
                dummy[0] = '\0';
                strcat(dummy, "variale already declared: ");
                strcat(dummy, variablename);
                set_error_msg(prog, dummy);
                return false;
            } else {
                counter_arr = nlab_array_create_1d(1);
                map_add(prog->variable_map, variablename, counter_arr);
                nlab_array_free(counter_arr);
            }
            #endif

            if(integer(prog)){
                condition_str = LOOK_AT_PREV_WORD;
                condition_int = atoi(condition_str);
                if((condition_int != 0)){
                    if(STRINGS_EQUAL(CURRENT_WORD, LBRACE)){
                        INCR_CURRENT_WORD;

                        // Parse version
                        #ifndef INTERP
                        if(instrc_list(prog)){
                            return true;
                        }
                        #endif

                        // Interp version
                        #ifdef INTERP
                        jump_to = prog->current_token;
                        counter_arr = map_get_key_value(prog->variable_map, variablename); 
                        while(counter_arr->array[0][0] <= condition_int){
                            
                            counter_arr = map_get_key_value(prog->variable_map, variablename); 
                            int store = counter_arr->array[0][0];
                            int tempcounter;

                            if(instrc_list(prog)){
                                counter_arr = map_get_key_value(prog->variable_map, variablename);
                                
                                if(counter_arr == NULL && counter_arr->array==NULL){
                                    tempcounter = store;
                                } else{
                                    tempcounter = counter_arr->array[0][0];
                                }
                                
                                if(tempcounter >= condition_int){
                                    short key = map_get_keycode(variablename);
                                    strcpy(prog->variable_map->variablemap[key].key, "\0\0\0");
                                    nlab_array_free(counter_arr);
                                    counter_arr = NULL;
                                    prog->variable_map->variablemap[key].value=NULL;
                                    return true;
                                }
                            }
                            counter_arr->array[0][0] = counter_arr->array[0][0] + 1;
                            prog->current_token = jump_to;
                        }
                    
                        return true;
                        #endif
                    }
                }
            }
        }
        
        SET_ERROR_STATE(error_parse);
        set_error_msg(prog, "<LOOP> ::= \"LOOP\" <VARNAME> <INTEGER> \"{\" <INSTRCLIST>");
        return false;
    }
    return false;
}


/* --- INTERPRETER FUNCTIONS --- */

char* interp_print_variable(Program* prog, char* current_token){

    nlab_array* arr;
    char* tostring = (char*) calloc(sizeof(char), MAX_STRING_LENGTH);
    short zero_offset;
    unsigned int single_dim;
    if(tostring == NULL){
        fprintf(stderr, "Memory error - uable to calloc space for string");
        exit(EXIT_FAILURE);
    }
    
    tostring[0] = '\0';
    zero_offset = 1;
    single_dim = 1;

    if(map_contains_key(prog->variable_map, current_token)){
        arr = map_get_key_value(prog->variable_map, current_token);
        char int_to_str[MAX_STRING_LENGTH];

        if(arr->rows == single_dim && arr->cols == single_dim){
            sprintf(int_to_str, "%d", arr->array[0][0]);
            strcat(tostring, int_to_str);

        } else {
            for(unsigned int y = 0; y < arr->rows; y++){
                for(unsigned int x = 0; x < arr->cols; x++){

                    sprintf(int_to_str, "%d", arr->array[y][x]);
                    strcat(tostring, int_to_str);

                    if(x != (arr->cols - zero_offset)){
                        strcat(tostring, " ");
                    } else{
                        strcat(tostring, "\n");
                    }
                }
            }
        }
        strcat(tostring, "\n");
        return tostring;
    }

    prog->error_state = error_interp;
    return tostring;
}

char* interp_print_string(char* word){
    char* print_stmt ;
    int j, offset_opening_quot_marks, offset_closing_quot_marks;

    if(word == NULL){
        return NULL;
    }

    print_stmt = (char*) calloc(sizeof(char), MAX_STRING_LENGTH);
    j = 0;
    offset_opening_quot_marks = 1;
    offset_closing_quot_marks = (strlen(word) - 1);

    for(int i = offset_opening_quot_marks; i < offset_closing_quot_marks; i++, j++){
        print_stmt[j] = word[i];
    }
     print_stmt[j] = '\0';
    
    return print_stmt;
}

bool interp_set(Program* prog){

    char* word = prog->tokens[prog->current_token-1];
    nlab_array* result;

    char* variable_context = _interp_get_var_context(prog);

    if(STRINGS_EQUAL(word, "U-NOT")){
        if(!interp_u_not(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "U-EIGHTCOUNT")){
        if(!interp_u_eightcount(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-AND")){
        if(!interp_b_and(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-OR")){
        if(!interp_b_or(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-GREATER")){
        if(!interp_b_greater(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-LESS")){
        if(!interp_b_less(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-ADD")){
        if(!interp_b_add(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-TIMES")){
        if(!interp_b_times(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-EQUALS")){
        if(!interp_b_equals(prog)){
            return false;
        }
    }
    #ifdef EXTENSION
    else if(STRINGS_EQUAL(word, "U-TRACE")){
        if(!extension_u_trace(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "U-TRANSPOSE")){
        if(!extension_u_transpose(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "U-SUBMATRIX")){
        if(!extension_u_submatrix(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-DOTPRODUCT")){
        if(!extension_b_dotproduct(prog)){
            return false;
        }
    } else if(STRINGS_EQUAL(word, "B-POWER")){
        if(!extension_b_power(prog)){
            return false;
        }
    }
#endif
    // add result currently on the top of stack into variable map
    result = stack_peek(prog->polish_stack);

    if(!map_add(prog->variable_map, variable_context, result)){
        SET_ERROR_STATE(error_interp);

        char dummy[MAX_STRING_LENGTH];
        dummy[0] = '\0';
        strcat(dummy, "unable to add token ");
        strcat(dummy, LOOK_AT_PREV_WORD);
        strcat(dummy, " into variable map");
        set_error_msg(prog, dummy);
        return false;
    }

    return true;
}

bool interp_pushdown_variable(Program* prog){
    if(map_contains_key(prog->variable_map, LOOK_AT_PREV_WORD)){
        
        nlab_array* value = map_get_key_value(prog->variable_map,LOOK_AT_PREV_WORD);
        if(stack_push(prog->polish_stack, value)){
            return true;
        } else{
            SET_ERROR_STATE(error_interp);

            char dummy[MAX_STRING_LENGTH];
            dummy[0] = '\0';
            strcat(dummy, "Unable to add variable ");
            strcat(dummy, LOOK_AT_PREV_WORD);
            strcat(dummy, "into stack");
            set_error_msg(prog, dummy);
            return false;
        }

    } else {
        SET_ERROR_STATE(error_interp);
        char message[MAX_STRING_LENGTH];
        message[0] = '\0';
        strcat(message, "illegal use of uninitialized variable 2: \'");
        strcat(message, LOOK_AT_PREV_WORD);
        strcat(message, "\'");
        set_error_msg(prog, message);
        return false;
    }

    return true;
}

bool _add_value_to_map(Program* prog, char* key, nlab_array* array){
    if(map_add(prog->variable_map, key, array)){
        // pass-by-value, so free pointer on this side
        nlab_array_free(array);
        return true;
    } else {
        if(array != NULL){
            nlab_array_free(array);

            SET_ERROR_STATE(error_interp);

            char dummy[MAX_STRING_LENGTH];
            dummy[0] = '\0';
            strcat(dummy, "unable to add token ");
            strcat(dummy, LOOK_AT_PREV_WORD);
            strcat(dummy, " into variable map");
            set_error_msg(prog, dummy);
            
        }
    }
    return false;
}

bool interp_u_not(Program* prog){

    if(prog == NULL){
        return false;
    }

    if(prog->polish_stack->size > 0){
        nlab_array* pop = stack_pop(prog->polish_stack);

        nlab_array* result = nlab_array_copy(pop);

        if(result == NULL){
            return false;
        }

        for(int y = 0; y < (int) pop->rows; y++){
            for(int x = 0; x < (int) pop->cols; x++){
                if(pop->array[y][x] == false){
                    result->array[y][x] = true;
                } else {
                    result->array[y][x] = false;
                }
            }
        }

        stack_push(prog->polish_stack, result);
        // pass-by-value, so free result on this side as a copy is passed to stack
        nlab_array_free(result);

        return true;
    }
    return false;
}

bool interp_u_eightcount(Program* prog){

    if(prog == NULL || prog->polish_stack == NULL){
        return false;
    }

    if(prog->polish_stack->size > 0){
        nlab_array* pop = stack_pop(prog->polish_stack);
        nlab_array* result = nlab_array_copy(pop);

        if(result == NULL){
            return false;
        }

        for(int y = 0; y < (int) pop->rows; y++){
            for(int x = 0; x < (int) pop->cols; x++){
                result->array[y][x] = _calc_moore_neighbourhood(pop, x, y);
            }
        }

        stack_push(prog->polish_stack, result);
        // pass-by-value, so free result on this side as a copy is passed to stack
        nlab_array_free(result);

        return true;
    }

    return false;
}

/* UNARY EXTENTION OPERATIONS */
#ifdef EXTENSION

bool extension_u_trace(Program* prog){

    if(prog == NULL || prog->polish_stack == NULL){
        return false;
    }

    unsigned int trace_count;
    nlab_array* peek;
    nlab_array* pop;
    nlab_array* result;

    peek = NULL;

    if(prog->polish_stack->size > 0){
        peek = stack_peek(prog->polish_stack);
        if(peek != NULL){
            if(peek->rows == peek->cols){
                trace_count = 0;
                pop = stack_pop(prog->polish_stack);

                for(unsigned int diagonal = 0; diagonal < pop->rows; diagonal++){
                    trace_count += pop->array[diagonal][diagonal];
                }

                result = nlab_array_create_1d(trace_count);
                stack_push(prog->polish_stack, result);
                // pass-by-value, so free result on this side as a copy is passed to stack
                nlab_array_free(result);
                return true;
            }

        }
    }

    SET_ERROR_STATE(error_interp);
    char dummy[MAX_STRING_LENGTH];
    dummy[0] = '\0';
    strcat(dummy, "unable to interpret trace of matrix");
    set_error_msg(prog, dummy);
    return false;
}

bool extension_u_transpose(Program* prog){


    nlab_array* pop;
    nlab_array* result;

    if(prog == NULL || prog->polish_stack == NULL){
        return false;
    }

    if(prog->polish_stack->size > 0){

        pop = stack_pop(prog->polish_stack);

        result = nlab_array_create_ones(pop->cols, pop->rows);

        for(unsigned int y = 0; y < pop->rows; y++){
            for(unsigned int x = 0; x < pop->cols; x++){
                result->array[x][y] = pop->array[y][x];
            }
        }

        stack_push(prog->polish_stack, result);
        // pass-by-value, so free result on this side as a copy is passed to stack
        nlab_array_free(result);
        return true;
    }

    SET_ERROR_STATE(error_interp);
    char dummy[MAX_STRING_LENGTH];
    dummy[0] = '\0';
    strcat(dummy, "unable to interpret tranposition of matrix");
    set_error_msg(prog, dummy);
    return false;
}

bool extension_u_submatrix(Program* prog){
    nlab_array* popped_arr;
    nlab_array* cols;
    nlab_array* rows;
    nlab_array* result;
    unsigned int rm_row;
    unsigned int rm_col;
    unsigned int new_rows, new_cols, zero_based_rm_row, zero_based_rm_col, current_row, current_col;

    if(prog == NULL || prog->polish_stack == NULL){
        return false;
    }

    short num_of_operands = 3;

    if(prog->polish_stack->size == num_of_operands){

        popped_arr = stack_pop(prog->polish_stack);
        cols = stack_pop(prog->polish_stack);
        rows = stack_pop(prog->polish_stack);

        rm_row = rows->array[0][0];
        rm_col = cols->array[0][0];

        if(!(rm_row > popped_arr->rows) && (rm_row > 0) 
        && !(rm_col > popped_arr->cols) && (rm_col > 0)){

            zero_based_rm_row = rm_row - 1;
            zero_based_rm_col = rm_col - 1;
            new_rows = popped_arr->rows-1;
            new_cols = popped_arr->cols-1;


            result = nlab_array_create_ones(new_rows, new_cols);
            current_row = current_col = 0;
            for(unsigned int y = 0; y < popped_arr->rows; y++){
                if(y != zero_based_rm_row){
                    for(unsigned int x = 0; x < popped_arr->cols; x++){
                        if(x != zero_based_rm_col){
                            result->array[current_row][current_col] = popped_arr->array[y][x];
                            current_col++;
                        }
                    }
                    current_row++;
                }
                current_col = 0;
            }

            stack_push(prog->polish_stack, result);
            // pass-by-value, so free result on this side as a copy is passed to stack
            nlab_array_free(result);
            return true;
        }
    }

    SET_ERROR_STATE(error_interp);
    char dummy[MAX_STRING_LENGTH];
    dummy[0] = '\0';
    strcat(dummy, "unable to interpret submatrix");
    set_error_msg(prog, dummy);
    return false;

}


#endif

int _calc_moore_neighbourhood(nlab_array* nlab, int mid_x, int mid_y){

    short counter = 0;

    for(int localy = mid_y-1; localy <= mid_y+1; localy++){
        for(int localx = mid_x-1; localx <= mid_x+1; localx++){
            if((localy >= 0 && localx >= 0) 
            && (localy < (int) nlab->rows && localx < (int) nlab->cols)){
                if(!(localy == mid_y && localx == mid_x)){ 
                    if(nlab->array[localy][localx] == true){
                        counter++;
                    }
                } 
            }
        }
    }
    
    return counter;
}


bool _do_binary_operation(Program* prog, binary_op operation_type){

    bool is_op1_scalar, is_op2_scalar;
    nlab_array *operand1, *operand2;
    unsigned int single_dim;

    
    if(prog == NULL || prog->polish_stack == NULL){
        return false;
    }

    // operand2 is pushed after operand1, so operand2 is popped first!
    operand2 = stack_pop(prog->polish_stack);
    operand1 = stack_pop(prog->polish_stack);

    if(operand1 == NULL || operand2 == NULL){
        return false;
    }

    is_op1_scalar = is_op2_scalar = false;
    single_dim = 1;

    if(operand1->rows == single_dim && operand1->cols == single_dim){
        is_op1_scalar = true;
    }

    if(operand2->rows == single_dim && operand2->cols == single_dim){
        is_op2_scalar = true;
    }

    nlab_array* result = NULL;

    if(is_op1_scalar == false && is_op2_scalar == true){
        result = _binop_scalar_vector(operand2, operand1, operation_type);

    } else if(is_op1_scalar == true && is_op2_scalar == false){
        result = _binop_scalar_vector(operand1, operand2, operation_type);

    } else if(is_op1_scalar == false && is_op2_scalar == false) {
        result = _binop_vector_vector(operand1, operand2, operation_type);

    } else if(is_op1_scalar == true && is_op2_scalar == true) {
        result = _binop_scalar_scalar(operand1, operand2, operation_type);
    }

    if(result == NULL){
        return false;
    }

    stack_push(prog->polish_stack, result);
    // pass-by-value
    nlab_array_free(result);

    return true;
}

bool interp_b_and(Program* prog){
    if(_do_binary_operation(prog, binop_and)){
        return true;
    }
    return false;
}

bool interp_b_or(Program* prog){
    if(_do_binary_operation(prog, binop_or)){
        return true;
    }
    return false;
}

bool interp_b_greater(Program* prog){
    if(_do_binary_operation(prog, binop_greater)){
        return true;
    }
    return false;
}

bool interp_b_less(Program* prog){
    if(_do_binary_operation(prog, binop_less)){
        return true;
    }
    return false;
}

bool interp_b_times(Program* prog){
    if(_do_binary_operation(prog, binop_times)){
        return true;
    }
    return false;
}

bool interp_b_add(Program* prog){
    if(_do_binary_operation(prog, binop_add)){
        return true;
    }
    return false;
}

bool interp_b_equals(Program* prog){
    if(_do_binary_operation(prog, binop_equals)){
        return true;
    }
    return false;
}

/* EXTENSION BINARY OPERATIONS*/
#ifdef EXTENSION
bool extension_b_dotproduct(Program* prog){
    if(_do_binary_operation(prog, binop_dotproduct)){
        return true;
    }
    return false;
}


bool extension_b_power(Program* prog){

    nlab_array* power_arr;
    nlab_array* orig_vector;
    short power, scalar_dims, power_of_one, power_of_ten;

    power_arr = stack_pop(prog->polish_stack);
    power = power_arr->array[0][0];

    orig_vector = nlab_array_copy(stack_peek(prog->polish_stack));

    scalar_dims = 1;
    power_of_one = 1;
    power_of_ten = 10;

    if(((short) power_arr->cols == scalar_dims) 
    && ((short) power_arr->rows == scalar_dims) 
    && (power == power_of_one)){
        nlab_array_free(orig_vector);
        return true;

    } else if(((short) power_arr->cols == scalar_dims) 
    && ((short) power_arr->rows == scalar_dims) 
    && (power > power_of_one)
    && (power <= power_of_ten)){

        while(power > power_of_one){
            
            stack_push(prog->polish_stack, orig_vector);
            if(!_do_binary_operation(prog, binop_dotproduct)){
                nlab_array_free(orig_vector);
                return false;
            }

            power--;
        }
        nlab_array_free(orig_vector);
        return true;
    }
    nlab_array_free(orig_vector);
    return false;
}
#endif

nlab_array* _binop_scalar_vector(nlab_array* scalar, nlab_array* vector, binary_op operation_type){
    
    nlab_array* result;
    
    if(scalar == NULL || vector == NULL){
        return NULL;
    }

    result = nlab_array_copy(vector);
            
    if(operation_type == binop_and){
        for(unsigned int y = 0; y < vector->rows; y++){
            for(unsigned int x = 0; x < vector->cols; x++){
                result->array[y][x] = vector->array[y][x] &&  scalar->array[0][0];
            }
        }
    } else if(operation_type == binop_or){
        for(unsigned int y = 0; y < vector->rows; y++){
            for(unsigned int x = 0; x < vector->cols; x++){
                result->array[y][x] = vector->array[y][x] ||  scalar->array[0][0];
            }
        }
    } else if(operation_type == binop_greater){
        for(unsigned int y = 0; y < vector->rows; y++){
            for(unsigned int x = 0; x < vector->cols; x++){
                if(vector->array[y][x] > scalar->array[0][0]){
                    result->array[y][x] = true;
                } else{
                    result->array[y][x] = false;
                }
            }
        }
    } else if(operation_type == binop_less){
        for(unsigned int y = 0; y < vector->rows; y++){
            for(unsigned int x = 0; x < vector->cols; x++){
                if(vector->array[y][x] < scalar->array[0][0]){
                    result->array[y][x] = true;
                } else{
                    result->array[y][x] = false;
                }
            }
        }
    } else if(operation_type == binop_add){
        for(unsigned int y = 0; y < vector->rows; y++){
            for(unsigned int x = 0; x < vector->cols; x++){
                result->array[y][x] = vector->array[y][x] + scalar->array[0][0];
            }
        }
    } else if(operation_type == binop_times){
        for(unsigned int y = 0; y < vector->rows; y++){
            for(unsigned int x = 0; x < vector->cols; x++){
                result->array[y][x] = vector->array[y][x] * scalar->array[0][0];
            }
        }
    } else if(operation_type == binop_equals){
        for(unsigned int y = 0; y < vector->rows; y++){
            for(unsigned int x = 0; x < vector->cols; x++){
                if(vector->array[y][x] == scalar->array[0][0]){
                    result->array[y][x] = true;
                } else{
                    result->array[y][x] = false;
                }
            }
        }
    } else {
        nlab_array_free(result);
        return NULL;
    }

    return result;
}

nlab_array* _binop_vector_vector(nlab_array* v1, nlab_array* v2, binary_op operation_type){
    
    nlab_array* result;
    
    if(v1 == NULL || v2 == NULL){
        return NULL;
    }

    result = nlab_array_copy(v1);

    if(operation_type == binop_and){
        if(v1->rows == v2->rows && v1->cols == v2->cols){
            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    result->array[y][x] = v1->array[y][x] &&  v2->array[y][x];
                } 
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } else if(operation_type == binop_or){
        if(v1->rows == v2->rows && v1->cols == v2->cols){
            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    result->array[y][x] = v1->array[y][x] ||  v2->array[y][x];
                } 
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } else if(operation_type == binop_greater){
        if(v1->rows == v2->rows && v1->cols == v2->cols){
            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    if(v1->array[y][x] > v2->array[y][x]){
                        result->array[y][x] = true;
                    } else{
                        result->array[y][x] = false;
                    }
                }
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } else if(operation_type == binop_less){
        if(v1->rows == v2->rows && v1->cols == v2->cols){
            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    if(v1->array[y][x] < v2->array[y][x]){
                        result->array[y][x] = true;
                    } else{
                        result->array[y][x] = false;
                    }
                }
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } else if(operation_type == binop_add){
        if(v1->rows == v2->rows && v1->cols == v2->cols){
            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    result->array[y][x] = v1->array[y][x] + v2->array[y][x];
                }
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } else if(operation_type == binop_times){
        if(v1->rows == v2->rows && v1->cols == v2->cols){
            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    result->array[y][x] = v1->array[y][x] * v2->array[y][x];
                }
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } else if(operation_type == binop_equals){
        if(v1->rows == v2->rows && v1->cols == v2->cols){
            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    if(v1->array[y][x] == v2->array[y][x]){
                        result->array[y][x] = true;
                    } else{
                        result->array[y][x] = false;
                    }
                }
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } 
    #ifdef EXTENSION
    else if(operation_type == binop_dotproduct){
        if(v1->cols == v2->rows){
            nlab_array_free(result);
            unsigned int counter;
            result = nlab_array_create_ones(v1->rows, v2->cols);

            for(unsigned int y = 0; y < result->rows; y++){
                for(unsigned int x = 0; x < result->cols; x++){
                    counter = 0;
                    result->array[y][x] = 0;
                    while(counter < v1->cols){
                        result->array[y][x] += v1->array[y][counter] * v2->array[counter][x];
                        counter++;
                    }
                }
            }
        } else {
            nlab_array_free(result);
            return NULL;
        }
    } 

    #endif
    else {
        nlab_array_free(result);
        return NULL;
    }

    return result;
}

nlab_array* _binop_scalar_scalar(nlab_array* s1, nlab_array* s2, binary_op operation_type){    
    nlab_array* result;
    
    if(s1 == NULL || s2 == NULL){
        return NULL;
    }

    result = nlab_array_copy(s1);

    if(operation_type == binop_and){
        result->array[0][0] = s1->array[0][0] &&  s2->array[0][0];
    } else  if(operation_type == binop_or){
        result->array[0][0] = s1->array[0][0] ||  s2->array[0][0];
    } else if(operation_type == binop_greater){
        if(s1->array[0][0] > s2->array[0][0]){
            result->array[0][0] = true;
        } else{
            result->array[0][0] = false;
        }
    } else if(operation_type == binop_less){
        if(s1->array[0][0] < s2->array[0][0]){
            result->array[0][0] = true;
        } else{
            result->array[0][0] = false;
        }
    } else if(operation_type == binop_add){
        result->array[0][0] = s1->array[0][0] + s2->array[0][0];
    } else if(operation_type == binop_times){
        result->array[0][0] = s1->array[0][0] * s2->array[0][0];
    } else if(operation_type == binop_equals){
        if(s1->array[0][0] == s2->array[0][0]){
            result->array[0][0] = true;
        } else{
            result->array[0][0] = false;
        }
    } else {
        nlab_array_free(result);
        return NULL;
    }

    return result;
}

char* _interp_get_var_context(Program* prog){

    int currentword;

    currentword = prog->current_token;

    while(currentword >= 0){
        if(STRINGS_EQUAL(prog->tokens[currentword],"SET")){
            currentword++;
            return prog->tokens[currentword];
        }
        currentword--;
    }
    return  NULL;
}

bool interp_create_ones(Program* prog, char* key, nlab_array* ones_array){

    if(prog != NULL && prog->variable_map!= NULL && ones_array != NULL){
        if(_add_value_to_map(prog, key, ones_array)){
            return true;
        }
    }

    return false;
}


bool interp_create_read(Program* prog, char* key, nlab_array* narray, char* filename){

    FILE* fp;   
    int element;
    unsigned int num_rows, num_cols, num_elements;
    short scan_arr_dims, scan_single_element;

    if(_format_filename(filename)){
        fp = fopen(filename, "rt");

        if(fp == NULL){
            return false;
        }

        if(_is_correct_file_extention(filename,".arr")){
            
            scan_arr_dims = 2;
            num_rows = num_cols = 0;

            if(fscanf(fp, "%u %u\n", &num_rows, &num_cols) == scan_arr_dims){

                unsigned int y, x;

                narray = nlab_array_create_ones(num_rows, num_cols);

                scan_single_element = 1;
                x = y = element = num_elements = 0;
            
                while(fscanf(fp, "%d", &element) == scan_single_element){
                    if(element < 0){
                        return false;
                    }
                    if(num_elements == (num_cols*num_rows)){
                        nlab_array_free(narray);
                        fclose(fp);
                        return false;
                    }
                    narray->array[y][x] = element;
                    x++;
                    num_elements++;
                    if(x%num_cols == 0){
                        y++;
                        x = 0;
                    }
                }

                if(num_elements == (num_cols*num_rows)){
                    if(_add_value_to_map(prog, key, narray)){
                        fclose(fp);
                        return true;
                    }
                }
            }
        nlab_array_free(narray);
        
        }
        fclose(fp);
    }

    return false;
}
