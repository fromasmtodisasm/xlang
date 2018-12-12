#include "exp.h"
#include "lexer.h"
#include "tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define begin_func() \
  fprintf(stderr, "Function is %s line is %d\n", __FUNCTION__, __LINE__)
#define end_func() \
  printf( "On line [%d]\n", __LINE__)//fprintf(stderr, "Function %s is end on line %d\n", __FUNCTION__, __LINE__)

variable *vars;
//static token_t *curr_token;// = curr_token;

int make_builtin_vars()
{
	vars = malloc(sizeof(variable));
	static variable false =
	{
		"false",
		0
	};
	static variable true = {
		"true",
		1,
	};

	vars = &true;
	true.next = &false;
}

int exp_parser_init()
{
	make_builtin_vars();
}

int block(char **buffer)
{
	return 0;
}

int lookup(char *name, int *val)
{
	variable *cur_var;
	int res = 0;
	for (cur_var = vars; cur_var != NULL; cur_var = cur_var->next)
	{
		if (cur_var->name)
			if (!strcmp(name, cur_var->name))
			{
				*val = cur_var->value;
				res = 1;
				break;
			}
	}
	return res;
}

int assign_value(char *name, int val)
{
	variable *cur_var;
	variable *tmp;
	for (cur_var = vars; cur_var != NULL; cur_var = cur_var->next)
	{
		tmp = cur_var;
		if (cur_var->name == NULL)
		{
			cur_var->name = name;
			cur_var->value = val;
			break;
		}
		else if (!strcmp(name, cur_var->name))
		{
			cur_var->name = name;
			cur_var->value = val;
			break;
		}
	}
	if (!cur_var)
	{
		tmp->next = malloc(sizeof(variable));
		tmp = tmp->next;
		tmp->name = name;
		tmp->value = val;
		tmp->next = NULL;

	}
}

int primary_expression(node_t **root)
{
	int res = 0;

	switch (curr_token->type)
	{

	case lcNUMBER:
  case lcIDENT:
	{
    *root = create_node();
    assert(curr_token->text != NULL);
    assert(*root != NULL);
    (*root)->text = strdup(curr_token->text);
    (*root)->type = curr_token->type;
		break;
	}
	case lcLBRACE:
	{
		get_token(/*NEXT_TOKEN*/);
		res = assignment_expression(root);
		if (curr_token->type != lcRBRACE)
		{
			printf("Error, expected ')'\n");
			getchar();
			//exit(-1);
		}
		break;
	}
	default:
		printf("Error, expected primary on line %d!!!\n", get_line());
		getchar();
		exit(-1);
	}
	get_token();
	return res;
}

int multiplicative_expression(node_t **root)
{
	int res = 0;
	int stop = 0;
  node_t *node;
  
  // Build left subtree
	res = primary_expression(root);
	while ( curr_token->type == lcMUL || curr_token->type == lcDIV)
	{
    node = create_node();
    assert(curr_token->text != NULL);
    assert(node != NULL);
    node->left = (*root);
    node->text = strdup(curr_token->text);
    node->type = curr_token->type;
    get_token();
		primary_expression(&(node->right));
    *root = node;
  }
  assert(*root != NULL);
	return res;
}

int additive_expression(node_t **root)
{
	int res = 0;
  node_t *node;

	res = multiplicative_expression(root);
  assert(*root != NULL);
	while (curr_token->type == lcPLUS || curr_token->type == lcMINUS)
	{
    node = create_node();
    assert(curr_token->text != NULL);
    assert(node != NULL);
    node->left = *root;
    node->text = strdup(curr_token->text);
    node->type = curr_token->type;
    get_token();
		multiplicative_expression(&(node->right));
    *root = node;
	}
  assert(*root != NULL);
  
	return res;
}

int is_relop(token_type type)
{
	return 
		(
			lcAND_OP == type ||
			lcOR_OP == type ||
			lcLE_OP == type ||
			lcGE_OP == type ||
			lcG_OP == type ||
			lcEQ_OP == type ||
			lcL_OP == type ||
			lcNE_OP == type
		);
}

int conditional_expression(node_t **root)
{
	int res = 0;
  node_t *node;
	token_type type = curr_token->type;
	res = additive_expression(root);
	
	while (is_relop(type=curr_token->type))
	{
		get_token(/*NEXT_TOKEN*/);

    node = create_node();
    assert(curr_token->text != NULL);
    assert(node != NULL);
    node->left = *root;
    
    node->text = strdup(curr_token->text);

    node->type = curr_token->type;
		additive_expression(&(node->right));
    *root = node;

		type = curr_token->type;
	}
  printf("result = %d\n", res);
  assert(*root != NULL);
	return res;
}

int assignment_expression(node_t **root)
{
	char *name;
	int tmp = 0;
	int res = 0;
	token_t prev_token;

  node_t *node;

  puts("begin assign");
	name = curr_token->text;
	if (curr_token->type == lcSEMI)
  {
		return res;
  }
	if (curr_token->type == lcIDENT)
	{
		char *name;
		int tmp = 0;
		name = curr_token->text;
		char *prev_pos = get_pos();
		memcpy(&prev_token, curr_token, sizeof(token_t));
    printf("This is ident!\n");
		
    *root = create_node();
    (*root)->text = strdup(curr_token->text);
    (*root)->type = curr_token->type;
    token_type type = get_token()->type;
    //root->left = node;
		if (type == lcASSIGN || type == lcPLUS_ASSIGN ||
			type == lcMINUS_ASSIGN || type == lcMUL_ASSIGN ||
			type == lcDIV_ASSIGN)
		{
			//lookup(name, &tmp);
			
      node = create_node();
      assert(curr_token->text != NULL);
      assert(node != NULL);
      node->left = *root;
      node->text = strdup(curr_token->text);
      node->type = curr_token->type;

			get_token(/*NEXT_TOKEN*/);

      assignment_expression(&(node->right));
      *root = node;
      printf("left root = %s;op = %s; right root = %s\n", (*root)->left->text, (*root)->text, (*root)->right->text);

			//assign_value(name, res);
			return res;
		}
		else
		{
			memcpy(curr_token, &prev_token, sizeof(token_t));
			set_pos(prev_pos);
		}
	}
	res = conditional_expression(root);
  //prefix_tree(root
	return res;
}

void make_space(int n)
{
  for ( ; n > 0; n--)
  {
    printf(" ");
  }
}
void print_node(node_t *node, int level)
{
  assert(node != NULL);
  //make_space(level);
  printf("%s ", node->text); 
}

void prefix_tree(node_t *tree, int level)
{
  if (tree){
    print_node(tree, level + 1);  
    prefix_tree(tree->left, level + 1);  
    prefix_tree(tree->right, level + 1);  
  }
}
