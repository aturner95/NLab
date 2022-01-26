#pragma once

#include "map/map.h"
#include "map/specific.h"
#include "stack/stack.h"
#include "stack/specific.h"

#define MAX_NUM_OF_TOKENS 1000
#define MAX_TOKEN_SIZE 100
#define MAX_LEN_OF_ERROR_MESSAGE 100
#define NUM_OF_PROGRAMS 1
#define SEMICOLON ";"
#define LBRACE "{"
#define RBRACE "}"

#define CURRENT_WORD prog->tokens[prog->current_token]
#define INCR_CURRENT_WORD prog->current_token++
#define DECR_CURRENT_WORD prog->current_token--
#define LOOK_AT_PREV_WORD prog->tokens[prog->current_token - 1]
#define CHECK_PROG_FOR_NULL(prog) if(prog == NULL || prog->tokens == NULL){return false;}
#define SET_ERROR_STATE(A) if(prog->error_state == error_none){prog->error_state = A;}

typedef enum error_state {error_none, error_io, error_parse, error_interp, error_unknown} error_state;
typedef enum binary_op {binop_and, binop_or, binop_greater, binop_less, binop_add, binop_times, binop_equals, binop_dotproduct, binop_power} binary_op;

typedef struct prog{
    char** tokens;
    int current_token;
    int num_of_tokens;
    char error_msg[MAX_LEN_OF_ERROR_MESSAGE];
    struct stack* polish_stack;
    struct map* variable_map;
    error_state error_state;
} Program;


/** GENERAL FUNCTIONS **/

void readfile(char* filename, Program* prog);
Program* program_builder_init(void);
bool program_builder_add(Program* prog, char* token);
void program_builder_free(Program* prog);
bool set_error_msg(Program* prog, const char* msg);
void process_error_msg(Program* prog, char* message);
int word_to_integer(char* word);
bool _is_correct_file_extention(char* filename, char* exttype);
bool _format_filename(char* fname);

/** GRAMMAR FUNCTIONS **/
bool program(Program* prog);
bool instrc_list(Program* prog);
bool instrc(Program* prog);
bool print(Program* prog);
bool varname(Program* prog);
bool string(Program* prog);
bool set(Program* prog);
bool polish_list(Program* prog);
bool polish(Program* prog);
bool pushdown(Program* prog);
bool integer(Program* prog);
bool unaryop(Program* prog);
bool binaryop(Program* prog);
bool create(Program* prog);
bool rows(Program* prog);
bool cols(Program* prog);
bool filename(Program* prog);
bool loop(Program* prog);



/** GRAMMAR TESTS **/

void test_program(void);
void test_instrc_list(void);
void test_instrc(void);
void test_print(void);
void test_varname(void);
void test_string(void);
void test_set(void);
void test_polish_list(void);
void test_polish(void);
void test_pushdown(void);
void test_integer(void);
void test_unaryop(void);
void test_binaryop(void);
void test_create(void);
void test_rows(void);
void test_cols(void);
void test_filename(void);
void test_loop(void);


/** TEST GENERAL FUNCTIONS **/
void test(void);
void test_program_builder_init(void);
void test_program_builder_add(void);
void test_set_error_msg(void);
void test_word_to_integer(void);
void test_is_correct_file_extention(void);
void test_format_filename(void);

/* TEST FUNCTIONS FOR DATA STRUCTURES*/
void test_nlab(void);
void test_stack(void);
void test_map(void);
void test_nlab_array(void);

/* INTERPRETER FUNCTIONS */
char* interp_print_variable(Program* prog, char* current_token);
char* interp_print_string(char* word);
bool interp_set(Program* prog);
bool interp_pushdown_variable(Program* prog);
bool interp_u_not(Program* prog);
bool interp_u_eightcount(Program* prog);
bool interp_b_and(Program* prog);
bool interp_b_or(Program* prog);
bool interp_b_greater(Program* prog);
bool interp_b_less(Program* prog);
bool interp_b_add(Program* prog);
bool interp_b_times(Program* prog);
bool interp_b_equals(Program* prog);
bool interp_loop(Program* prog);
bool interp_create_ones(Program* prog, char* key,  nlab_array* ones_array);
bool interp_create_read(Program* prog, char* key, nlab_array* arr, char* filename);

bool _add_value_to_map(Program* prog, char* key, nlab_array* value);
int _calc_moore_neighbourhood(nlab_array* nlab, int x, int y);
bool _do_binary_operation(Program* prog, binary_op operation_type);
nlab_array* _binop_scalar_vector(nlab_array* scalar, nlab_array* vector, binary_op operation_type);
nlab_array* _binop_vector_vector(nlab_array* v1, nlab_array* v2, binary_op operation_type);
nlab_array* _binop_scalar_scalar(nlab_array* s1, nlab_array* s2, binary_op operation_type);
bool _evaluate_operation(Program* prog);
char* _interp_get_var_context(Program* prog);

/* EXTENSION FUNCTIONS */
#ifdef EXTENSION
bool extension_u_trace(Program* prog);
bool extension_u_transpose(Program* prog);
bool extension_u_submatrix(Program* prog);
bool extension_b_dotproduct(Program* prog);
bool extension_b_power(Program* prog);
#endif

/* TEST INTERPRETER FUNCTIONS */
void test_interp_print_variable(void);
void test_interp_print_string(void);
void test_interp_set(void);
void test_interp_get_var_context(void);
void test_interp_u_not(void);
void test_interp_u_eightcount(void);
void test_interp_b_and(void);
void test_interp_b_or(void);
void test_interp_b_greater(void);
void test_interp_b_less(void);
void test_interp_b_add(void);
void test_interp_b_times(void);
void test_interp_b_equals(void);
void test_interp_create_read(void);
void test_interp_loop(void);
void test_binop_scalar_vector(void);
void test_binop_vector_vector(void);
void test_binop_scalar_scalar(void);

/* TEST EXTENSION FUNCTIONS */
#ifdef EXTENSION
void test_extension_u_trace(void);
void test_extension_u_transpose(void);
void test_extension_u_submatrix(void);
void test_extension_b_dotproduct(void);
void test_extension_b_power(void);
#endif
