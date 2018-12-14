#include "exp.h"
#include "generic_list.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define begin_func()                                                           \
  fprintf(stderr, "Function is %s line is %d\n", __FUNCTION__, __LINE__)
#define end_func()                                                             \
  printf("On line [%d]\n", __LINE__) // fprintf(stderr, "Function %s is end on
                                     // line %d\n", __FUNCTION__, __LINE__)
typedef struct Node listof;

listof *vars;
// static token_t *curr_token;// = curr_token;

typedef struct name_val
{
  char *name;
  void *val;
}name_val;

int make_builtin_vars() {
  //vars = malloc(sizeof(variable));
  variable false = {"false", 0};
  variable true = {
      "true",
      1,
  };
  push(&vars, &true, sizeof(variable));
  push(&vars, &false, sizeof(variable));
}

int exp_parser_init() { 
  make_builtin_vars(); 
}

int block(char **buffer) { return 0; }

void *cmp_var_name(void *vars, void *data)
{
  char *name = (char*)data;
  variable *cur_var = (variable*)vars;
  if (!strcmp(name, cur_var->name)) {
    printf("founded var with name  = %s\n", name);
    return cur_var;
  }
  return 0;
}
int lookup(char *name, float *val) {
  variable *cur_var;
  int res = 0;
  //printf("Finding %s varilable\n", name, val);
  variable *var = NULL;
  if (var = exist_element(vars, cmp_var_name, name))
  {
    *val = var->value;
    res = 1;
  }
  return res;
}

float assign_value(char *name, float val) {
  variable *cur_var;
  variable tmp;
  printf("assign value with len = %d for %s\n", strlen(name), name);
  if ((cur_var = exist_element(vars, cmp_var_name, name))) {
    cur_var->name = name;
    cur_var->value = val;
  }
  else  {
    printf("create new var [%s] with val = %f on line %d\n", name, val, get_line());
    tmp.name = name;
    tmp.value = val;
    puts("push");
    push(&vars, &tmp, sizeof(variable));
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
    printf("Error, expected primary on line %d!!!\n", get_line());
    getchar();
    exit(-1);
  }
  get_token();
  return res;
}

int multiplicative_expression(node_t **root) {
  int res = 0;
  int stop = 0;
  node_t *node;

  // Build left subtree
  res = primary_expression(root);
  while (curr_token->type == lcMUL || curr_token->type == lcDIV) {
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
      printf("eq_op\n");
      break;
    case lcNE_OP:
      *val = (val1 != val2);
      printf("ne_op\n");
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
  // printf("result = %f\n", retval);
  return retval;
}
