#pragma once
#include "lexer.h"

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


int start(char ** buffer);
way_out do_if();
way_out do_while();
way_out statement(compound_origin origin);
way_out compound_statement(compound_origin origin);

#ifdef __cplusplus
}
#endif

