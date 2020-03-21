/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include "exp.h"
#include "lexer.h"
#include "interpreter.h"
#include "interpreter_private.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/***************************************************************************/
/***************** Defines used by this module only ************************/
/***************************************************************************/
#define begin_func()                                                           \
  DEBUG_TRACE(stderr, "Function is %s line is %d\n", __FUNCTION__, __LINE__)
#define end_func()                                                             \
  DEBUG_TRACE("On line [%d]\n", __LINE__) // fprintf(stderr, "Function %s is end on
                                     // line %d\n", __FUNCTION__, __LINE__)
#define ERROR(...) DEBUG_PROD(__VA_ARGS__)
#ifdef BTREE_USE

#include "tree.h"

int cmp_var_name(xlang_context* ctx, const void *left, const void *right);
typedef struct btree_node_t listof;

listof *vars;
#define push_var(var) btree_insert(&vars, &var, sizeof(var), cmp_var_name)
#define find_var(var) btree_search(vars, &var, cmp_var_name)
#define VAR_STORAGE "BTREE_USE"

#elif LIST_USE

#include "generic_list.h"

void *cmp_var_name(const void *vars, const void *data);
typedef struct list_t listof;

static listof *vars;
#define push_var(var) push(&vars, &var, sizeof(var))
#define find_var(var) (variable*)foreach_element(vars, &var.name, cmp_var_name)
#define VAR_STORAGE "LIST_USE"

#endif

/***************************************************************************/
/********** Global Variables defined for this module only ******************/
/************************ (should be static) *******************************/
/***************************************************************************/
const char *var_storage = VAR_STORAGE;

/***************************************************************************/
/*********************** Function Prototypes *******************************/
/******  (should be static, if not they should be in '.h' file) ************/
/***************************************************************************/
DEFINE_LEXER_CONTEXT();
//variable *vars;
extern xlang_context* global_context;

typedef struct primary_value_t
{
  enum primary_type
  {
    LITERAL,
    VARIABLE
  }type;
  union
  {
    variable* var;
    union 
    {
      char* sval;
      char cval;
      unsigned char ucval;
      int ival;
      unsigned int uival;
      float fval;
    }/*literal_value*/;
  };

}primary_value_t;

int make_builtin_vars()
{
	//vars = malloc(sizeof(variable));
  vars = global_context->symbol_table;
#if 0
  variable False =
  {
    .name = string_ref_create("false"),
    .ival = 0
	};
	variable True = {
		.name = string_ref_create("true"),
		.ival = 1,
	};

	memcpy(vars, &True, sizeof(variable));
  vars->next = malloc(sizeof(variable));
	memcpy(vars->next, &False, sizeof(variable));
#endif
}

int exp_parser_init()
{
	make_builtin_vars();
}

int block(char **buffer)
{
	return 0;
}

#ifdef LIST_USE

void* cmp_var_name(const void* vars, const void* data) {
	string_ref* name = (string_ref*)data;
	variable* cur_var = (variable*)vars;
	if (!strncmp(name->pos, cur_var->name.pos, cur_var->name.len)) {
		DEBUG_ALL("founded var with name  = %s\n", name);
		return cur_var;
	}
	return 0;
}

#endif //LIST_USE

void print_var(void* var) {
	printf("var name = %s\n", ((variable*)(var))->name);
}

int lookup(node_t* node) {
	variable* cur_var;
	int res = 0;
	variable* var = NULL;
	variable tmp;
	tmp.name = node->text;
	if (var = find_var(tmp))
	{
		node->value.f = var->fval;
		res = 1;
	}
	return res;
}

float assign_value(node_t* node) {
	variable* cur_var;
	variable tmp;
	assert(node != NULL);
	tmp.name = node->text;
	if (cur_var = find_var(tmp)) {
		//cur_var->name = name;
		cur_var->fval = node->value.f;
	}
	else {

		//tmp.name = name;
		tmp.fval = node->value.f;
		push_var(tmp);
	}
	return node->value.f;
}

int primary_expression(xlang_context* ctx, node_t **root) {
  int res = 0;
  //DEBUG_TRACE("\n");
  switch (CURRENT_TOKEN(ctx->lexer).type) {

  case lcNUMBER:
  case lcSTRING:
  case lcIDENT: {
    assert(CURRENT_TOKEN(ctx->lexer).text.len != 0);
    *root = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
    assert(*root != NULL);
    break;
  }
  case lcLBRACE: {
    lexer_get_token(ctx);
    res = assignment_expression(ctx, root);
    if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACE) {
      printf("Error, expected ')'\n");
      getchar();
      // exit(-1);
    }
    break;
  }
  default:
    ERROR("Error, expected primary on line %d!!! ", lexer_get_line(ctx));
    ERROR("Meeted %s\n", CURRENT_TOKEN(ctx->lexer).text);
    getchar();
    exit(-1);
  }
  lexer_get_token(ctx);
  return res;
}

int postfix_expression(xlang_context* ctx, node_t **root) {
  int res = 0;
  node_t *node;
  int need_get = true;
  //DEBUG_TRACE("\n");
  
  res = primary_expression(ctx, root);

  switch (CURRENT_TOKEN(ctx->lexer).type)
  {
  case lcPLUS_PLUS: {
  case lcMINUS_MINUS:
    DEBUG_ALL("This id postfix expression: %s\n", CURRENT_TOKEN(ctx->lexer).text);
    node = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
    node->left = (*root);
    *root = node;
  } break;
  case '[': {
    lexer_get_token(ctx);
    res = primary_expression(ctx, root);
    if (CURRENT_TOKEN(ctx->lexer).type != ']')
      ERROR("Expected ']'\n");
  } break;
  case lcLBRACE: {
    if (CURRENT_TOKEN(ctx).type != lcRBRACE)
    {
      //list_t *args;  
      node = create_node(lcCALL, CURRENT_TOKEN(ctx->lexer).text);
      node->left = (*root);
      *root = node;
      
      res = assignment_expression(ctx, root);
      while(CURRENT_TOKEN(ctx->lexer).type == lcCOMMA) {
        lexer_get_token(ctx);
        res = assignment_expression(ctx, root);
      }
      if (CURRENT_TOKEN(ctx->lexer).type != lcRBRACE)
        ERROR("Expected RBRACE\n");
    }
    else
    {
      printf("Empty arg list\n");
      node = create_node(lcCALL, CURRENT_TOKEN(ctx->lexer).text);
      node->left = (*root);
      *root = node;
    }
  } break;
  case lcPOINT: {
  case lcARROW:
    if (CURRENT_TOKEN(ctx).type != lcIDENT) {
      ERROR("Expected identifier on line %d\n", lexer_get_line(ctx));
    } 
    else {
      lexer_get_token(ctx);
      need_get = false;
      while (CURRENT_TOKEN(ctx->lexer).type == lcPOINT, CURRENT_TOKEN(ctx->lexer).type == lcARROW) {
        if (CURRENT_TOKEN(ctx).type == lcIDENT)
        {
          lexer_get_token(ctx);
        }
        else break;
      }
    }
  } break;

  default: {
    need_get = false; 
  } break;
  }

  if (need_get) lexer_get_token(ctx);
  return res;
}

int unary_expression(xlang_context* ctx, node_t **root) {
  int res = 0;
  node_t *node;
  //DEBUG_TRACE("\n");

  res = postfix_expression(ctx, root);

  return res;
}

int multiplicative_expression(xlang_context* ctx, node_t **root) {
  int res = 0;
  int stop = 0;
  node_t *node;

  //DEBUG_TRACE("\n");
  // Build left subtree
  res = unary_expression(ctx, root);
  while (CURRENT_TOKEN(ctx->lexer).type == lcMUL || CURRENT_TOKEN(ctx->lexer).type == lcDIV) {
    assert(CURRENT_TOKEN(ctx->lexer).text.len != 0);
    node = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
    assert(node != NULL);
    node->left = (*root);
    lexer_get_token(ctx);
    unary_expression(ctx, &(node->right));
    *root = node;
  }
  assert(*root != NULL);
  return res;
}

int additive_expression(xlang_context* ctx, node_t **root) {
  int res = 0;
  node_t *node;

  //DEBUG_TRACE("\n");
  res = multiplicative_expression(ctx, root);
  assert(*root != NULL);
  while (CURRENT_TOKEN(ctx->lexer).type == lcPLUS || CURRENT_TOKEN(ctx->lexer).type == lcMINUS) {
    assert(CURRENT_TOKEN(ctx->lexer).text.len != 0);
    node = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
    assert(node != NULL);
    node->left = *root;
    lexer_get_token(ctx);
    multiplicative_expression(ctx, &(node->right));
    *root = node;
  }
  assert(*root != NULL);

  return res;
}

int is_relop(token_type type) {
  return (lcAND_OP == type || lcOR_OP == type || lcLE_OP == type ||
          lcGE_OP == type || lcG_OP == type || lcEQ_OP == type ||
          lcL_OP == type || lcNE_OP == type);
}

int conditional_expression(xlang_context* ctx, node_t **root) {
  int res = 0;
  node_t *node;
  token_type type = CURRENT_TOKEN(ctx->lexer).type;

  //DEBUG_TRACE("\n");
  res = additive_expression(ctx, root);

  while (is_relop(type = CURRENT_TOKEN(ctx->lexer).type)) {
    assert(CURRENT_TOKEN(ctx->lexer).text.len != 0);
    node = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
    assert(node != NULL);
    node->left = *root;
    lexer_get_token(ctx);
    additive_expression(ctx, &(node->right));
    *root = node;

    type = CURRENT_TOKEN(ctx->lexer).type;
  }
  assert(*root != NULL);
  return res;
}

int assignment_expression(xlang_context* ctx, node_t **root) {
  string_ref name;
  int tmp = 0;
  int res = 0;
  token_t prev_token;
  node_t *node;

  name = CURRENT_TOKEN(ctx->lexer).text;
  if (CURRENT_TOKEN(ctx->lexer).type == lcSEMI) {
    //lexer_get_token();
    return res;
  }
  if (CURRENT_TOKEN(ctx->lexer).type == lcIDENT) {
    string_ref name;
    int tmp = 0;
		token_type type = CURRENT_TOKEN(ctx->lexer).type;
    char *prev_pos = lexer_get_pos(ctx);
		name = CURRENT_TOKEN(ctx->lexer).text;
    res = conditional_expression(ctx, root);
    DEBUG_PROD("root = %s\n", (*root)->text);
    DEBUG_PROD("CURRENT TOKEN = %s\n", CURRENT_TOKEN(ctx->lexer).text);
    if (type == lcASSIGN || type == lcPLUS_ASSIGN || type == lcMINUS_ASSIGN ||
        type == lcMUL_ASSIGN || type == lcDIV_ASSIGN) {
			node_t *exp;
      assert(CURRENT_TOKEN(ctx->lexer).text.len != 0);
      node = create_node(CURRENT_TOKEN(ctx->lexer).type, CURRENT_TOKEN(ctx->lexer).text);
      assert(node != NULL);
      node->left = *root;
      lexer_get_token(ctx);
      assignment_expression(ctx, &(node->right));
      exp = create_node(lcEXP, string_ref_create("expression"));
      node->left->type = node->right->type;
      exp->right = node;
      *root = exp;
      return res;
    } else {
      //memcpy(curr_token, &prev_token, sizeof(token_t));
      //set_pos(prev_pos);
    }
  }
  else { res = conditional_expression(ctx, root); }
  return res;
}

void make_space(xlang_context* ctx, int n) {
  for (; n > 0; n--) {
    printf(" ");
  }
}

void print_node(xlang_context* ctx, node_t *node, int level) {
  assert(node != NULL);
  // make_space(level);
  switch (node->type) {
  case lcPLUS:
    printf("add");
    break;
  case lcMINUS:
    printf("sub");
    break;
  case lcASSIGN:
    printf("move");
    break;
  }
}

#if 0
void functional(xlang_context* ctx, node_t *tree, int level) {
  if (tree) {
    switch (tree->type) {
    case lcPLUS:
    case lcMINUS:
    case lcASSIGN: {
      print_node(tree, level + 1);
      putchar('(');
      functional(tree->left, level + 1);
      printf(", ");
      functional(tree->right, level + 1);
      putchar(')');
      break;
    }
    default:
      printf("%s", tree->text);
    }
  }
}
#endif
void calculate(xlang_context* ctx, node_t *tree) {
  
  float *val = &tree->value.f, val1 = 0, val2 = 0;
  if (tree) {
    calculate(ctx, tree->left);
    calculate(ctx, tree->right);
    switch (tree->type) {
    case lcPLUS: {
      *val = tree->value.f = tree->left->value.f + tree->right->value.f;
    } break;
    case lcMINUS: {
      *val = tree->value.f = tree->left->value.f - tree->right->value.f;
    } break;
    case lcMUL: {
      *val = tree->value.f = tree->left->value.f * tree->right->value.f;
    } break;
    case lcDIV: {
      *val = tree->value.f = tree->left->value.f / tree->right->value.f;
    } break;
    case lcINC_OP: {
      *val = tree->value.f++;
    } break;
    case lcDEC_OP: {
      *val--;
    } break;

    case lcAND_OP: {
      *val = tree->value.f = (tree->left->value.f &&  tree->right->value.f);
      //*val = (val1 && val2);
    } break;
    case lcOR_OP: {
      *val = tree->value.f = (tree->left->value.f ||  tree->right->value.f);
      //*val = (val1 || val2);
    } break;
    case lcLE_OP: {
      *val = tree->value.f = (tree->left->value.f <=  tree->right->value.f);
    } break;
    case lcGE_OP: {
      *val = tree->value.f = (tree->left->value.f >=  tree->right->value.f);
    } break;
    case lcEQ_OP: {
      *val = tree->value.f = (tree->left->value.f ==  tree->right->value.f);
      //*val = (val1 == val2);
    } break;
    case lcNE_OP: {
      *val = tree->value.f = (tree->left->value.f !=  tree->right->value.f);
      //*val = (val1 != val2);
    } break;

    case lcG_OP: {
      *val = tree->value.f = (tree->left->value.f >  tree->right->value.f);
      //*val = (val1 > val2);
    } break;
    case lcL_OP: {
      //printf("right value = %f\n",tree->right->value.f); 
      *val = tree->value.f = (tree->left->value.f <  tree->right->value.f);
      //*val = (val1 < val2);
    } break;

    case lcNUMBER: {
      tree->type = lcNUMBER;
      *val = atof(tree->text.pos);
    } break;
    case lcIDENT: {
      if (!lookup(ctx, tree->text)) {
        printf("Undefined var: %s\n", tree->text);
      }
    } break;

    case lcASSIGN: {
      calculate(ctx, tree->right);
      tree->left->value.f = tree->right->value.f;
      *val = assign_value(tree->left);
      //printf("assign res = %f\n", *val);
    } break;
    case lcPLUS_ASSIGN: {
      calculate(ctx, tree->right);
      tree->left->value.f += tree->right->value.f; 
      *val = assign_value(tree->left);
      // printf("plus_asign = %f\n", *val);
    } break;
    case lcMINUS_ASSIGN: {
      calculate(ctx, tree->right);
      tree->left->value.f -= tree->right->value.f; 
      *val = assign_value(tree->left);
    } break;
    case lcMUL_ASSIGN: {
      calculate(ctx, tree->right);
      tree->left->value.f *= tree->right->value.f; 
      *val = assign_value(tree->left);
    } break;
    case lcDIV_ASSIGN: {
      calculate(ctx, tree->right);
      tree->left->value.f /= tree->right->value.f; 
      *val = assign_value(tree->left);
    } break;
    case lcPLUS_PLUS: {
      calculate(ctx, tree->left);
      *val = tree->left->value.f++;
      assign_value(tree->left);
    } break;
    case lcMINUS_MINUS: {
      calculate(ctx, tree->left);
      *val = tree->left->value.f--;
      assign_value(tree->left);
    } break;
    case lcCALL: {
      calculate(ctx, tree->left);
      //printf("func call\n");
      assign_value(tree->left);
      *val = tree->left->value.f;
    } break;
    }
  }
}


node_t *eval(xlang_context* ctx) {
  node_t *root;
  float retval = assignment_expression(ctx, &root);

  // functional(root,0);
  calculate(ctx, root);
  //printf("result = %f\n", root->value.f);
  return root;
}
