#ifndef _PARSER_H_  /* Protect against multiple inclusion */
#define _PARSER_H_ 

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/************************** Other Includes *********************************/
/***************************************************************************/
#include "exp.h"

#include <stdbool.h>
/***************************************************************************/
/*********************** Typedefs/Structures *******************************/
/***************************************************************************/
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
  UCHAR_TYPE,
  BOOL_TYPE,
  INT_TYPE,
  UINT_TYPE,
  FLOAT_TYPE,
  DOUBLE_TYPE,

  VEC2_TYPE,
  VEC3_TYPE,
  VEC4_TYPE,

  DVEC2_TYPE,
  DVEC3_TYPE,
  DVEC4_TYPE,

  MAT2_TYPE,
  MAT3_TYPE,
  MAT4_TYPE,

  DMAT2_TYPE,
  DMAT3_TYPE,
  DMAT4_TYPE,


  VOID_TYPE,
  UNKNOWN_TYPE

}builtin_types;

typedef enum object_type_t
{
    PRIMITIVE,
    STRUCT,
    ENUM,
    ARRAY,

    GLOBAL,
    UNKNOWN_OBJECT

}object_type_t;

typedef struct type_t
{
    string_ref name;
    object_type_t object_type;
    builtin_types btype;
    bool is_tag;
    int size;
    int offset;

    int num_types;
    string_ref* names;
    struct type_t* parent_scope;
    struct type_t* childrens;
}type_t;

typedef enum symbol_type
{
  esymbol_variable,
  esymbol_fuction
}symbol_type;

typedef struct symbol_function
{
  type_t* return_type;
}symbol_function;

typedef union symbol
{
  symbol_type type;


}symbol;

#if 0
typedef struct symbol_table
{
  int num_
  variable* variables;
};
#endif


typedef struct block_t function_body_t;
typedef struct xlang_context xlang_context;
typedef int (*CFunction)(xlang_context*);

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
/***************************************************************************/
/*************************** Prototypes ************************************/
/***************************************************************************/
node_t *parse(xlang_context *ctx, char **buffer);

way_out do_if(xlang_context* ctx);
way_out do_while(xlang_context* ctx);
way_out statement(xlang_context* ctx, node_t **root);
way_out compound_statement(xlang_context* ctx, node_t **root);

int block(char **buffer);
int assignment_expression(xlang_context* ctx, node_t **node);
int lookup(node_t* node);
float assign_value(node_t* node);
int primary_expression(xlang_context* ctx, node_t **node);
int multiplicative_expression(xlang_context* ctx, node_t **node);
int additive_expression(xlang_context* ctx, node_t **node);
int conditional_expression(xlang_context* ctx, node_t **node);

void prefix_tree(xlang_context* ctx, node_t *tree, int level);
void calculate(xlang_context* ctx, node_t *root);
node_t *eval(xlang_context* ctx);

xlang_context* xlang_create();
void xlang_set_buffer(xlang_context* ctx, char* buffer);
bool xlang_parse(xlang_context* ctx);

bool register_cfunction(xlang_context* ctx, CFunction* func, const char* name);
bool is_cfunction(xlang_context* ctx, string_ref name);
void call_cfunction(xlang_context* ctx, function_t* func);
function_t* find_cfunction(xlang_context* ctx, string_ref name);



/***************************************************************************/
/********************* Externally Defined Globals **************************/
/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _INTERPRETER_H_ */
