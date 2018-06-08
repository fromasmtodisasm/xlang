#pragma once
#include "lexer.h"

typedef enum variable_type
{
	VAR_INT,
	VAR_STRING,
	VAR_BOOL,
	VAR_CONST,
	VAR_SONST_LITERAL
};

typedef struct _variable
{
	char *name;
	int value;
	int type;
	struct _variable *next;
}variable;


int block(char ** buffer);

int assignment_expression();

int lookup(char *name, int * val);

int assign_value(char *name, int val);

int primary_expression();

int multiplicative_expression();

int additive_expression();

