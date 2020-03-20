#pragma once
#include "lexer.h"

typedef struct type_t type_t;

typedef enum variable_type
{
	VAR_INT,
	VAR_STRING,
	VAR_BOOL,
	VAR_CONST,
	VAR_SONST_LITERAL
}variable_type;

typedef struct _variable
{
  string_ref name;
  union value
  {
    char* sval;
    char cval;
    unsigned char ucval;
    int ival;
    unsigned int uival;
    float fval;

    void* object;
  };
  type_t *type;
  struct _variable *next;
}variable;


int block(char ** buffer);

int assignment_expression();

variable* lookup(string_ref name);

int assign_value(string_ref name, int val);

int primary_expression();

int multiplicative_expression();

int additive_expression();

int exp_parser_init();
