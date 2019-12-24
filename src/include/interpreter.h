#pragma once
#include "lexer.h"
#include "exp.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif 
// __cplusplus

typedef enum compound_origin
{
	COMPOUND,
	ITERATION,
	SELECTION
}compound_origin;

typedef enum way_out
{
	NORMAL,
	BREAK,
	CONTINUE,
	RETURN
}way_out;

typedef enum builtin_types
{
    CHAR_TYPE,
    INT_TYPE,
    UCHAR_TYPE,
    UINT_TYPE,
    FLOAT_TYPE,

    VOID_TYPE,
    UNKNOWN_TYPE
}builtin_types;

typedef enum object_type_t
{
    PRIMITIVE,
    STRUCT,
    ENUM,
    ARRAY,

    UNKNOWN_OBJECT

}object_type_t;

typedef struct type_t
{
    string_ref name;
    object_type_t object_type;
    builtin_types btype;

    char* names;
    struct type_t* types;
}type_t;

typedef struct block_t function_body_t;
typedef struct interpreter_context interpreter_context;
typedef int (*CFunction)(interpreter_context*);

typedef enum {
  USER_FUNCTION,
  BUILTIN_FUNCTION,
  C_FUNCTION
} function_type_t;

typedef struct function_t
{
  function_type_t type;
  string_ref name;
  variable* args;
  type_t return_type;
  union
  {
    function_body_t* body;
    CFunction cfunc;
  };
}function_t;


int start(char ** buffer);
way_out do_if();
way_out do_while();
way_out statement(compound_origin origin);
way_out compound_statement(compound_origin origin);

#ifdef __cplusplus
}
#endif

bool register_cfunction(interpreter_context *ctx, CFunction* func, const char* name);
bool is_cfunction(interpreter_context* ctx, string_ref name);
void call_cfunction(interpreter_context* ctx, function_t* func);
function_t* find_cfunction(interpreter_context* ctx, string_ref name);
