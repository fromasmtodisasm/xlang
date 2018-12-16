#include "common.h"
#include "config.h"
#include "exp.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define begin_func()                                                           \
  DEBUG(stderr, "Function is %s line is %d\n", __FUNCTION__, __LINE__)
#define end_func()                                                             \
  DEBUG("On line [%d]\n", __LINE__) // fprintf(stderr, "Function %s is end on
                                     // line %d\n", __FUNCTION__, __LINE__)
#ifdef BTREE_USE

#include "tree.h"

int cmp_var_name(const void *left, const void *right);
typedef struct btree_node_t listof;

listof *vars;
#define push_var(var) btree_insert(&vars, &var, sizeof(var), cmp_var_name)
#define find_var(var) btree_search(vars, &var, cmp_var_name)
#define VAR_STORAGE "BTREE_USE"

#elif LIST_USE

#include "generic_list.h"

void *cmp_var_name(const void *vars, const void *data);
typedef struct list_t listof;

listof *vars;
#define push_var(var) push(&vars, &var, sizeof(var))
#define find_var(var) foreach_element(vars, var.name, cmp_var_name)
#define VAR_STORAGE "LIST_USE"

#endif

const char *var_storage = VAR_STORAGE;

int make_builtin_vars() {
  variable _false = { "false", 0 };
  variable _true = { "true", 1, };
  push_var(_false);
  push_var(_true);
}

int exp_parser_init() { 
  DEBUG("Used var storage: %s\n", var_storage);
  make_builtin_vars(); 
}

#ifdef BTREE_USE

int cmp_var_name(const void *left, const void *right)
{
  return strcmp(((variable*)left)->name, ((variable*)right)->name) == 0;
}

#endif //BTREE_USE

#ifdef LIST_USE

void *cmp_var_name(const void *vars, const void *data)
{
  char *name = (char*)data;
  variable *cur_var = (variable*)vars;
  if (!strcmp(name, cur_var->name)) {
    DEBUG("founded var with name  = %s\n", name);
    return cur_var;
  }
  return 0;
}

#endif //LIST_USE

void print_var(void *var)
{
  printf("var name = %s\n", ((variable*)(var))->name);
}
int lookup(char *name, float *val) {
  variable *cur_var;
  int res = 0;
  variable *var = NULL;
  variable tmp;
  tmp.name = name;
  if (var = find_var(tmp))
  {
    *val = var->value;
    res = 1;
  }
  return res;
}

float assign_value(char *name, float val) {
  variable *cur_var;
  variable tmp;
  tmp.name = name;
  if (cur_var = find_var(tmp)) {
    cur_var->name = name;
    cur_var->value = val;
  }
  else  {
    
    tmp.name = name;
    tmp.value = val;
    push_var(tmp);
  }
  return val;
}

int primary_expression(node_t **root) {
  int res = 0;

  switch (curr_token->type) {

  case lcNUMBER:
  case lcSTRING:
  case lcIDENT: {
    *root = create_node();
    assert(curr_token->text != NULL);
    assert(*root != NULL);
    (*root)->text = strdup(curr_token->text);
    (*root)->type = curr_token->type;
    break;
  }
  case lcLBRACE: {
    get_token(/*NEXT_TOKEN*/);
    res = assignment_expression(root);
    if (curr_token->type != lcRBRACE) {
      printf("Error, expected ')'\n");
      getchar();
      // exit(-1);
    }
    break;
  }
  default:
    ERROR("Error, expected primary on line %d!!! ", get_line());
    ERROR("Meeted %s\n", curr_token->text);
    getchar();
    exit(-1);
  }
  get_token();
  return res;
}

int postfix_expression(node_t **root)
{
  int res = 0;
  node_t *node;
  int need_get = TRUE;
  
  res = primary_expression(root);

  switch (curr_token->type)
  {
  case lcPLUS_PLUS: {
  case lcMINUS_MINUS:
    DEBUG("This id postfix expression: %s\n", curr_token->text);
  } break;
  case '[': {
    get_token();
    res = primary_expression(root);
    if (curr_token->type != ']')
      ERROR("Expected ']'\n");
  } break;
  case lcLBRACE: {
    if (get_token()->type != lcRBRACE)
    {
      /*
      do {
        res = primary_expression(root);
      }while(curr_token->type == lcCOMMA);
      */
      res = primary_expression(root);
      while(curr_token->type == lcCOMMA) {
        get_token();
        res = primary_expression(root);
      }
      if (curr_token->type != lcRBRACE)
        ERROR("Expected RBRACE\n");
    }
    else
    {
      printf("Empty arg list\n");
    }
  } break;
  case lcPOINT: {
  case lcARROW:
    if (get_token()->type != lcIDENT) {
      ERROR("Expected identifier on line %d\n", get_line());
    } 
    else {
      get_token();
      need_get = FALSE;
      while (curr_token->type == lcPOINT, curr_token->type == lcARROW) {
        if (get_token()->type == lcIDENT)
        {
          get_token();
        }
        else break;
      }
    }
  } break;

  default: {
    need_get = FALSE; 
  } break;
  }

  if (need_get) get_token();
  return res;
}

int unary_expression(node_t **root)
{
  int res = 0;
  node_t *node;

  res = postfix_expression(root);

  return res;
}

int multiplicative_expression(node_t **root) {
  int res = 0;
  int stop = 0;
  node_t *node;

  // Build left subtree
  res = unary_expression(root);
  while (curr_token->type == lcMUL || curr_token->type == lcDIV) {
    node = create_node();
    assert(curr_token->text != NULL);
    assert(node != NULL);
    node->left = (*root);
    node->text = strdup(curr_token->text);
    node->type = curr_token->type;
    get_token();
    unary_expression(&(node->right));
    *root = node;
  }
  assert(*root != NULL);
  return res;
}

int additive_expression(node_t **root) {
  int res = 0;
  node_t *node;

  res = multiplicative_expression(root);
  assert(*root != NULL);
  while (curr_token->type == lcPLUS || curr_token->type == lcMINUS) {
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

int is_relop(token_type type) {
  return (lcAND_OP == type || lcOR_OP == type || lcLE_OP == type ||
          lcGE_OP == type || lcG_OP == type || lcEQ_OP == type ||
          lcL_OP == type || lcNE_OP == type);
}

int conditional_expression(node_t **root) {
  int res = 0;
  node_t *node;
  token_type type = curr_token->type;
  res = additive_expression(root);

  while (is_relop(type = curr_token->type)) {
    node = create_node();
    assert(curr_token->text != NULL);
    assert(node != NULL);
    node->left = *root;

    node->text = strdup(curr_token->text);

    node->type = curr_token->type;

    get_token(/*NEXT_TOKEN*/);
    additive_expression(&(node->right));
    *root = node;

    type = curr_token->type;
  }
  assert(*root != NULL);
  return res;
}

int assignment_expression(node_t **root) {
  char *name;
  int tmp = 0;
  int res = 0;
  token_t prev_token;

  node_t *node;

  name = curr_token->text;
  if (curr_token->type == lcSEMI) {
    return res;
  }
  if (curr_token->type == lcIDENT) {
    char *name;
    int tmp = 0;
    name = curr_token->text;
    char *prev_pos = get_pos();
    memcpy(&prev_token, curr_token, sizeof(token_t));

    *root = create_node();
    (*root)->text = strdup(curr_token->text);
    (*root)->type = curr_token->type;
    token_type type = get_token()->type;
    if (type == lcASSIGN || type == lcPLUS_ASSIGN || type == lcMINUS_ASSIGN ||
        type == lcMUL_ASSIGN || type == lcDIV_ASSIGN) {
      node = create_node();
      assert(curr_token->text != NULL);
      assert(node != NULL);
      node->left = *root;
      node->text = strdup(curr_token->text);
      node->type = curr_token->type;

      get_token(/*NEXT_TOKEN*/);

      assignment_expression(&(node->right));
      *root = node;
      return res;
    } else {
      memcpy(curr_token, &prev_token, sizeof(token_t));
      set_pos(prev_pos);
    }
  }
  res = conditional_expression(root);
  return res;
}

void make_space(int n) {
  for (; n > 0; n--) {
    printf(" ");
  }
}
void print_node(node_t *node, int level) {
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

void functional(node_t *tree, int level) {
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

void calculate(node_t *tree, float *val) {
  float val1 = 0, val2 = 0;
  if (tree) {
    calculate(tree->left, &val1);
    calculate(tree->right, &val2);
    switch (tree->type) {
    case lcPLUS:
      *val = val1 + val2;
      break;
    case lcMINUS:
      *val = val1 - val2;
      break;
    case lcMUL:
      *val = val1 * val2;
      break;
    case lcDIV:
      *val = val1 / val2;
      break;
    case lcINC_OP:
      *val++;
      break;
    case lcDEC_OP:
      *val--;
      break;

    case lcAND_OP:
      *val = (val1 && val2);
      break;
    case lcOR_OP:
      *val = (val1 || val2);
      break;
    case lcLE_OP:
      *val = (val1 <= val2);
      break;
    case lcGE_OP:
      *val = (val1 >= val2);
      break;
    case lcEQ_OP:
      *val = (val1 == val2);
      break;
    case lcNE_OP:
      *val = (val1 != val2);
      break;

    case lcG_OP:
      *val = (val1 > val2);
      break;
    case lcL_OP:
      *val = (val1 < val2);
      break;

    case lcNUMBER:
      *val = atof(tree->text);
      break;
    case lcIDENT:
      if (!lookup(tree->text, val)) {
        printf("Undefined var: %s\n", tree->text);
      }
      break;

    case lcASSIGN: {
      float res;
      calculate(tree->right, &res);
      *val = assign_value(tree->left->text, res);
    } break;
    case lcPLUS_ASSIGN: {
      float res;
      calculate(tree->right, &val2);
      *val = assign_value(tree->left->text, val1 + val2);
      // printf("plus_asign = %f\n", *val);
    } break;
    case lcMINUS_ASSIGN: {
      float res;
      calculate(tree->right, &val2);
      *val = assign_value(tree->left->text, val1 - val2);
    } break;
    case lcMUL_ASSIGN: {
      float res;
      calculate(tree->right, &val2);
      *val = assign_value(tree->left->text, val1 * val2);
    } break;
    case lcDIV_ASSIGN: {
      float res;
      calculate(tree->right, &val2);
      *val = assign_value(tree->left->text, val1 / val2);
    } break;
    }
  }
}

float eval() {
  node_t *root;
  float retval = assignment_expression(&root);

  // functional(root,0);
  calculate(root, &retval);
  DEBUG("result = %f\n", retval);
  return retval;
}
