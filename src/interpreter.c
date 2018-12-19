/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <stddef.h>
#include <stdio.h>

#include "common.h"
#include "interpreter.h"
//#include "syntax_parser.h"
//
#define PRINT_PAD(lvl,...) { int i = lvl; \
  for (; i > 1; printf("| "), i--); printf("+");} \
  printf(__VA_ARGS__);

static node_t *exec_expression(node_t *root,int level);
static void int_print(node_t *root);
void do_statements(node_t *node, int leve);
void do_statement(node_t *node, int level); 

node_t *exec_expression(node_t *root,int level) {
  //calculate(root); 
  if (root != NULL) {
    PRINT_PAD(level,"node is < %s >\n", root->text);
    exec_expression(root->left,level+1);
    exec_expression(root->right,level+1);
    //PRINT_PAD(level,"node is < %s >\n", root->text);
  }
  calculate(root);
  return root;
}

/*
void exec_while(node_t *root, int level) {
  printf("in while\n");
  int cond;
  //printf("cond = %d\n", cond = (int)(exec_expression(root->left)->value.f));
  do_statement(root->left, level + 1);
  do_statement(root->right, level + 1);
}
*/

void int_print(node_t *root) {

}
void do_statement(node_t *node, int level); 

void exec_while(node_t *node, int level)
{
  //exec_while(node, level);
  PRINT_PAD(level,"begin while\n");
  //PRINT_PAD(level,"check condition\n");
  do_statements(node->left, level + 1);
  do_statements(node->right, level + 1);
  assert(node->right != NULL);
  PRINT_PAD(level,"end while\n");
  //do_statements(node->right, level + 1); 

}

void exec_if(node_t *node, int level)
{
  //exec_while(node, level);
  PRINT_PAD(level,"begin if\n");
  //PRINT_PAD(level,"check condition\n");
  //do_statements(node->left, level + 1);

  if ((int)exec_expression(node->left->right, level)->value.f != 0) {
    do_statements(node->right, level + 1);
  }
  assert(node->right != NULL);
  PRINT_PAD(level,"end if\n");
  //do_statements(node->right, level + 1); 

}

void do_statements(node_t *node, int level) {
  //printf("in function %s\n", __FUNCTION__);
  if (node != NULL) {
  //for (node= node_; node != NULL && node != NULL; node = node->left)
  //{
    PRINT_PAD(level,"begin\n");
    if (node != NULL) {
      //assert(node->text != NULL);
      int i = level;
      PRINT_PAD(level,"node is < %s >\n", node->text);
      switch(node->type)
      {
        case lcPRINT: {
          PRINT_PAD(level,"this is print operator\n");
          calculate(node->left);
        } break;
        case lcWHILE: {
          exec_while(node,level);
          return;
        } break;
        case lcIF: {
          exec_if(node,level); 
          return;
        } break;
        case lcEXP:
        {
          calculate(node->right->right);
          //do_statements(node->left, level + 1);
          exec_expression(node->right, level);
          PRINT_PAD(level,"val exp = %f\n", node->right->right->value.f);
        } break;
        case lcBLOCK:
        {
          do_statements(node->right, level + 1);
        } break;
        case lcSTMNT:
        {
          PRINT_PAD(level,"in statement\n");
          do_statements(node->right, level);
        }break;
        case lcVARDEF:
        {
          PRINT_PAD(level,"var define\n");
          return;
        } break;
        default:
          printf("unknown stmnt is %s with code = %d\n", node->right->text, node->right->type);
          printf("unknown code = %d\n", lcUNKNOWN);

      }
      //PRINT_PAD(level,"end\n");
      //assert(node->left != NULL);
      do_statements(node->left, level + 1);
      //do_statements(node->right, level + 1);
      PRINT_PAD(level,"end\n");

      }
    //}
  }
}

void extern_defs(node_t *root)
{
  node_t *node = root;
  printf("root = %s\n------\n", root->text);
  for( ; node->right != NULL; node = node->left)
  {
    switch (node->right->type) {
      case lcVARDEF:
        {
          puts("this is vardef");
        }break; 
      case lcFUNCTION:
        {
          printf("this is func %s\n", node->right->text);
          do_statements(node->right->right,1);
        } break;
      default:
        {
          printf("code type %d\n", node->right->type);
          puts("unknown node");
        }
    }
  }
}

int interprete(node_t *syntax_tree) {
  assert(syntax_tree != NULL);
  printf("Start interpreting...\n"); 
  assert(syntax_tree->text != NULL);
  printf("root text: %s\n", syntax_tree->text);
  //assert(syntax_tree->right != NULL);
  extern_defs(syntax_tree);
  //do_statements(syntax_tree->right,0);  
  return 0;
}
