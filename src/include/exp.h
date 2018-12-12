#pragma once
#include "lexer.h"
#include "tree.h"

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
	char *name;
	float value;
	int type;
	struct _variable *next;
}variable;


int block(char ** buffer);
int assignment_expression(node_t **node);
int lookup(char *name, float  *val);
int assign_value(char *name, float val);
int primary_expression(node_t **node);
int multiplicative_expression(node_t **node);
int additive_expression(node_t **node);
int conditional_expression(node_t **node);

int exp_parser_init();
void prefix_tree(node_t *tree, int level);
float eval();
