#pragma once
//#include "interpreter.h"
#include "syntax_parser.h"

#define FATAL_ERROR(str) (fprintf(stderr, "Fatal error on line %d: %s\n", __LINE__, str), exit(-1))

#define TYPES_CAPACITY 256
#define STACK_SIZE 64 
#define FUNCTIONS_COUNT 128 


typedef struct xlang_context
{
  const char* source;
  type_t* global_types;
  int num_types;
  int types_capacity;
	bool need_get_token;

  int num_funcs;
  function_t functions[FUNCTIONS_COUNT];
  function_t* current_function;
  function_t* entry_point;

	lexer_context_t *lexer;

  variable* symbol_table;
  int num_symbols;

  int stack[STACK_SIZE];
  int sp;
}xlang_context;

type_t* create_type(type_t* parent);
