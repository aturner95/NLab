#include "../src/nlab.h"

/*
    TOP LEVEL TEST FUNCTION
*/

void test_nlab(void){

    /* General function tests*/
    test_program_builder_init();
    test_program_builder_add();
    test_set_error_msg();
    test_word_to_integer();
    test_is_correct_file_extention();
    test_format_filename();

    /* Grammar tests */
    test_program();
    test_instrc_list();
    test_instrc();
    test_print();
    test_varname();
    test_string();
    test_set();
    test_polish_list();
    test_polish();
    test_pushdown();
    test_integer();
    test_unaryop();
    test_binaryop();
    test_create();
    test_rows();
    test_cols();
    test_filename();
    test_loop();

    /* Interp tests */
    test_interp_print_variable();
    test_interp_print_string();
    test_interp_create_read();
    test_interp_set();
    test_interp_get_var_context();
    test_interp_u_not();
    test_interp_u_eightcount();
    test_interp_b_and();
    test_interp_b_or();
    test_interp_b_greater();
    test_interp_b_less();
    test_interp_b_add();
    test_interp_b_times();
    test_interp_b_equals();
    test_interp_loop();
    
    test_binop_scalar_vector();
    test_binop_vector_vector();
    test_binop_scalar_scalar();

    /* Extension tests */
    #ifdef EXTENSION
    test_extension_u_trace();
    test_extension_u_transpose();
    test_extension_u_submatrix();
    test_extension_b_dotproduct();
    test_extension_b_power();
    #endif

}


/*
    TESTING THE SETUP FUNCTIONS
*/

void test_program_builder_init(void){
    Program* prog = program_builder_init();

    // ensure no sanitizer issues from indexing
    // expecting program size
    assert(prog->tokens[0]);
    assert(prog->tokens[MAX_NUM_OF_TOKENS - 1]);


    assert(prog->current_token == 0);
    assert(prog->num_of_tokens == 0);

    program_builder_free(prog);
}

void test_program_builder_add(void){
    // Test #1 - init a program and add in two tokens. Then, check they're in
    // the expected positions within the tokens array and test token count
    Program* prog = program_builder_init();
    assert(program_builder_add(prog, "PRINT"));
    assert(program_builder_add(prog, "$I"));

    assert(strcmp(prog->tokens[0], "PRINT") == 0);
    assert(strcmp(prog->tokens[1], "$I") == 0);
    assert(prog->num_of_tokens == 2);

    // unpopulated rows should be empty
    assert(strlen(prog->tokens[2]) == 0);

    // test content of populated rows
    assert(prog->tokens[0][0] == 'P');
    assert(prog->tokens[0][3] == 'N');
    assert(prog->tokens[1][0] == '$');

    // test adding another token in
    assert(program_builder_add(prog, "}"));
    assert(strcmp(prog->tokens[2], "}") == 0);
    assert(prog->num_of_tokens == 3);

    // test nulls
    assert(!program_builder_add(prog, NULL));
    assert(!program_builder_add(NULL, "}"));

    program_builder_free(prog);

}

/*
    TEST GENERAL FUNCTIONS
*/

void test_set_error_msg(void){

    // test #1 - set a valid messge
    Program* p1 = program_builder_init();
    char errmsg1[5] = "doof";
    assert(set_error_msg(p1, "doof"));
    assert(STRINGS_EQUAL(p1->error_msg, errmsg1));
    program_builder_free(p1);

    // test #2 - set another msg with a spec char
    Program* p2 = program_builder_init();
    char errmsg2[10] = "test\ntest";
    assert(set_error_msg(p2, "test\ntest"));
    assert(STRINGS_EQUAL(p2->error_msg, errmsg2));
    program_builder_free(p2);

    // test #3 - set an empty msg (returns false)
    Program* p3 = program_builder_init();
    assert(!set_error_msg(p3, ""));
    program_builder_free(p3);

    // test #3 - set a msg when msg already exists, (false)
    Program* p4 = program_builder_init();
    strcpy(p4->error_msg, "message1");
    assert(!set_error_msg(p4, "message2"));
    assert(!STRINGS_EQUAL(p4->error_msg, "message2"));
    assert(STRINGS_EQUAL(p4->error_msg, "message1"));
    program_builder_free(p4);

}


void test_word_to_integer(void){
    assert(word_to_integer("5") == 5);
    assert(word_to_integer("1000000") == 1000000);
    assert(word_to_integer("-5") == -1);
    assert(word_to_integer("minus seven") == -1);
    assert(word_to_integer("1f") == -1);
    assert(word_to_integer("0") == 0);
    assert(word_to_integer(NULL) == -1);
}

void test_is_correct_file_extention(void){
    // test #1 - a valid extention
    char* f1 = "file.arr";
    char* e1 = ".arr";
    assert(_is_correct_file_extention(f1,e1));

    // test #2 - another valid extention
    char* f2 = "../folder/file.arr";
    char* e2 = ".arr";
    assert(_is_correct_file_extention(f2,e2));

    // test #3 - invalid extention
    char* f3 = "file.txt";
    char* e3 = ".arr";
    assert(!_is_correct_file_extention(f3,e3));

    // test #4 - valid extention
    char* f4 = "file.txt";
    char* e4 = ".txt";
    assert(_is_correct_file_extention(f4,e4));

    // test #5 - test NULL
    assert(!_is_correct_file_extention(f1,NULL));

}

void test_format_filename(void){

    // test#1 - valid filename
    char* fname1 = malloc(sizeof(char)*11);
    strcpy(fname1, "\"test.arr\"");
    assert(_format_filename(fname1));
    assert(STRINGS_EQUAL(fname1, "test.arr"));

    // test #2 - invalid filename
    char* fname2 = malloc(sizeof(char)*11);
    strcpy(fname2, "test.arr");
    assert(!_format_filename(fname1));

    // test #3 - NULL
    assert(!_format_filename(NULL));

    free(fname1);
    free(fname2);
}

/*
   TESTING GRAMMAR FUNCTIONS
*/

void test_program(void){

    // test #1 - example 1 from brief
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "BEGIN"));
    assert(program_builder_add(p1, "{"));
    assert(program_builder_add(p1, "SET"));
    assert(program_builder_add(p1, "$I"));
    assert(program_builder_add(p1, ":="));
    assert(program_builder_add(p1, "5"));
    assert(program_builder_add(p1, ";"));
    assert(program_builder_add(p1, "PRINT"));
    assert(program_builder_add(p1, "$I"));
    assert(program_builder_add(p1, "}"));
    assert(program(p1));
    assert(strlen(p1->error_msg) == 0);
    program_builder_free(p1);

    // test #2 - example 2 from brief
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "BEGIN"));
    assert(program_builder_add(p2, "{"));
    assert(program_builder_add(p2, "ONES"));
    assert(program_builder_add(p2, "6"));
    assert(program_builder_add(p2, "5"));
    assert(program_builder_add(p2, "$A"));
    assert(program_builder_add(p2, "SET"));
    assert(program_builder_add(p2, "$A"));
    assert(program_builder_add(p2, ":="));
    assert(program_builder_add(p2, "$A"));
    assert(program_builder_add(p2, "2"));
    assert(program_builder_add(p2, "B-ADD"));
    assert(program_builder_add(p2, ";"));
    assert(program_builder_add(p2, "PRINT"));
    assert(program_builder_add(p2, "\"ARRAY:\""));
    assert(program_builder_add(p2, "PRINT"));
    assert(program_builder_add(p2, "$A"));
    assert(program_builder_add(p2, "}"));
    assert(program(p2));
    assert(strlen(p2->error_msg) == 0);
    program_builder_free(p2);

    // test #3 - example 3 (factorials i.e., 10!) from brief
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "BEGIN"));
    assert(program_builder_add(p3, "{"));
    assert(program_builder_add(p3, "SET"));
    assert(program_builder_add(p3, "$F"));
    assert(program_builder_add(p3, ":="));
    assert(program_builder_add(p3, "1"));
    assert(program_builder_add(p3, ";"));
    assert(program_builder_add(p3, "LOOP"));
    assert(program_builder_add(p3, "$I"));
    assert(program_builder_add(p3, "10"));
    assert(program_builder_add(p3, "{"));
    assert(program_builder_add(p3, "SET"));
    assert(program_builder_add(p3, "$F"));
    assert(program_builder_add(p3, ":="));
    assert(program_builder_add(p3, "$F"));
    assert(program_builder_add(p3, "$I"));
    assert(program_builder_add(p3, "B-TIMES"));
    assert(program_builder_add(p3, ";"));
    assert(program_builder_add(p3, "PRINT"));
    assert(program_builder_add(p3, "$F"));
    assert(program_builder_add(p3, "}"));
    assert(program_builder_add(p3, "}"));
    assert(program(p3));
    assert(strlen(p3->error_msg) == 0);
    program_builder_free(p3);

    // test #4 - PROG grammar wrong
    Program* p4 = program_builder_init();
    assert(program_builder_add(p4, "BEGIN"));
    // missing bracket
    assert(program_builder_add(p4, "SET"));
    assert(program_builder_add(p4, "$F"));
    assert(program_builder_add(p4, ":="));
    assert(program_builder_add(p4, "1"));
    assert(program_builder_add(p4, ";"));
    assert(program_builder_add(p4, "}"));
    assert(!program(p4));
    assert(STRINGS_EQUAL(p4->error_msg, "\"BEGIN\" \"{\" <INSTRCLIST>"));
    program_builder_free(p4);

    #ifdef INTERP
   /*   test #5 - whole program that interperates a loop
        Here we set $A to 1 and multiply itself by the $J counter
        1*1=1
        2*1=2
        3*2=6
        4*4=24
        5*24=120
   */ 
    Program* p5 = program_builder_init();
    program_builder_add(p5, "BEGIN");
    program_builder_add(p5, "{");
    program_builder_add(p5, "SET");
    program_builder_add(p5, "$A");
    program_builder_add(p5, ":=");
    program_builder_add(p5, "1");
    program_builder_add(p5, ";");
    program_builder_add(p5, "LOOP");
    program_builder_add(p5, "$J");
    program_builder_add(p5, "5");
    program_builder_add(p5, "{");
    program_builder_add(p5, "SET");
    program_builder_add(p5, "$A");
    program_builder_add(p5, ":=");
    program_builder_add(p5, "$A");
    program_builder_add(p5, "$J");
    program_builder_add(p5, "B-TIMES");
    program_builder_add(p5, ";");
    program_builder_add(p5, "PRINT");
    program_builder_add(p5, "$A");
    program_builder_add(p5, "}");
    program_builder_add(p5, "}");
    assert(program(p5));
    nlab_array* final = map_get_key_value(p5->variable_map, "$A");
    assert(final->array[0][0] == 120);
    program_builder_free(p5);

    // test #6 - testing a bug found interpretting Neill's example #4
    Program* p6 = program_builder_init();
    program_builder_add(p6, "BEGIN");
    program_builder_add(p6, "{");
    program_builder_add(p6, "SET");
    program_builder_add(p6, "$X");
    program_builder_add(p6, ":=");
    program_builder_add(p6, "0");
    program_builder_add(p6, ";");
    program_builder_add(p6, "LOOP");
    program_builder_add(p6, "$I");
    program_builder_add(p6, "10");
    program_builder_add(p6, "{");
    program_builder_add(p6, "SET");
    program_builder_add(p6, "$I");
    program_builder_add(p6, ":=");
    program_builder_add(p6, "$I");
    program_builder_add(p6, "1");
    program_builder_add(p6, "B-ADD");
    program_builder_add(p6, ";");
    program_builder_add(p6, "PRINT");
    program_builder_add(p6, "$I");
    program_builder_add(p6, "SET");
    program_builder_add(p6, "$X");
    program_builder_add(p6, ":=");
    program_builder_add(p6, "$I");
    program_builder_add(p6, ";");
    program_builder_add(p6, "}");
    program_builder_add(p6, "}");
    assert(program(p6));
    nlab_array* var_X = map_get_key_value(p6->variable_map, "$X");
    assert(var_X);
    assert(var_X->array[0][0] == 10);
    program_builder_free(p6);

    
   /*   test #7 - interp with a read in preperation for testing Life
   */ 
    Program* p7 = program_builder_init();
    program_builder_add(p7, "BEGIN");
    program_builder_add(p7, "{");
    program_builder_add(p7, "READ");
    program_builder_add(p7, "\"test/test1.arr\"");
    program_builder_add(p7, "$A");
    program_builder_add(p7, "LOOP");
    program_builder_add(p7, "$I");
    program_builder_add(p7, "10");
    program_builder_add(p7, "{");
    program_builder_add(p7, "SET");
    program_builder_add(p7, "$B");
    program_builder_add(p7, ":=");
    program_builder_add(p7, "$A");
    program_builder_add(p7, "U-EIGHTCOUNT");
    program_builder_add(p7, ";");
    program_builder_add(p7, "PRINT");
    program_builder_add(p7, "$A");
    program_builder_add(p7, "}");
    program_builder_add(p7, "}");
    assert(program(p7));
    program_builder_free(p7);

        // test #8 - example 3 (factorials i.e., 10!) from brief
    Program* p8 = program_builder_init();
    assert(program_builder_add(p8, "BEGIN"));
    assert(program_builder_add(p8, "{"));
    assert(program_builder_add(p8, "SET"));
    assert(program_builder_add(p8, "$F"));
    assert(program_builder_add(p8, ":="));
    assert(program_builder_add(p8, "1"));
    assert(program_builder_add(p8, ";"));
    assert(program_builder_add(p8, "LOOP"));
    assert(program_builder_add(p8, "$I"));
    assert(program_builder_add(p8, "10"));
    assert(program_builder_add(p8, "{"));
    assert(program_builder_add(p8, "SET"));
    assert(program_builder_add(p8, "$F"));
    assert(program_builder_add(p8, ":="));
    assert(program_builder_add(p8, "$F"));
    assert(program_builder_add(p8, "$I"));
    assert(program_builder_add(p8, "B-TIMES"));
    assert(program_builder_add(p8, ";"));
    assert(program_builder_add(p8, "PRINT"));
    assert(program_builder_add(p8, "$F"));
    assert(program_builder_add(p8, "}"));
    assert(program_builder_add(p8, "}"));
    assert(program(p8));
    assert(strlen(p8->error_msg) == 0);
    nlab_array* var_f = map_get_key_value(p8->variable_map, "$F");
    assert(var_f->array[0][0] == 3628800);
    program_builder_free(p8);
    #endif

}


void test_instrc_list(void){

    // test #1 - valid Instruction list brace
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "}"));
    assert(instrc_list(p1));
    program_builder_free(p1);

    // test #2 - (context free) test valid instruction list with PRINT
    #ifndef INTERP
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "PRINT"));
    assert(program_builder_add(p2, "$A"));
    assert(program_builder_add(p2, "}"));
    assert(instrc_list(p2));
    program_builder_free(p2);
    #endif

    #ifndef INTERP
    // test #3 - test valid instruction list with SET
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "SET"));
    assert(program_builder_add(p3, "$A"));
    assert(program_builder_add(p3, ":="));
    assert(program_builder_add(p3, "3"));
    assert(program_builder_add(p3, ";"));
    assert(program_builder_add(p3, "}"));
    assert(instrc_list(p3));
    program_builder_free(p3);
    #endif

    // test #4 - (context free) test valid instruction list with PRINT
    #ifndef INTERP
    Program* p4 = program_builder_init();
    assert(program_builder_add(p4, "PRINT"));
    assert(program_builder_add(p4, "\"ARRAY\""));
    assert(program_builder_add(p4, "PRINT"));
    assert(program_builder_add(p4, "$A"));
    assert(program_builder_add(p4, "}"));
    assert(instrc_list(p4));
    program_builder_free(p4);
    #endif

    // test #5 - test  valid instruction list with CREATE
    Program* p5 = program_builder_init();
    assert(program_builder_add(p5, "ONES"));
    assert(program_builder_add(p5, "2"));
    assert(program_builder_add(p5, "2"));
    assert(program_builder_add(p5, "$X"));
    assert(program_builder_add(p5, "}"));
    assert(instrc_list(p5));
    program_builder_free(p5);

    // test #6 - (context free) test  valid instruction list with nested LOOP
    #ifndef INTERP
    Program* p6 = program_builder_init();
    assert(program_builder_add(p6, "LOOP"));
    assert(program_builder_add(p6, "$I"));
    assert(program_builder_add(p6, "5"));
    assert(program_builder_add(p6, "{"));
    assert(program_builder_add(p6, "LOOP"));
    assert(program_builder_add(p6, "$J"));
    assert(program_builder_add(p6, "5"));
    assert(program_builder_add(p6, "{"));
    assert(program_builder_add(p6, "PRINT"));
    assert(program_builder_add(p6, "$A"));
    assert(program_builder_add(p6, "}"));
    assert(program_builder_add(p6, "}"));
    assert(program_builder_add(p6, "}"));
    assert(instrc_list(p6));
    program_builder_free(p6);
    #endif

    // test #7 - (context free) missing final brace here - have a brace for
    // either loop however need one more to finish INSTRC_LIST
    #ifndef INTERP
    Program* p7 = program_builder_init();
    assert(program_builder_add(p7, "LOOP"));
    assert(program_builder_add(p7, "$Y"));
    assert(program_builder_add(p7, "8"));
    assert(program_builder_add(p7, "{"));
    assert(program_builder_add(p7, "LOOP"));
    assert(program_builder_add(p7, "$X"));
    assert(program_builder_add(p7, "8"));
    assert(program_builder_add(p7, "{"));
    assert(program_builder_add(p7, "SET"));
    assert(program_builder_add(p7, "$A"));
    assert(program_builder_add(p7, ":="));
    assert(program_builder_add(p7, "2"));
    assert(program_builder_add(p7, ";"));
    assert(program_builder_add(p7, "}"));
    assert(program_builder_add(p7, "}"));
    // closing face for instrc list goes here
    assert(!instrc_list(p7));
    assert(STRINGS_EQUAL(p7->error_msg, "<INSTRCLIST> ::= \"}\" | <INSTRC> <INSTRCLIST>"));
    program_builder_free(p7);
    #endif


    // test #8 - (context free) testing instrc list from 
    // example 2 on breif
    #ifndef INTERP
    Program* p8 = program_builder_init();
    assert(program_builder_add(p8, "ONES"));
    assert(program_builder_add(p8, "6"));
    assert(program_builder_add(p8, "5"));
    assert(program_builder_add(p8, "$A"));
    assert(program_builder_add(p8, "SET"));
    assert(program_builder_add(p8, "$A"));
    assert(program_builder_add(p8, ":="));
    assert(program_builder_add(p8, "$A"));
    assert(program_builder_add(p8, "2"));
    assert(program_builder_add(p8, "B-ADD"));
    assert(program_builder_add(p8, ";"));
    assert(program_builder_add(p8, "PRINT"));
    assert(program_builder_add(p8, "\"ARRAY:\""));
    assert(program_builder_add(p8, "PRINT"));
    assert(program_builder_add(p8, "$A"));
    assert(program_builder_add(p8, "}"));
    assert(instrc_list(p8));
    program_builder_free(p8);
    #endif

    // test #9 - (context required) push var1 onto stack, then assign
    // var2=var1 and push var2 then try printing var2
    #ifndef INTERP
    Program* p9 = program_builder_init();
    assert(program_builder_add(p9, "SET"));
    assert(program_builder_add(p9, "$Y"));
    assert(program_builder_add(p9, ":="));
    assert(program_builder_add(p9, "5"));
    assert(program_builder_add(p9, ";"));
    assert(program_builder_add(p9, "SET"));
    assert(program_builder_add(p9, "$X"));
    assert(program_builder_add(p9, ":="));
    assert(program_builder_add(p9, "$Y"));
    assert(program_builder_add(p9, ";"));
    assert(program_builder_add(p9, "PRINT"));
    assert(program_builder_add(p9, "$X"));
    assert(program_builder_add(p9, "}"));
    assert(instrc_list(p9));
    program_builder_free(p9);
    #endif

    // test #10 - whilst the grammar is valid, $X isn't initialized
    // in the variable map, so fails if context is required
    #ifdef INTERP
    Program* p10 = program_builder_init();
    assert(program_builder_add(p10, "SET"));
    assert(program_builder_add(p10, "$I"));
    assert(program_builder_add(p10, ":="));
    assert(program_builder_add(p10, "5"));
    assert(program_builder_add(p10, ";"));
    assert(program_builder_add(p10, "SET"));
    assert(program_builder_add(p10, "$J"));
    assert(program_builder_add(p10, ":="));
    assert(program_builder_add(p10, "$X"));
    assert(program_builder_add(p10, ";"));
    assert(program_builder_add(p10, "PRINT"));
    assert(program_builder_add(p10, "$X"));
    assert(program_builder_add(p10, "}"));
    assert(!instrc_list(p10));
    program_builder_free(p10);
    #endif
}

void test_instrc(void){
    // test #1 - valid PRINT VARNAME
    #ifndef INTERP
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "PRINT"));
    assert(program_builder_add(p1, "$A"));
    assert(instrc(p1));
    program_builder_free(p1);
    #endif

    // test #2 - valid PRINT STRING
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "PRINT"));
    assert(program_builder_add(p2, "\"ARRAY\""));
    assert(instrc(p2));
    program_builder_free(p2);

    // test #3 - invalid PRINT VARNAME (missing $)
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "PRINT"));
    assert(program_builder_add(p3, "A"));
    assert(!instrc(p3));
    
    program_builder_free(p3);

    // test #4 - invalid PRINT STRING (missing "")
    Program* p4 = program_builder_init();
    assert(program_builder_add(p4, "PRINT"));
    assert(program_builder_add(p4, "ARRAY"));
    assert(!instrc(p4));
    program_builder_free(p4);

    // test #5 - valid CREATE ONES
    Program* p5 = program_builder_init();
    assert(program_builder_add(p5, "ONES"));
    assert(program_builder_add(p5, "4"));
    assert(program_builder_add(p5, "3"));
    assert(program_builder_add(p5, "$A"));
    assert(instrc(p5));
    program_builder_free(p5);

    // test #6 - valid CREATE READ
    #ifndef INTERP
    Program* p6 = program_builder_init();
    assert(program_builder_add(p6, "READ"));
    assert(program_builder_add(p6, "\"MYFILE.txt\""));
    assert(program_builder_add(p6, "$A"));
    assert(instrc(p6));
    program_builder_free(p6);
    #endif

    // test #7 - invalid CREATE ONES (varname more 
    // than 1 char)
    Program* p7 = program_builder_init();
    assert(program_builder_add(p7, "ONES"));
    assert(program_builder_add(p7, "4"));
    assert(program_builder_add(p7, "3"));
    assert(program_builder_add(p7, "$VAR"));
    assert(!instrc(p7));
    program_builder_free(p7);

    // test #8 - invalid CREATE READ (missing $)
    #ifndef INTERP
    Program* p8 = program_builder_init();
    assert(program_builder_add(p8, "READ"));
    assert(program_builder_add(p8, "\"MYFILE.txt\""));
    assert(program_builder_add(p8, "A"));
    assert(!instrc(p8));
    program_builder_free(p8);
    #endif

}

void test_print(void){

    Program* prog = program_builder_init();

    #ifndef INTERP
    // test #1 - (context free) print a variable - 
    assert(program_builder_add(prog, "PRINT"));
    assert(program_builder_add(prog, "$X"));
    assert(print(prog));
    #endif

    // test #2 - parse print a string
    assert(program_builder_add(prog, "PRINT"));
    assert(program_builder_add(prog, "\"ARRAY\""));
    assert(print(prog));

    // test #3 - parse invalid var name
    assert(program_builder_add(prog, "PRINT"));
    assert(program_builder_add(prog, "A"));
    assert(!print(prog));
    assert(STRINGS_EQUAL(prog->error_msg, "<PRINT> ::= \"PRINT\" <VARNAME> | \"PRINT\" <STRING>"));

    // test #4 - parse invalid string
    assert(program_builder_add(prog, "PRINT"));
    assert(program_builder_add(prog, "../../file.c"));
    assert(!print(prog));

    // test #5 - parse mispelt "PRINT"
    assert(program_builder_add(prog, "PRIN"));
    assert(program_builder_add(prog, "$A"));
    assert(!print(prog));

    // test #6 - test from invisble string in one of Neill's
    // examples that caught me out
    assert(program_builder_add(prog, "PRINT"));
    assert(program_builder_add(prog, "PRINT"));
    assert(!print(prog));
    assert(program_builder_add(prog, "PRINT"));
    assert(program_builder_add(prog, "\"ARRAY:\""));
    assert(!print(prog));

    program_builder_free(prog);
}


void test_varname(void){

    Program* prog = program_builder_init();

    // Test set #1 - valid cases
    assert(program_builder_add(prog, "$A"));
    assert(program_builder_add(prog, "$X"));
    assert(program_builder_add(prog, "$C"));
    assert(program_builder_add(prog, "$I"));
    assert(varname(prog));
    assert(varname(prog));
    assert(varname(prog));
    assert(varname(prog));

    // Test set #2 - invalid cases
    assert(program_builder_add(prog, "$a"));
    assert(program_builder_add(prog, "$INDEX"));
    assert(program_builder_add(prog, "$1"));
    assert(program_builder_add(prog, "$"));
    assert(program_builder_add(prog, "A"));
    assert(program_builder_add(prog, "ARRAY"));;
    assert(!varname(prog));
    prog->current_token++;
    assert(!varname(prog));
    prog->current_token++;
    assert(!varname(prog));
    prog->current_token++;
    assert(!varname(prog));
    prog->current_token++;
    assert(!varname(prog));
    prog->current_token++;
    assert(!varname(prog));

   program_builder_free(prog);

}

void test_string(void){

    Program* prog = program_builder_init();

    // Test set #1 - valid cases
    assert(program_builder_add(prog, "\"Hello!\""));
    assert(program_builder_add(prog, "\"../../doof.arr\""));
    assert(program_builder_add(prog, "\"ARRAY\""));
    assert(program_builder_add(prog, "\"$A\""));
    assert(string(prog));
    assert(string(prog));
    assert(string(prog));
    assert(string(prog));

    // Test set #2 - invalid cases
    assert(program_builder_add(prog, "Hello!"));
    assert(program_builder_add(prog, "../../doof.arr"));
    assert(program_builder_add(prog, "ARRAY"));
    assert(program_builder_add(prog, "$A"));
    assert(!string(prog));
    prog->current_token++;
    assert(!string(prog));
    prog->current_token++;
    assert(!string(prog));
    prog->current_token++;
    assert(!string(prog));
    prog->current_token++;

    program_builder_free(prog);
}

void test_set(){

    // test #1 - set a valid integer
    #ifndef INTERP
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "SET"));
    assert(program_builder_add(p1, "$A"));
    assert(program_builder_add(p1, ":="));
    assert(program_builder_add(p1, "5"));
    assert(program_builder_add(p1, ";"));
    assert(set(p1));
    program_builder_free(p1);
    #endif

    #ifndef INTERP
    // test #2 - (context free) set a binary multiplication
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "SET"));
    assert(program_builder_add(p2, "$A"));
    assert(program_builder_add(p2, ":="));
    assert(program_builder_add(p2, "$X"));
    assert(program_builder_add(p2, "$Y"));
    assert(program_builder_add(p2, "B-TIMES"));
    assert(program_builder_add(p2, ";"));
    assert(set(p2));
    program_builder_free(p2);
    #endif

    #ifndef INTERP
    // test #3 - (context free) missing semi colon
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "SET"));
    assert(program_builder_add(p3, "$X"));
    assert(program_builder_add(p3, ":="));
    assert(program_builder_add(p3, "$Y"));
    // semi-colon goes here
    assert(program_builder_add(p3, "}"));
    assert(!set(p3));
    assert(STRINGS_EQUAL(p3->error_msg, "<SET> ::= \"SET\" <VARNAME> \":=\" <POLISHLIST>"));
    program_builder_free(p3);
    #endif

    #ifndef INTERP
    // test #4 - (context free) incorrect assignment operator (should be ":=")
    Program* p4 = program_builder_init();
    assert(program_builder_add(p4, "SET"));
    assert(program_builder_add(p4, "$X"));
    assert(program_builder_add(p4, "="));
    assert(program_builder_add(p4, "3"));
    assert(program_builder_add(p4, ";"));
    assert(!set(p4));
    assert(STRINGS_EQUAL(p4->error_msg, "<SET> ::= \"SET\" <VARNAME> \":=\" <POLISHLIST>"));
    program_builder_free(p4);
    #endif

    #ifdef INTERP
    // test #8 - test combining parse and interp...!
    Program* p5 = program_builder_init();
    program_builder_add(p5, "SET");
    program_builder_add(p5, "$I");
    program_builder_add(p5, ":=");
    program_builder_add(p5, "5");
    program_builder_add(p5, ";");
    assert(set(p5));
    assert(map_contains_key(p5->variable_map, "$I"));
    nlab_array* variable = map_get_key_value(p5->variable_map, "$I");
    assert(variable->array[0][0] == 5);
    program_builder_free(p5);
    #endif

    #ifdef INTERP
    // test #6 - a chained list of operations and interpret
    // (interp-set can't test chained operations)
    // (from lifeb3s23) SET $C := $B 2 B-EQUALS $D B-OR
    Program* p6 = program_builder_init();
    program_builder_add(p6, "SET");
    program_builder_add(p6, "$C");
    program_builder_add(p6, ":=");
    program_builder_add(p6, "$B");
    program_builder_add(p6, "2");
    program_builder_add(p6, "B-EQUALS");
    program_builder_add(p6, "$D");
    program_builder_add(p6, "B-OR");
    program_builder_add(p6, ";");
    nlab_array* var_B = nlab_array_create_ones(5,5);
    var_B->array[0][0] = 0;
    var_B->array[0][1] = 0;
    var_B->array[0][2] = 0;
    var_B->array[0][3] = 0;
    var_B->array[0][4] = 0;
    var_B->array[1][0] = 1;
    var_B->array[1][1] = 2;
    var_B->array[1][2] = 3;
    var_B->array[1][3] = 2;
    var_B->array[1][4] = 1;
    var_B->array[2][0] = 1;
    var_B->array[2][1] = 1;
    var_B->array[2][2] = 2;
    var_B->array[2][3] = 1;
    var_B->array[2][4] = 1;
    var_B->array[3][0] = 1;
    var_B->array[3][1] = 2;
    var_B->array[3][2] = 3;
    var_B->array[3][3] = 2;
    var_B->array[3][4] = 1;
    var_B->array[4][0] = 0;
    var_B->array[4][1] = 0;
    var_B->array[4][2] = 0;
    var_B->array[4][3] = 0;
    var_B->array[4][4] = 0;
    nlab_array* var_D = nlab_array_create_ones(5,5);
    var_D->array[0][0] = 0;
    var_D->array[0][1] = 0;
    var_D->array[0][2] = 0;
    var_D->array[0][3] = 0;
    var_D->array[0][4] = 0;
    var_D->array[1][0] = 0;
    var_D->array[1][1] = 0;
    var_D->array[1][2] = 1;
    var_D->array[1][3] = 0;
    var_D->array[1][4] = 0;
    var_D->array[2][0] = 0;
    var_D->array[2][1] = 0;
    var_D->array[2][2] = 0;
    var_D->array[2][3] = 0;
    var_D->array[2][4] = 0;
    var_D->array[3][0] = 0;
    var_D->array[3][1] = 0;
    var_D->array[3][2] = 1;
    var_D->array[3][3] = 0;
    var_D->array[3][4] = 0;
    var_D->array[4][0] = 0;
    var_D->array[4][1] = 0;
    var_D->array[4][2] = 0;
    var_D->array[4][3] = 0;
    var_D->array[4][4] = 0;
    map_add(p6->variable_map, "$B", var_B);
    map_add(p6->variable_map, "$D", var_D);
    assert(set(p6));
    assert(p6->polish_stack->size == 0);
    assert(map_contains_key(p6->variable_map, "$C"));
    nlab_array* result = map_get_key_value(p6->variable_map, "$C");
    assert(result->array[0][0] == 0);
    assert(result->array[0][1] == 0);
    assert(result->array[0][2] == 0);
    assert(result->array[0][3] == 0);
    assert(result->array[0][4] == 0);
    assert(result->array[1][0] == 0);
    assert(result->array[1][1] == 1);
    assert(result->array[1][2] == 1);
    assert(result->array[1][3] == 1);
    assert(result->array[1][4] == 0);
    assert(result->array[2][0] == 0);
    assert(result->array[2][1] == 0);
    assert(result->array[2][2] == 1);
    assert(result->array[2][3] == 0);
    assert(result->array[2][4] == 0);
    assert(result->array[3][0] == 0);
    assert(result->array[3][1] == 1);
    assert(result->array[3][2] == 1);
    assert(result->array[3][3] == 1);
    assert(result->array[3][4] == 0);
    assert(result->array[4][0] == 0);
    assert(result->array[4][1] == 0);
    assert(result->array[4][2] == 0);
    assert(result->array[4][3] == 0);
    assert(result->array[4][4] == 0);
    nlab_array_free(var_B);
    nlab_array_free(var_D);
    program_builder_free(p6);
    #endif
}

void test_polish_list(){

    #ifndef INTERP // these tests only work when context free

    // test #1 - $A $B B-AND (valid)
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "$A"));
    assert(program_builder_add(p1, "$B"));
    assert(program_builder_add(p1, "B-AND"));
    assert(program_builder_add(p1, ";"));
    assert(polish_list(p1));
    program_builder_free(p1);

    // test #2 - $X $Y B-OR (valid)
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "$X"));
    assert(program_builder_add(p2, "$Y"));
    assert(program_builder_add(p2, "B-OR"));
    assert(program_builder_add(p2, ";"));
    assert(polish_list(p2));
    program_builder_free(p2);

    // test #3 - $I UNOT (valid)
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "$I"));
    assert(program_builder_add(p3, "U-NOT"));
    assert(program_builder_add(p3, ";"));
    assert(polish_list(p3));
    program_builder_free(p3);


    // test #4 - $A B-GREATER $B... not reverse polish notation
    // however is actually valid according to the grammar:
    // <POLISHLIST> ::= <POLISH><POLISHLIST> | ";"
    // <POLISH> ::= <PUSHDOWN> | <UNARYOP> | <BINARYOP>
    // <PUSHDOWN> ::== <VARNAME> | <INTEGER>
    Program* p4 = program_builder_init();
    assert(program_builder_add(p4, "$A"));
    assert(program_builder_add(p4, "B-GREATER"));
    assert(program_builder_add(p4, "$B"));
    assert(program_builder_add(p4, ";"));
    assert(polish_list(p4));
    program_builder_free(p4);

    // test #5 - $N $M "B-ADD" (invalid)
    Program* p5 = program_builder_init();
    assert(program_builder_add(p5, "$M"));
    assert(program_builder_add(p5, "$N"));
    assert(program_builder_add(p5, "\"B-ADD\""));
    assert(program_builder_add(p5, ";"));
    assert(!polish_list(p5));
    program_builder_free(p5);

    // test #6 - A B B-AND (invalid)
    Program* p6 = program_builder_init();
    assert(program_builder_add(p6, "A"));
    assert(program_builder_add(p6, "B"));
    assert(program_builder_add(p6, "B-AND"));
    assert(program_builder_add(p6, ";"));
    assert(!polish_list(p6));
    program_builder_free(p6);

    // test #7 - A B B-AND (missing ;)
    Program* p7 = program_builder_init();
    assert(program_builder_add(p7, "$A"));
    assert(program_builder_add(p7, "$B"));
    assert(program_builder_add(p7, "B-AND"));
    // should have ";" here, instead place with some other
    // character to stop seg fault
    assert(program_builder_add(p7, "}"));
    assert(!polish_list(p7));
    program_builder_free(p7);
    #endif

}

void test_polish(){

    // Test #1 - (context free) valid pushdown
    #ifndef INTERP
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "$A"));
    assert(polish(p1));
    program_builder_free(p1);

    // test #2 - valid unary op
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "U-NOT"));
    assert(polish(p2));
    program_builder_free(p2);
    

    // test #3 - valid binary op
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "B-ADD"));
    assert(polish(p3));
    program_builder_free(p3);
    #endif
}

void test_pushdown(){
    
    // test #1 - a valid integer
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "3"));
    assert(pushdown(p1));
    program_builder_free(p1);

    // test #2 - (context free) valid varname
    #ifndef INTERP
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "$A"));
    assert(pushdown(p2));
    program_builder_free(p2);
    #endif

    // test 3 - not an int
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "3.3"));
    assert(!pushdown(p3));
    program_builder_free(p3);

    // test 4 - var missing $
    Program* p4 = program_builder_init();
    assert(program_builder_add(p4, "A"));
    assert(!pushdown(p4));
    program_builder_free(p4);
}


void test_integer(void){
    Program* prog = program_builder_init();

    // test set #1 - some valid integers
    assert(program_builder_add(prog, "1"));
    assert(program_builder_add(prog, "23"));
    assert(program_builder_add(prog, "1001"));
    assert(program_builder_add(prog, "0"));
    assert(integer(prog));
    assert(integer(prog));
    assert(integer(prog));
    assert(integer(prog));

    // test set #2 - some invalid integers
    assert(program_builder_add(prog, "-1"));
    assert(program_builder_add(prog, "1.05"));
    assert(program_builder_add(prog, "string"));
    assert(program_builder_add(prog, "2+3"));
    assert(!integer(prog));
    prog->current_token++;
    assert(!integer(prog));
    prog->current_token++;
    assert(!integer(prog));
    prog->current_token++;
    assert(!integer(prog));

    program_builder_free(prog);
}

void test_unaryop(void){

    Program* prog = program_builder_init();

    // test set #1 - valid unary ops
    assert(program_builder_add(prog, "U-NOT"));
    assert(program_builder_add(prog, "U-EIGHTCOUNT"));
    assert(unaryop(prog));
    assert(unaryop(prog));

    // test set #2 - some invalid unary ops
    assert(program_builder_add(prog, "\"U-NOT\""));
    assert(program_builder_add(prog, "SPAM"));
    assert(!integer(prog));
    prog->current_token++;
    assert(!integer(prog));

    program_builder_free(prog);
}


void test_binaryop(void){
    Program* prog = program_builder_init();

    // test set #1 - valid unary ops
    assert(program_builder_add(prog, "B-AND"));
    assert(program_builder_add(prog, "B-EQUALS"));
    assert(binaryop(prog));
    assert(binaryop(prog));

    // test set #2 - some invalid unary ops
    assert(program_builder_add(prog, "ADD"));
    assert(program_builder_add(prog, "+"));
    assert(!binaryop(prog));
    prog->current_token++;
    assert(!binaryop(prog));

    program_builder_free(prog);
}


void test_create(void){

    // test #1 - create a valid array
    Program* prog1 = program_builder_init();
    assert(program_builder_add(prog1, "ONES"));
    assert(program_builder_add(prog1, "2"));
    assert(program_builder_add(prog1, "3"));
    assert(program_builder_add(prog1, "$A"));
    assert(create(prog1));
    assert(strlen(prog1->error_msg) == 0);
    program_builder_free(prog1);

    // test #2 - create a valid file
    #ifndef INTERP
    Program* prog2 = program_builder_init();
    assert(program_builder_add(prog2, "READ"));
    assert(program_builder_add(prog2, "\"FILENAME\""));
    assert(program_builder_add(prog2, "$X"));
    assert(create(prog2));
    assert(strlen(prog2->error_msg) == 0);
    program_builder_free(prog2);
    #endif

    // test #3 - filename is missing double quots
    Program* prog3 = program_builder_init();
    assert(program_builder_add(prog3, "READ"));
    assert(program_builder_add(prog3, "FILENAME"));
    assert(program_builder_add(prog3, "$X"));
    assert(!create(prog3));
    assert(STRINGS_EQUAL(prog3->error_msg, "<CREATE> ::= \"READ\" <FILENAME> <VARNAME>"));
    program_builder_free(prog3);

    /**  test #4 - array initialized incorrectly. We see no err msg as
    TWOS is not a valid instruction and so we don't set the msg in SET
    and allow the program to trace back up to INSTRUC_LIST, where it is
    set. **/
    Program* prog4 = program_builder_init();
    assert(program_builder_add(prog4, "TWOS"));
    assert(program_builder_add(prog4, "3"));
    assert(program_builder_add(prog4, "3"));
    assert(program_builder_add(prog4, "$A"));
    assert(!create(prog4));
    // assert(STRINGS_EQUAL(prog4->error_msg, "<INSTRCLIST> ::= \"}\" | <INSTRC> <INSTRCLIST>"));
    program_builder_free(prog4);

    // test #5 - array missing variable
    Program* prog5 = program_builder_init();
    assert(program_builder_add(prog5, "ONES"));
    assert(program_builder_add(prog5, "3"));
    assert(program_builder_add(prog5, "3"));
    assert(!create(prog5));
    assert(STRINGS_EQUAL(prog5->error_msg, "<CREATE> ::= \"ONES\" <ROWS> <COLS> <VARNAME>"));
    program_builder_free(prog5);

    // test #6 - attempted to create a 3x3 array
    Program* prog6 = program_builder_init();
    assert(program_builder_add(prog6, "ONES"));
    assert(program_builder_add(prog6, "3"));
    assert(program_builder_add(prog6, "3"));
    assert(program_builder_add(prog6, "3"));
    assert(program_builder_add(prog6, "$Y"));
    assert(!create(prog6));
    assert(STRINGS_EQUAL(prog6->error_msg, "<CREATE> ::= \"ONES\" <ROWS> <COLS> <VARNAME>"));
    program_builder_free(prog6);


    /*
        test #7 - interp a valid .arr file within /test folder - remember that executable lives in the top
        level of our workspace so arrays live in a subfolder
    */
    #ifdef INTERP
    Program* prog7 = program_builder_init();
    assert(program_builder_add(prog7, "READ"));
    assert(program_builder_add(prog7, "\"test/test1.arr\""));
    assert(program_builder_add(prog7, "$A"));
    assert(create(prog7));
    assert(map_contains_key(prog7->variable_map, "$A"));
    nlab_array* var_a = map_get_key_value(prog7->variable_map, "$A");
    assert(var_a->rows == 5);
    assert(var_a->cols == 5);
    assert(var_a->array[1][1] == 0);
    assert(var_a->array[2][2] == 1);
    assert(var_a->array[3][3] == 0);
    program_builder_free(prog7);
    #endif

    /*
        test #8 - interp a indvalid .arr file (this .arr has 3 dims on top row)
    */
    #ifdef INTERP
    Program* prog8 = program_builder_init();
    assert(program_builder_add(prog8, "READ"));
    assert(program_builder_add(prog8, "\"test/test3.arr\""));
    assert(program_builder_add(prog8, "$A"));
    assert(!create(prog8));
    program_builder_free(prog8);
    #endif

    // test #9 - bug found when trying to play Life...
    #ifdef INTERP
    Program* prog9 = program_builder_init();
    assert(program_builder_add(prog9, "READ"));
    assert(program_builder_add(prog9, "\"arrays/lglider.arr\""));
    assert(program_builder_add(prog9, "$A"));
    assert(create(prog9));
    program_builder_free(prog9);
    #endif
}

void test_rows(void){
    Program* prog = program_builder_init();

    // test set #1 - valid tests
    assert(program_builder_add(prog, "1"));
    assert(program_builder_add(prog, "23"));
    assert(program_builder_add(prog, "1001"));
    assert(program_builder_add(prog, "0"));
    assert(rows(prog));
    assert(rows(prog));
    assert(rows(prog));
    assert(rows(prog));

    // test set #2 - invalid tests
    assert(program_builder_add(prog, "-1"));
    assert(program_builder_add(prog, "1.05"));
    assert(program_builder_add(prog, "string"));
    assert(program_builder_add(prog, "2+3"));
    assert(!rows(prog));
    prog->current_token++;
    assert(!rows(prog));
    prog->current_token++;
    assert(!rows(prog));
    prog->current_token++;
    assert(!rows(prog));

    program_builder_free(prog);
}

void test_cols(void){
    Program* prog = program_builder_init();

    // test set#1 - valid tests
    assert(program_builder_add(prog, "1"));
    assert(program_builder_add(prog, "23"));
    assert(program_builder_add(prog, "1001"));
    assert(program_builder_add(prog, "0"));
    assert(cols(prog));
    assert(cols(prog));
    assert(cols(prog));
    assert(cols(prog));

    // test set #2 - valid tests
    assert(program_builder_add(prog, "-1"));
    assert(program_builder_add(prog, "1.05"));
    assert(program_builder_add(prog, "string"));
    assert(program_builder_add(prog, "2+3"));
    assert(!cols(prog));
    prog->current_token++;
    assert(!cols(prog));
    prog->current_token++;
    assert(!cols(prog));
    prog->current_token++;
    assert(!cols(prog));

    program_builder_free(prog);
}

void test_filename(void){

    Program* prog = program_builder_init();

    // Test set #1 - valid cases
    assert(program_builder_add(prog, "\"Hello!\""));
    assert(program_builder_add(prog, "\"../../doof.arr\""));
    assert(program_builder_add(prog, "\"ARRAY\""));
    assert(program_builder_add(prog, "\"$A\""));
    assert(filename(prog));
    assert(filename(prog));
    assert(filename(prog));
    assert(filename(prog));

    // Test set #2 - invalid cases
    assert(program_builder_add(prog, "Hello!"));
    assert(program_builder_add(prog, "../../doof.arr"));
    assert(program_builder_add(prog, "ARRAY"));
    assert(program_builder_add(prog, "$A"));
    assert(!filename(prog));
    prog->current_token++;
    assert(!filename(prog));
    prog->current_token++;
    assert(!filename(prog));
    prog->current_token++;
    assert(!filename(prog));
    prog->current_token++;

    program_builder_free(prog);
}

void test_loop(void){

    #ifndef INTERP
    // test #1 - a valid loop
    Program* p1 = program_builder_init();
    assert(program_builder_add(p1, "LOOP"));
    assert(program_builder_add(p1, "$A"));
    assert(program_builder_add(p1, "3"));
    assert(program_builder_add(p1, "{"));
    assert(program_builder_add(p1, "}"));
    assert(loop(p1));
    program_builder_free(p1);

    // test #2 - invalid variable
    Program* p2 = program_builder_init();
    assert(program_builder_add(p2, "LOOP"));
    assert(program_builder_add(p2, "A"));
    assert(program_builder_add(p2, "3"));
    assert(program_builder_add(p2, "{"));
    assert(program_builder_add(p2, "}"));
    assert(!loop(p2));
    assert(STRINGS_EQUAL(p2->error_msg, "<LOOP> ::= \"LOOP\" <VARNAME> <INTEGER> \"{\" <INSTRCLIST>"));
    program_builder_free(p2);

    // test #3 - an invalid integer
    Program* p3 = program_builder_init();
    assert(program_builder_add(p3, "LOOP"));
    assert(program_builder_add(p3, "$A"));
    assert(program_builder_add(p3, "3.5"));
    assert(program_builder_add(p3, "{"));
    assert(program_builder_add(p3, "}"));
    assert(!loop(p3));
    assert(STRINGS_EQUAL(p3->error_msg, "<LOOP> ::= \"LOOP\" <VARNAME> <INTEGER> \"{\" <INSTRCLIST>"));
    program_builder_free(p3);

    // test #4 - a missing bracket
    Program* p4 = program_builder_init();
    assert(program_builder_add(p4, "LOOP"));
    assert(program_builder_add(p4, "$A"));
    assert(program_builder_add(p4, "3"));
    assert(program_builder_add(p4, "}"));
    assert(!loop(p4));
    assert(STRINGS_EQUAL(p4->error_msg, "<LOOP> ::= \"LOOP\" <VARNAME> <INTEGER> \"{\" <INSTRCLIST>"));
    program_builder_free(p4);

    // test #5 - an invalid instruction list (String
    // missing "")
    Program* p5 = program_builder_init();
    assert(program_builder_add(p5, "LOOP"));
    assert(program_builder_add(p5, "$F"));
    assert(program_builder_add(p5, "3"));
    assert(program_builder_add(p5, "{"));
    assert(program_builder_add(p5, "PRINT"));
    assert(program_builder_add(p5, "ARRAY"));
    assert(program_builder_add(p5, "}"));
    assert(!loop(p5));
    assert(STRINGS_EQUAL(p5->error_msg, "<PRINT> ::= \"PRINT\" <VARNAME> | \"PRINT\" <STRING>"));
    program_builder_free(p5);

    // test #6 - loop starts incrementing from 1, so can't have 
    // the exit condition (int) as zero, however zero is a valid int
    Program* p6 = program_builder_init();
    assert(program_builder_add(p6, "LOOP"));
    assert(program_builder_add(p6, "$A"));
    assert(program_builder_add(p6, "0"));
    assert(program_builder_add(p6, "{"));
    assert(program_builder_add(p6, "}"));
    assert(!loop(p6));
    assert(STRINGS_EQUAL(p6->error_msg, "<LOOP> ::= \"LOOP\" <VARNAME> <INTEGER> \"{\" <INSTRCLIST>"));
    program_builder_free(p6);
    #endif

    #ifdef INTERP
    // test #7 - testing interp via parsing for an example that requires 
    // no context
    Program* p7 = program_builder_init();
    program_builder_add(p7, "LOOP");
    program_builder_add(p7, "$I");
    program_builder_add(p7, "10");
    program_builder_add(p7, "{");
    program_builder_add(p7, "PRINT");
    program_builder_add(p7, "\"MESSAGE\"");
    program_builder_add(p7, "}");
    assert(loop(p7));
    program_builder_free(p7);

    // test #8 - same as #7 but with a nested loop...
    Program* p8 = program_builder_init();
    program_builder_add(p8, "LOOP");
    program_builder_add(p8, "$I");
    program_builder_add(p8, "5");
    program_builder_add(p8, "{");
    program_builder_add(p8, "LOOP");
    program_builder_add(p8, "$J");
    program_builder_add(p8, "5");
    program_builder_add(p8, "{");
    program_builder_add(p8, "PRINT");
    program_builder_add(p8, "\"MESSAGE\"");
    program_builder_add(p8, "}");
    program_builder_add(p8, "}");
    assert(loop(p8));
    program_builder_free(p8);

    #endif

}

/* INTERPRETER TESTS */

void test_interp_print_variable(void){
    // test #1 - test variable
    Program* p1 = program_builder_init();
    nlab_array* arr1 = nlab_array_create_1d(7);
    map_add(p1->variable_map, "$A", arr1);
    char* msg1 = interp_print_variable(p1, "$A");
    assert(STRINGS_EQUAL(msg1, "7\n"));
    free(msg1);
    nlab_array_free(arr1);
    program_builder_free(p1);

    // test #2 - test ONES array
    Program* p2 = program_builder_init();
    nlab_array* arr2 = nlab_array_create_ones(3, 3);
    map_add(p2->variable_map, "$A", arr2);
    char* msg2 = interp_print_variable(p2, "$A");
    assert(STRINGS_EQUAL(msg2, "1 1 1\n1 1 1\n1 1 1\n\n"));
    free(msg2);
    nlab_array_free(arr2);
    program_builder_free(p2);
}

void test_interp_print_string(void){
    char* print1 = interp_print_string("\"PRINT\"");
    assert(STRINGS_EQUAL("PRINT", print1));
    char* print2 = interp_print_string("\"$I\"");
    assert(STRINGS_EQUAL("$I", print2));
    free(print1);
    free(print2);

}

void test_interp_create_read(void){

    // Tests aren't ideal as files are local and so aren't hard coded... but anyway

    // Test #1 - valid file
    Program* p1 = program_builder_init();
    nlab_array* arr1 = NULL;
    char* filename1 = malloc(sizeof(char) * 17);
    strcpy(filename1, "\"test/test1.arr\"");
    char* varname1 = "$F";

    assert(interp_create_read(p1, varname1, arr1, filename1));
    // arr1 gets added to Map, passed-by-copy and so arr1 is freed - we therefore check the map
    assert(!arr1);
    nlab_array* copy1 = map_get_key_value(p1->variable_map, "$F");
    assert(copy1->rows == 5);
    assert(copy1->cols == 5);
    assert(copy1->array[0][0] == 0);
    assert(copy1->array[1][1] == 0);
    assert(copy1->array[2][2] == 1);
    assert(copy1->array[3][3] == 0);
    assert(copy1->array[4][4] == 0);

    program_builder_free(p1);

    // Test #2 - inlvaid .txt extension
    Program* p2 = program_builder_init();
    nlab_array* arr2 = NULL;
    char* filename2 = malloc(sizeof(char) * 17);
    strcpy(filename2, "\"test/test2.txt\"");
    char* varname2 = "$F";
    assert(!interp_create_read(p2, varname2, arr2, filename2));
    program_builder_free(p2);

    // Test #3 - inlvaid format of 3 dims - not working yet!
    Program* p3 = program_builder_init();
    nlab_array* arr3 = NULL;
    char* filename3 = malloc(sizeof(char) * 17);
    strcpy(filename3, "\"test/test2.arr\"");
    char* varname3 = "$F";
    assert(!interp_create_read(p3, varname3, arr3, filename3));

    free(filename1);
    free(filename2);
    free(filename3);
    program_builder_free(p3);
}


void test_interp_set(void){

    /* 
        NOTE: the interp_set doesn't reach either interp_pushdown
        functions, so push vars onto stack manually.
    */

    // test #1 - set a variable.
    Program* p1 = program_builder_init();
    program_builder_add(p1, "SET");
    program_builder_add(p1, "$I");
    program_builder_add(p1, ":=");
    program_builder_add(p1, "5");
    program_builder_add(p1, ";");
    p1->current_token = 6;
    nlab_array* arr1 = nlab_array_create_1d(5);
    stack_push(p1->polish_stack, arr1);
    assert(interp_set(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* result1 = stack_peek(p1->polish_stack);
    assert(result1->array[0][0] == 5);
    nlab_array_free(arr1);
    assert(map_contains_key(p1->variable_map, "$I"));
    assert(map_get_key_value(p1->variable_map, "$I")->array[0][0] == 5);

    program_builder_free(p1);

    // test #2 - set 5 3 B-ADD
    Program* p2 = program_builder_init();
    program_builder_add(p2, "SET");
    program_builder_add(p2, "$I");
    program_builder_add(p2, ":=");
    program_builder_add(p2, "5");
    program_builder_add(p2, "3");
    program_builder_add(p2, "B-ADD");
    program_builder_add(p2, ";");
    p2->current_token = 6;
    nlab_array* arr2 = nlab_array_create_1d(5);
    nlab_array* arr3 = nlab_array_create_1d(3);
    stack_push(p2->polish_stack, arr2);
    stack_push(p2->polish_stack, arr3);
    assert(p2->polish_stack->size == 2);
    assert(interp_set(p2));
    assert(p2->polish_stack->size == 1);
    nlab_array* result2 = stack_peek(p2->polish_stack);
    assert(result2->array[0][0] == 8);
    nlab_array_free(arr2);
    nlab_array_free(arr3);
    assert(map_contains_key(p2->variable_map, "$I"));
    assert(map_get_key_value(p2->variable_map, "$I")->array[0][0] == 8);

    program_builder_free(p2);


    #ifdef INTERP
    // test #2 - set an array using B-TIMES and interpret
    Program* p3 = program_builder_init();
    program_builder_add(p3, "SET");
    program_builder_add(p3, "$I");
    program_builder_add(p3, ":=");
    program_builder_add(p3, "$X");
    program_builder_add(p3, "$Y");
    program_builder_add(p3, "B-TIMES");
    program_builder_add(p3, ";");
    p3->current_token = 6;
    nlab_array* arr4 = nlab_array_create_ones(3,3);
    arr4->array[0][0] = 0;
    arr4->array[0][1] = 1;
    arr4->array[0][2] = 2;
    arr4->array[1][0] = 3;
    arr4->array[1][1] = 4;
    arr4->array[1][2] = 5;
    arr4->array[2][0] = 6;
    arr4->array[2][1] = 7;
    arr4->array[2][2] = 8;
    nlab_array* arr5 = nlab_array_create_ones(3,3);
    arr5->array[0][0] = 8;
    arr5->array[0][1] = 7;
    arr5->array[0][2] = 6;
    arr5->array[1][0] = 5;
    arr5->array[1][1] = 4;
    arr5->array[1][2] = 3;
    arr5->array[2][0] = 2;
    arr5->array[2][1] = 1;
    arr5->array[2][2] = 0;
    stack_push(p3->polish_stack, arr4);
    stack_push(p3->polish_stack, arr5);
    assert(p3->polish_stack->size == 2);
    assert(interp_set(p3));
    assert(p3->polish_stack->size == 1);
    nlab_array* result3 = stack_peek(p3->polish_stack);
    assert(result3->array[0][0] == 0);
    assert(result3->array[0][1] == 7);
    assert(result3->array[0][2] == 12);
    assert(result3->array[1][0] == 15);
    assert(result3->array[1][1] == 16);
    assert(result3->array[1][2] == 15);
    assert(result3->array[2][0] == 12);
    assert(result3->array[2][1] == 7);
    assert(result3->array[2][2] == 0);
    assert(map_contains_key(p3->variable_map, "$I"));
    assert(map_get_key_value(p3->variable_map, "$I")->array[0][0] == 0);
    assert(map_get_key_value(p3->variable_map, "$I")->array[0][1] == 7);
    assert(map_get_key_value(p3->variable_map, "$I")->array[0][2] == 12);
    assert(map_get_key_value(p3->variable_map, "$I")->array[1][0] == 15);
    assert(map_get_key_value(p3->variable_map, "$I")->array[1][1] == 16);

    nlab_array_free(arr4);
    nlab_array_free(arr5);
    program_builder_free(p3);
    #endif


}

void test_interp_get_var_context(void){

    // test #1 - setting a variable to a scalar
    Program* p1 = program_builder_init();
    program_builder_add(p1, "SET");
    program_builder_add(p1, "$A");
    program_builder_add(p1, ":=");
    program_builder_add(p1, "3");
    program_builder_add(p1, ";");
    p1->current_token = 6;
    char* var1 = _interp_get_var_context(p1);
    assert(var1);
    assert(STRINGS_EQUAL(var1, "$A"));
    program_builder_free(p1);

    // test #2 - setting a vector
    Program* p2 = program_builder_init();
    program_builder_add(p2, "SET");
    program_builder_add(p2, "$X");
    program_builder_add(p2, ":=");
    program_builder_add(p2, "ONES");
    program_builder_add(p2, "5");
    program_builder_add(p2, "3");
    program_builder_add(p2, ";");
    p2->current_token = 8;
    char* var2 = _interp_get_var_context(p2);
    assert(var2);
    assert(STRINGS_EQUAL(var2, "$X"));
    program_builder_free(p2);

    // test #3 - setting a variable to a variable
    Program* p3 = program_builder_init();
    program_builder_add(p3, "SET");
    program_builder_add(p3, "$I");
    program_builder_add(p3, ":=");
    program_builder_add(p3, "$J");
    program_builder_add(p3, ";");
    p3->current_token = 6;
    char* var3 = _interp_get_var_context(p3);
    assert(var3);
    assert(STRINGS_EQUAL(var3, "$I"));
    program_builder_free(p3);

    // test #4 - setting a variable to an operation
    Program* p4 = program_builder_init();
    program_builder_add(p4, "SET");
    program_builder_add(p4, "$X");
    program_builder_add(p4, ":=");
    program_builder_add(p4, "$Y");
    program_builder_add(p4, "$Z");
    program_builder_add(p4, "B-TIMES");
    program_builder_add(p4, ";");
    p4->current_token = 8;
    char* var4 = _interp_get_var_context(p4);
    assert(var4);
    assert(STRINGS_EQUAL(var4, "$X"));
    program_builder_free(p4);

    // test #5 - setting a variable to chained operations
    Program* p5 = program_builder_init();
    program_builder_add(p5, "SET");
    program_builder_add(p5, "$X");
    program_builder_add(p5, ":=");
    program_builder_add(p5, "$A");
    program_builder_add(p5, "$B");
    program_builder_add(p5, "B-TIMES");
    program_builder_add(p5, "$C");
    program_builder_add(p5, "B-OR");
    program_builder_add(p5, ";");
    p5->current_token = 10;
    char* var5 = _interp_get_var_context(p5);
    assert(var5);
    assert(STRINGS_EQUAL(var5, "$X"));
    program_builder_free(p5);


}

void test_interp_u_not(void){

    // test #1 - a single array [[0, 1],[1, 0]] is on the stack.
    Program* p1 = program_builder_init();
    nlab_array* arr1 = nlab_array_create_ones(2,2);
    arr1->array[0][0] = 0;
    arr1->array[1][1] = 0;
    stack_push(p1->polish_stack, arr1);
    assert(p1->polish_stack->size == 1);

    assert(interp_u_not(p1));
    nlab_array* unot = stack_peek(p1->polish_stack);
    assert(p1->polish_stack->size == 1);
    assert(unot->array[0][0] == true);
    assert(unot->array[0][1] == false);
    assert(unot->array[1][0] == false);
    assert(unot->array[1][1] == true);

    // test #2 - try and NULL pointers
    assert(!interp_u_not(NULL));

    // test #3 - U-NOT on a non-boolean array
    Program* p2 = program_builder_init();
    nlab_array* arr2 = nlab_array_create_ones(2,2);
    arr2->array[0][0] = 2;
    arr2->array[0][1] = 4;
    arr2->array[1][0] = 6;
    arr2->array[1][1] = 8;
    stack_push(p2->polish_stack, arr2);
    assert(p2->polish_stack->size == 1);

    assert(interp_u_not(p2));
    nlab_array* unot2 = stack_peek(p2->polish_stack);
    assert(p2->polish_stack->size == 1);
    assert(unot2->array[0][0] == 0);
    assert(unot2->array[0][1] == 0);
    assert(unot2->array[1][0] == 0);
    assert(unot2->array[1][1] == 0);

    nlab_array_free(arr1);
    nlab_array_free(arr2);
    program_builder_free(p1);
    program_builder_free(p2);
}

/*
    returns number of trues around each cell in
    the array
*/
void test_interp_u_eightcount(void){

    // test #1 - eigen ones
    // [1 0 0]    [1 2 1]
    // [0 1 0] => [2 2 2]
    // [0 0 1]    [1 2 1]
    Program* p1 = program_builder_init();
    nlab_array* arr1 = nlab_array_create_ones(3,3);
    arr1->array[0][1] = 0;
    arr1->array[0][2] = 0;
    arr1->array[1][0] = 0;
    arr1->array[1][2] = 0;
    arr1->array[2][0] = 0;
    arr1->array[2][1] = 0;
    stack_push(p1->polish_stack, arr1);
    assert(p1->polish_stack->size == 1);

    assert(interp_u_eightcount(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* top = stack_peek(p1->polish_stack);
    assert(top->array[0][0] == 1);
    assert(top->array[0][1] == 2);
    assert(top->array[0][2] == 1);
    assert(top->array[1][0] == 2);
    assert(top->array[1][1] == 2);
    assert(top->array[1][2] == 2);
    assert(top->array[2][0] == 1);
    assert(top->array[2][1] == 2);
    assert(top->array[2][2] == 1);

    // test #2 - no items on stack
    nlab_array* popped = stack_pop(p1->polish_stack);
    assert(!interp_u_eightcount(p1));

    popped = NULL;
    nlab_array_free(arr1);
    program_builder_free(p1);
}

void test_interp_b_and(void){
    Program* p1 = program_builder_init();

    // test #1 - 60 13
    nlab_array* arr1 = nlab_array_create_1d(60);
    nlab_array* arr2 = nlab_array_create_1d(13);
    assert(stack_push(p1->polish_stack, arr1));
    assert(stack_push(p1->polish_stack, arr2));
    assert(p1->polish_stack->size == 2);
    assert(interp_b_and(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* res1 = stack_peek(p1->polish_stack);
    assert(res1->array[0][0] == 1);

    nlab_array_free(arr1);
    nlab_array_free(arr2);
    program_builder_free(p1);
}

void test_interp_b_or(void){
    // test #1 - OR scalars
    Program* p1 = program_builder_init();

    nlab_array* arr1 = nlab_array_create_1d(60);
    nlab_array* arr2 = nlab_array_create_1d(13);
    assert(stack_push(p1->polish_stack, arr1));
    assert(stack_push(p1->polish_stack, arr2));
    assert(p1->polish_stack->size == 2);
    assert(interp_b_or(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* res1 = stack_peek(p1->polish_stack);
    assert(res1->array[0][0] == 1);

    nlab_array_free(arr1);
    nlab_array_free(arr2);
    program_builder_free(p1);
}

void test_interp_b_greater(void){
    Program* p1 = program_builder_init();
    // test #1 - test two arrays
    nlab_array* arr1 = nlab_array_create_ones(2,2);
    arr1->array[0][0] = 4;
    arr1->array[0][1] = 7;
    arr1->array[1][0] = -1;
    arr1->array[1][1] = 4;
    nlab_array* arr2 = nlab_array_create_ones(2,2);
    arr2->array[0][0] = 0;
    arr2->array[0][1] = 3;
    arr2->array[1][0] = 5;
    arr2->array[1][1] = 4;
    assert(stack_push(p1->polish_stack, arr1));
    assert(stack_push(p1->polish_stack, arr2));
    assert(p1->polish_stack->size == 2);
    assert(interp_b_greater(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* result1 = stack_peek(p1->polish_stack);
    assert(result1->array[0][0] == 1);
    assert(result1->array[0][1] == 1);
    assert(result1->array[1][0] == 0);
    assert(result1->array[1][1] == 0);

    nlab_array_free(arr1);
    nlab_array_free(arr2);
    program_builder_free(p1);
}

void test_interp_b_less(void){
    Program* p1 = program_builder_init();
    // test #1 - test two arrays
    nlab_array* arr1 = nlab_array_create_ones(2,2);
    arr1->array[0][0] = 4;
    arr1->array[0][1] = 7;
    arr1->array[1][0] = -1;
    arr1->array[1][1] = 4;
    nlab_array* arr2 = nlab_array_create_ones(2,2);
    arr2->array[0][0] = 0;
    arr2->array[0][1] = 3;
    arr2->array[1][0] = 5;
    arr2->array[1][1] = 4;
    assert(stack_push(p1->polish_stack, arr1));
    assert(stack_push(p1->polish_stack, arr2));
    assert(p1->polish_stack->size == 2);
    assert(interp_b_less(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* result1 = stack_peek(p1->polish_stack);
    assert(result1->array[0][0] == 0);
    assert(result1->array[0][1] == 0);
    assert(result1->array[1][0] == 1);
    assert(result1->array[1][1] == 0);

    nlab_array_free(arr1);
    nlab_array_free(arr2);
    program_builder_free(p1);
}

void test_interp_b_add(void){
    // test #1 - a scalar operation
    Program* p1 = program_builder_init();
    nlab_array* arr1 = nlab_array_create_ones(6,5);
    nlab_array* arr2 = nlab_array_create_1d(2);
    assert(stack_push(p1->polish_stack, arr1));
    assert(stack_push(p1->polish_stack, arr2));
    assert(p1->polish_stack->size == 2);
    assert(interp_b_add(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* result = stack_peek(p1->polish_stack);
    for(int y = 0; y < 6; y++){
        for(int x = 0; x < 5; x++){
            assert(result->array[y][x] == 3);
        }
    }
    program_builder_free(p1);

    // test #2 - two vectors of equal size
    Program* p2 = program_builder_init();
    nlab_array* arr3 = nlab_array_create_ones(4,4);
    nlab_array* arr4 = nlab_array_create_ones(4,4);
    assert(stack_push(p2->polish_stack, arr3));
    assert(stack_push(p2->polish_stack, arr4));
    assert(p2->polish_stack->size == 2);
    assert(interp_b_add(p2));
    assert(p2->polish_stack->size == 1);
    nlab_array* result2 = stack_peek(p2->polish_stack);
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            assert(result2->array[y][x] == 2);
        }
    }
    program_builder_free(p2);

    // test #3 - two arrays not of same dimensions
    Program* p3 = program_builder_init();
    nlab_array* arr5 = nlab_array_create_ones(4,4);
    nlab_array* arr6 = nlab_array_create_ones(3,3);
    assert(stack_push(p3->polish_stack, arr5));
    assert(stack_push(p3->polish_stack, arr6));
    assert(!interp_b_add(p3));
    program_builder_free(p3);


    nlab_array_free(arr1);
    nlab_array_free(arr2);
    nlab_array_free(arr3);
    nlab_array_free(arr4);
    nlab_array_free(arr5);
    nlab_array_free(arr6);

}
void test_interp_b_times(void){
    
    Program* p1 = program_builder_init();

    // test #1 - ONES 7 B-TIMES
    nlab_array* arr1 = nlab_array_create_ones(1,4);
    nlab_array* arr2 = nlab_array_create_1d(7);
    assert(stack_push(p1->polish_stack, arr1));
    assert(stack_push(p1->polish_stack, arr2));
    assert(p1->polish_stack->size == 2);
    assert(interp_b_times(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* res1 = stack_peek(p1->polish_stack);
    for(int y = 0; y < 1; y++){
        for(int x = 0; x < 4; x++){
            assert(res1->array[y][x] == 7);
        }
    }

    // test #1 - 3 7 B-TIMES
    nlab_array* arr3 = nlab_array_create_1d(3);
    nlab_array* arr4 = nlab_array_create_1d(7);
    assert(stack_push(p1->polish_stack, arr3));
    assert(stack_push(p1->polish_stack, arr4));
    assert(p1->polish_stack->size == 3);
    assert(interp_b_times(p1));
    assert(p1->polish_stack->size == 2);
    nlab_array* res2 = stack_peek(p1->polish_stack);
    assert(res2->array[0][0] == 21);

    // test #1 - TWOS TWOS => FOURS
    nlab_array* arr5 = nlab_array_create_ones(2,2);
    arr5->array[0][0] = 2;
    arr5->array[0][1] = 2;
    arr5->array[1][0] = 2;
    arr5->array[1][1] = 2;
    nlab_array* arr6 = nlab_array_create_ones(2,2);
    arr6->array[0][0] = 2;
    arr6->array[0][1] = 2;
    arr6->array[1][0] = 2;
    arr6->array[1][1] = 2;
    assert(stack_push(p1->polish_stack, arr5));
    assert(stack_push(p1->polish_stack, arr6));
    assert(interp_b_times(p1));
    nlab_array* res3 = stack_peek(p1->polish_stack);
    assert(res3->array[0][0] == 4);
    assert(res3->array[0][1] == 4);
    assert(res3->array[1][0] == 4);
    assert(res3->array[1][1] == 4);

    nlab_array_free(arr1);
    nlab_array_free(arr2);
    nlab_array_free(arr3);
    nlab_array_free(arr4);
    nlab_array_free(arr5);
    nlab_array_free(arr6);
    program_builder_free(p1);
}

void test_interp_b_equals(void){
    Program* p1 = program_builder_init();

    nlab_array* v1 = nlab_array_create_ones(2,4);
    v1->array[0][0] = 2;
    v1->array[0][1] = 2;
    v1->array[1][0] = 4;
    v1->array[1][1] = 4;
    nlab_array* v2 = nlab_array_create_ones(1,4);
    v2->array[0][0] = 2;
    v2->array[0][1] = 2;
    v2->array[0][2] = 2;
    v2->array[0][3] = 2;
    nlab_array* s1 = nlab_array_create_1d(2);
    nlab_array* s2 = nlab_array_create_1d(5);

    // test #1 - arrays with different dims returns false
    assert(stack_push(p1->polish_stack, v1));
    assert(stack_push(p1->polish_stack, v2));
    assert(p1->polish_stack->size == 2);
    assert(!interp_b_equals(p1));
    assert(p1->polish_stack->size == 0);

    // test #2 - an array and a vector with equal
    assert(stack_push(p1->polish_stack, v1));
    assert(stack_push(p1->polish_stack, s1));
    assert(p1->polish_stack->size == 2);
    assert(interp_b_equals(p1));
    assert(p1->polish_stack->size == 1);
    nlab_array* res2 = stack_peek(p1->polish_stack);
    assert(res2->array[0][0] == 1);
    assert(res2->array[0][1] == 1);
    assert(res2->array[1][0] == 0);
    assert(res2->array[1][1] == 0);
    
    // test #3 - two scalars not equal
    assert(stack_push(p1->polish_stack, s1));
    assert(stack_push(p1->polish_stack, s2));
    assert(p1->polish_stack->size == 3);
    assert(interp_b_equals(p1));
    assert(p1->polish_stack->size == 2);
    nlab_array* res3 = stack_peek(p1->polish_stack);
    assert(res3->array[0][0] == 0);
    
    nlab_array_free(v1);
    nlab_array_free(v2);
    nlab_array_free(s1);
    nlab_array_free(s2);
    program_builder_free(p1);

}

void test_interp_loop(void){

    // moved these tests into parsing test function test_loop();
}

void test_binop_scalar_vector(void){

    // test #1 - ONES 5 B-ADD
    nlab_array* v1 = nlab_array_create_ones(3,4);
    nlab_array* s1 = nlab_array_create_1d(5);
    nlab_array* r1 = _binop_scalar_vector(s1, v1, binop_add);
    assert(r1);
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 4; x++){
            assert(r1->array[y][x] == 6);
        }
    }

    // test #2 - ONES -2 B-TIMES
    nlab_array* v2 = nlab_array_create_ones(2,2);
    nlab_array* s2 = nlab_array_create_1d(-2);
    nlab_array* r2 = _binop_scalar_vector(s2, v2, binop_times);
    assert(r2);
    for(int y = 0; y < 2; y++){
        for(int x = 0; x < 2; x++){
            assert(r2->array[y][x] == -2);
        }
    }

    // test #3 - test some NULL values
    nlab_array* r3 = _binop_scalar_vector(s1, NULL, binop_times);
    assert(r3 == NULL);
    r3 = _binop_scalar_vector(NULL, s2, binop_times);
    assert(r3 == NULL);

    // test #4 - invalud binary operator type
    r3 = _binop_scalar_vector(s1, v1, -1);
    assert(r3 == NULL);

    // test #5 - THIRTEENS 60 B-AND
    nlab_array* v5 = nlab_array_create_ones(2,2);
    v5->array[0][0] = 13;
    v5->array[0][1] = 13;
    v5->array[1][0] = 13;
    v5->array[1][1] = 13;
    nlab_array* s5 = nlab_array_create_1d(60);
    nlab_array* r5 = _binop_scalar_vector(s5, v5, binop_and);
    assert(r5);
    for(int y = 0; y < 2; y++){
        for(int x = 0; x < 2; x++){
            assert(r5->array[y][x] == 1);
        }
    }

    nlab_array_free(v1);
    nlab_array_free(v2);
    nlab_array_free(v5);
    nlab_array_free(s1);
    nlab_array_free(s2);
    nlab_array_free(s5);
    nlab_array_free(r1);
    nlab_array_free(r2);
    nlab_array_free(r5);
}

void test_binop_vector_vector(void){

    // test #1 - ONES 5 B-ADD
    nlab_array* v1 = nlab_array_create_ones(3,4);
    nlab_array* v2 = nlab_array_create_ones(3,4);
    nlab_array* r1 = _binop_vector_vector(v1, v2, binop_add);
    assert(r1);
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 4; x++){
            assert(r1->array[y][x] == 2);
        }
    }

    // test #2 - Arrays are not same dims
    nlab_array* v3 = nlab_array_create_ones(2,2);
    nlab_array* v4 = nlab_array_create_ones(3,3);
    nlab_array* r2 = _binop_vector_vector(v3, v4, binop_times);
    assert(r2 == NULL);
 
    // test #3 - test some NULL values
    nlab_array* r3 = _binop_vector_vector(v1, NULL, binop_times);
    assert(r3 == NULL);
    r3 = _binop_vector_vector(NULL, v2, binop_times);
    assert(r3 == NULL);

    // test #4 - invalud binary operator type
    r3 = _binop_vector_vector(v1, v2, -1);
    assert(r3 == NULL);

    // test #5 - B-AND a vector
    nlab_array* v5 = nlab_array_create_ones(2,2);
    v5->array[0][0] = 0;
    v5->array[0][1] = 2;
    v5->array[1][0] = 0;
    v5->array[1][1] = -4;
    nlab_array* v6 = nlab_array_create_ones(2,2);
    v6->array[0][0] = 0;
    v6->array[0][1] = 2;
    v6->array[1][0] = 9;
    v6->array[1][1] = 0;
    nlab_array* r4 = _binop_vector_vector(v5, v6, binop_and);
    assert(r4);
    assert(r4->array[0][0] == 0);
    assert(r4->array[0][1] == 1);
    assert(r4->array[1][0] == 0);
    assert(r4->array[1][1] == 0);

    nlab_array_free(v1);
    nlab_array_free(v2);
    nlab_array_free(v3);
    nlab_array_free(v4);
    nlab_array_free(v5);
    nlab_array_free(v6);
    nlab_array_free(r1);
    nlab_array_free(r4);
}

void test_binop_scalar_scalar(void){

    // test #1 - ONES 5 B-ADD
    nlab_array* s1 = nlab_array_create_1d(3);
    nlab_array* s2 = nlab_array_create_1d(5);
    nlab_array* r1 = _binop_scalar_scalar(s1, s2, binop_add);
    assert(r1);
    assert(r1->rows == 1);
    assert(r1->cols == 1);
    assert(r1->array[0][0] == 8);

    // test #2 - ONES -2 B-TIMES
    nlab_array* s3 = nlab_array_create_1d(3);
    nlab_array* s4 = nlab_array_create_1d(5);
    nlab_array* r2 = _binop_scalar_scalar(s1, s2, binop_times);
    assert(r2);
    assert(r2->rows == 1);
    assert(r2->cols == 1);
    assert(r2->array[0][0] == 15);

    // test #3 - test some NULL values
    nlab_array* r3 = _binop_scalar_scalar(s1, NULL, binop_times);
    assert(r3 == NULL);
    r3 = _binop_scalar_scalar(NULL, s2, binop_times);
    assert(r3 == NULL);

    // test #4 - invalud binary operator type
    r3 = _binop_scalar_scalar(s1, s2, -1);
    assert(r3 == NULL);

    // test #5 - 16 1 B-AND
    nlab_array* s5 = nlab_array_create_1d(16);
    nlab_array* s6 = nlab_array_create_1d(1);
    nlab_array* r4 = _binop_scalar_scalar(s5, s6, binop_and);
    assert(r4);
    assert(r4->array[0][0] == 1);

    nlab_array_free(s1);
    nlab_array_free(s2);
    nlab_array_free(s3);
    nlab_array_free(s4);
    nlab_array_free(s5);
    nlab_array_free(s6);
    nlab_array_free(r1);
    nlab_array_free(r2);
    nlab_array_free(r4);
}

#ifdef EXTENSION
void test_extension_u_trace(){
    
    // test #1 - an example from Wiki 
    nlab_array* matrix1 = nlab_array_create_ones(3,3);
    matrix1->array[0][0] = 5;
    matrix1->array[0][1] = 3;
    matrix1->array[0][2] = 5;
    matrix1->array[1][0] = 4;
    matrix1->array[1][1] = 1;
    matrix1->array[1][2] = 2;
    matrix1->array[2][0] = 3;
    matrix1->array[2][1] = 8;
    matrix1->array[2][2] = 7;
    Program* p1 = program_builder_init();
    stack_push(p1->polish_stack, matrix1);
    assert(extension_u_trace(p1));
    nlab_array* result = stack_peek(p1->polish_stack);
    assert(result);
    assert(result->rows = 1);
    assert(result->cols = 1);
    assert(result->array[0][0] == 13);
    nlab_array_free(matrix1);
    program_builder_free(p1);

    // test #2 - not NxN
    nlab_array* matrix2 = nlab_array_create_ones(2,3);
    matrix2->array[0][0] = 5;
    matrix2->array[0][1] = 3;
    matrix2->array[0][2] = 5;
    matrix2->array[1][0] = 4;
    matrix2->array[1][1] = 1;
    matrix2->array[1][2] = 2;
    Program* p2 = program_builder_init();
    stack_push(p2->polish_stack, matrix2);
    assert(!extension_u_trace(p2));
    nlab_array_free(matrix2);
    program_builder_free(p2);

    // test #3 - a NULL prog
    assert(!extension_u_trace(NULL));

}

void test_extension_u_transpose(void){

    // test #1 - an example from Wiki 
    nlab_array* matrix1 = nlab_array_create_ones(2,3);
    matrix1->array[0][0] = 1;
    matrix1->array[0][1] = 2;
    matrix1->array[0][2] = 3;
    matrix1->array[1][0] = 0;
    matrix1->array[1][1] = 2;
    matrix1->array[1][2] = 7;
    Program* p1 = program_builder_init();
    stack_push(p1->polish_stack, matrix1);
    assert(extension_u_transpose(p1));
    nlab_array* result = stack_peek(p1->polish_stack);
    assert(result);
    assert(result->rows = 3);
    assert(result->cols = 2);
    assert(result->array[0][0] == 1);
    assert(result->array[0][1] == 0);
    assert(result->array[1][0] == 2);
    assert(result->array[1][1] == 2);
    assert(result->array[2][0] == 3);
    assert(result->array[2][1] == 7);
    nlab_array_free(matrix1);
    program_builder_free(p1);

    // test #2 - a NULL prog
    assert(!extension_u_transpose(NULL));
}

void test_extension_u_submatrix(void){

    // test #1 - an example from Wiki 
    nlab_array* matrix1 = nlab_array_create_ones(3,4);
    matrix1->array[0][0] = 1;
    matrix1->array[0][1] = 2;
    matrix1->array[0][2] = 3;
    matrix1->array[0][3] = 4;
    matrix1->array[1][0] = 5;
    matrix1->array[1][1] = 6;
    matrix1->array[1][2] = 7;
    matrix1->array[1][3] = 8;
    matrix1->array[2][0] = 9;
    matrix1->array[2][1] = 10;
    matrix1->array[2][2] = 11;
    matrix1->array[2][3] = 12;
    nlab_array* rows = nlab_array_create_1d(3);
    nlab_array* cols = nlab_array_create_1d(2);
    Program* p1 = program_builder_init();
    stack_push(p1->polish_stack, rows);
    stack_push(p1->polish_stack, cols);
    stack_push(p1->polish_stack, matrix1);
    assert(extension_u_submatrix(p1));
    nlab_array* result = stack_peek(p1->polish_stack);
    assert(result);
    assert(result->rows = 2);
    assert(result->cols = 3);
    assert(result->array[0][0] == 1);
    assert(result->array[0][1] == 3);
    assert(result->array[0][2] == 4);
    assert(result->array[1][0] == 5);
    assert(result->array[1][1] == 7);
    assert(result->array[1][2] == 8);
    nlab_array_free(rows);
    nlab_array_free(cols);
    nlab_array_free(matrix1);
    program_builder_free(p1);

    // test #2 - a NULL prog
    assert(!extension_u_submatrix(NULL));

    // test #3 - range of rows is greater than matrix
    nlab_array* matrix2 = nlab_array_create_ones(2,2);
    matrix2->array[0][0] = 0;
    matrix2->array[0][1] = 1;
    matrix2->array[1][0] = 0;
    matrix2->array[1][1] = 1;
    nlab_array* rows2 = nlab_array_create_1d(3);
    nlab_array* cols2 = nlab_array_create_1d(2);
    Program* p2 = program_builder_init();
    stack_push(p2->polish_stack, rows2);
    stack_push(p2->polish_stack, cols2);
    stack_push(p2->polish_stack, matrix2);
    assert(!extension_u_submatrix(p2));
    nlab_array_free(rows2);
    nlab_array_free(cols2);
    nlab_array_free(matrix2);
    program_builder_free(p2);

    // test #4 - range of cols is greater than matrix
    nlab_array* matrix3 = nlab_array_create_ones(2,2);
    matrix3->array[0][0] = 0;
    matrix3->array[0][1] = 1;
    matrix3->array[1][0] = 0;
    matrix3->array[1][1] = 1;
    nlab_array* two = nlab_array_create_1d(2);
    nlab_array* three = nlab_array_create_1d(3);
    Program* p3 = program_builder_init();
    stack_push(p3->polish_stack, two);
    stack_push(p3->polish_stack, three);
    stack_push(p3->polish_stack, matrix3);
    assert(!extension_u_submatrix(p3));
    nlab_array_free(two);
    nlab_array_free(three);
    nlab_array_free(matrix3);
    program_builder_free(p3);

    // test #5 - whilst our code is zero-based, these array dim numbers
    // start at 1!
    nlab_array* matrix5 = nlab_array_create_ones(2,2);
    matrix5->array[0][0] = 0;
    matrix5->array[0][1] = 1;
    matrix5->array[1][0] = 0;
    matrix5->array[1][1] = 1;
    nlab_array* zero = nlab_array_create_1d(0);
    Program* p5 = program_builder_init();
    stack_push(p5->polish_stack, zero);
    stack_push(p5->polish_stack, zero);
    stack_push(p5->polish_stack, matrix5);
    assert(!extension_u_submatrix(p5));
    nlab_array_free(zero);
    nlab_array_free(matrix5);
    program_builder_free(p5);

}

void test_extension_b_dotproduct(void){

    // test #1 - valid dot-product
    nlab_array* matrix1 = nlab_array_create_ones(2,3);
    matrix1->array[0][0] = 1;
    matrix1->array[0][1] = 2;
    matrix1->array[0][2] = 3;
    matrix1->array[1][0] = 4;
    matrix1->array[1][1] = 5;
    matrix1->array[1][2] = 6;
    nlab_array* matrix2 = nlab_array_create_ones(3,2);
    matrix2->array[0][0] = 10;
    matrix2->array[0][1] = 11;
    matrix2->array[1][0] = 20;
    matrix2->array[1][1] = 21;
    matrix2->array[2][0] = 30;
    matrix2->array[2][1] = 31;
    Program* p1 = program_builder_init();
    stack_push(p1->polish_stack, matrix1);
    stack_push(p1->polish_stack, matrix2);
    assert(extension_b_dotproduct(p1));
    nlab_array* result1 = stack_peek(p1->polish_stack);
    assert(result1);
    assert(p1->polish_stack->size == 1);
    assert(result1->rows == 2);
    assert(result1->cols == 2);
    assert(result1->array[0][0] == 140);
    assert(result1->array[0][1] == 146);
    assert(result1->array[1][0] == 320);
    assert(result1->array[1][1] == 335);
    nlab_array_free(matrix1);
    nlab_array_free(matrix2);
    program_builder_free(p1);

    // test #2 - another valid dot-product that result
    // in a 1x1 array
    nlab_array* matrix3 = nlab_array_create_ones(1,5);
    matrix3->array[0][0] = 5;
    matrix3->array[0][1] = 4;
    matrix3->array[0][2] = 3;
    matrix3->array[0][3] = 2;
    matrix3->array[0][4] = 1;
    nlab_array* matrix4 = nlab_array_create_ones(5,1);
    matrix4->array[0][0] = 5;
    matrix4->array[1][0] = 4;
    matrix4->array[2][0] = 3;
    matrix4->array[3][0] = 2;
    matrix4->array[4][0] = 1;
    Program* p2 = program_builder_init();
    stack_push(p2->polish_stack, matrix3);
    stack_push(p2->polish_stack, matrix4);
    assert(extension_b_dotproduct(p2));
    nlab_array* result2 = stack_peek(p2->polish_stack);
    assert(result2);
    assert(p2->polish_stack->size == 1);
    assert(result2->rows == 1);
    assert(result2->cols == 1);
    assert(result2->array[0][0] == 55);
    nlab_array_free(matrix3);
    nlab_array_free(matrix4);
    program_builder_free(p2);

    // test #3 - an exam where the dims aren't symmetrical
    nlab_array* matrix5 = nlab_array_create_ones(2,4);
    matrix5->array[0][0] = 3;
    matrix5->array[0][1] = 2;
    matrix5->array[0][2] = 1;
    matrix5->array[0][3] = 5;
    matrix5->array[1][0] = 9;
    matrix5->array[1][1] = 1;
    matrix5->array[1][2] = 3;
    matrix5->array[1][3] = 0;
    nlab_array* matrix6 = nlab_array_create_ones(4,3);
    matrix6->array[0][0] = 2;
    matrix6->array[0][1] = 9;
    matrix6->array[0][2] = 0;
    matrix6->array[1][0] = 1;
    matrix6->array[1][1] = 3;
    matrix6->array[1][2] = 5;
    matrix6->array[2][0] = 2;
    matrix6->array[2][1] = 4;
    matrix6->array[2][2] = 7;
    matrix6->array[3][0] = 8;
    matrix6->array[3][1] = 1;
    matrix6->array[3][2] = 5;
    Program* p3 = program_builder_init();
    stack_push(p3->polish_stack, matrix5);
    stack_push(p3->polish_stack, matrix6);
    assert(extension_b_dotproduct(p3));
    nlab_array* result3 = stack_peek(p3->polish_stack);
    assert(result3);
    assert(p3->polish_stack->size == 1);
    assert(result3->rows == 2);
    assert(result3->cols == 3);
    assert(result3->array[0][0] == 50);
    assert(result3->array[0][1] == 42);
    assert(result3->array[0][2] == 42);
    assert(result3->array[1][0] == 25);
    assert(result3->array[1][1] == 96);
    assert(result3->array[1][2] == 26);
    nlab_array_free(matrix5);
    nlab_array_free(matrix6);
    program_builder_free(p3);

    // test #4 - an empty stack
    Program* p4 = program_builder_init();
    assert(!extension_b_dotproduct(p4));
    program_builder_free(p4);

    // test #5 - only one item on stack
    nlab_array* matrix7 = nlab_array_create_ones(1,2);
    matrix7->array[0][0] = 3;
    matrix7->array[0][1] = 2;
    Program* p5 = program_builder_init();
    stack_push(p5->polish_stack, matrix7);
    assert(!extension_b_dotproduct(p5));
    nlab_array_free(matrix7);
    program_builder_free(p5);
}

void test_extension_b_power(void){

    // test #1 - [1,2][3,4]^2 = [7,10][15,22]
    Program* p1 = program_builder_init();
    nlab_array* matrix1 = nlab_array_create_ones(2,2);
    matrix1->array[0][0] = 1;
    matrix1->array[0][1] = 2;
    matrix1->array[1][0] = 3;
    matrix1->array[1][1] = 4;
    nlab_array* power2 = nlab_array_create_1d(2);
    stack_push(p1->polish_stack, matrix1);
    stack_push(p1->polish_stack, power2);
    assert(extension_b_power(p1));
    nlab_array* result1 = stack_peek(p1->polish_stack);
    assert(result1);
    assert(result1->array[0][0] == 7);
    assert(result1->array[0][1] == 10);
    assert(result1->array[1][0] == 15);
    assert(result1->array[1][1] == 22);
    nlab_array_free(matrix1);
    nlab_array_free(power2);
    program_builder_free(p1);

    // test #2 - [1,2][3,4]^3 = [37,54,81,118]
    Program* p2 = program_builder_init();
    nlab_array* matrix2 = nlab_array_create_ones(2,2);
    matrix2->array[0][0] = 1;
    matrix2->array[0][1] = 2;
    matrix2->array[1][0] = 3;
    matrix2->array[1][1] = 4;
    nlab_array* power3 = nlab_array_create_1d(3);
    stack_push(p2->polish_stack, matrix2);
    stack_push(p2->polish_stack, power3);
    assert(extension_b_power(p2));
    nlab_array* result2 = stack_peek(p2->polish_stack);
    assert(result2);
    assert(result2->array[0][0] == 37);
    assert(result2->array[0][1] == 54);
    assert(result2->array[1][0] == 81);
    assert(result2->array[1][1] == 118);
    nlab_array_free(matrix2);
    nlab_array_free(power3);
    program_builder_free(p2);

    // test #3 - a non sqr matrix cannot be powered
    Program* p3 = program_builder_init();
    nlab_array* matrix3 = nlab_array_create_ones(2,3);
    matrix3->array[0][0] = 1;
    matrix3->array[0][1] = 2;
    matrix3->array[1][0] = 3;
    matrix3->array[1][1] = 4;
    nlab_array* power4 = nlab_array_create_1d(4);
    stack_push(p3->polish_stack, matrix3);
    stack_push(p3->polish_stack, power4);
    assert(!extension_b_power(p3));
    nlab_array_free(matrix3);
    nlab_array_free(power4);
    program_builder_free(p3);

    // test #4 - M^1 should return the same 
    Program* p4 = program_builder_init();
    nlab_array* matrix4 = nlab_array_create_ones(2,2);
    matrix4->array[0][0] = 1;
    matrix4->array[0][1] = 2;
    matrix4->array[1][0] = 3;
    matrix4->array[1][1] = 4;
    nlab_array* power1 = nlab_array_create_1d(1);
    stack_push(p4->polish_stack, matrix4);
    stack_push(p4->polish_stack, power1);
    assert(extension_b_power(p4));
    nlab_array* result4 = stack_peek(p4->polish_stack);
    assert(result4);
    assert(result4->array[0][0] == 1);
    assert(result4->array[0][1] == 2);
    assert(result4->array[1][0] == 3);
    assert(result4->array[1][1] == 4);
    nlab_array_free(matrix4);
    nlab_array_free(power1);
    program_builder_free(p4);

    // test #5 -  M and n in wrong order
    Program* p5 = program_builder_init();
    nlab_array* matrix5 = nlab_array_create_ones(2,2);
    matrix5->array[0][0] = 1;
    matrix5->array[0][1] = 2;
    matrix5->array[1][0] = 3;
    matrix5->array[1][1] = 4;
    nlab_array* n = nlab_array_create_1d(1);
    stack_push(p5->polish_stack, n);
    stack_push(p5->polish_stack, matrix5);
    assert(!extension_b_power(p5));
    nlab_array_free(matrix5);
    nlab_array_free(n);
    program_builder_free(p5);


}

#endif
