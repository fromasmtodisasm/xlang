/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <stddef.h>
#include <stdio.h>

#include "common.h"
#include "interpreter.h"
//#include "syntax_parser.h"

static node_t *int_expression(node_t *root);
static void int_print(node_t *root);
void do_statements(node_t *node, int leve);
void do_statement(node_t *node, int level); 

node_t *int_expression(node_t *root) {
  calculate(root); 
  return root;
}

void exec_while(node_t *root, int level) {
  printf("in while\n");
  int cond;
  //printf("cond = %d\n", cond = (int)(int_expression(root->left)->value.f));
  do_statement(root->left, level + 1);
  do_statement(root->right, level + 1);
}

void int_print(node_t *root) {

}
void do_statement(node_t *node, int level); 

void do_statement(node_t *node, int level) {
  //printf("in function %s\n", __FUNCTION__);
  if (node != NULL)
  {
    assert(node->text != NULL);
    int i = level;
    for (; i > 0; printf(" "), i--);
    printf("node is < %s >\n", node->text);
    switch(node->type)
    {
      case lcPRINT: {
        printf("this is print operator\n");
        calculate(node->left);
      } break;
      case lcWHILE: {
        //exec_while(node, level);
        puts("begin while");
        do_statement(node->left, level + 1);
        puts("after left");
        do_statement(node->right, level + 1);
        assert(node->right != NULL);
        puts("end while");
        //do_statements(node->right, level + 1); 
      } break;
      case lcEXP:
      {
        calculate(node->right);
        printf("val exp = %f\n", node->right->value.f);
      } break;
      case lcBLOCK:
      {
        printf("this is block\n");
        do_statement(node->right, level + 1);
      } break;
      case lcSTMNT:
      {
        puts("in statement");
        do_statement(node->left, level);
      }

    }

    do_statement(node->left, level + 1);
    do_statement(node->right, level + 1);
  }
}

void do_statements(node_t *node_, int level) {
  node_t *node = node_;// = node_->left;
  //printf("in function %s\n", __FUNCTION__);
  if (node_ == NULL) return;
  //for (node= node_; node != NULL && node != NULL; node = node->left)
  //{
    puts("head");
    if (node != NULL) {
      //assert(node->text != NULL);
      int i = level;
      for (; i > 0; printf(" "), i--);
      printf("node is < %s >\n", node->text);
      switch(node->right->type)
      {
        case lcPRINT: {
          printf("this is print operator\n");
          calculate(node->left);
        } break;
        case lcWHILE: {
          //exec_while(node, level);
          puts("begin while");
          do_statement(node->left, level + 1);
          do_statements(node->right, level + 1);
          assert(node->right != NULL);
          puts("end while");
          //do_statements(node->right, level + 1); 
        } break;
        case lcEXP:
        {
          //calculate(node->right->right);
      do_statements(node->left, level + 1);
          printf("val exp = %f\n", node->right->right->value.f);
        } break;
        case lcBLOCK:
        {
          do_statements(node->right, level + 1);
        } break;
        case lcSTMNT:
        {
          puts("in statement");
          do_statements(node->right->right, level);
        }break;
        default:
          printf("unknown stmnt is %s with code = %d\n", node->right->text, node->right->type);
          printf("unknown code = %d\n", lcUNKNOWN);

      }
      puts("end");
      //assert(node->left != NULL);
      do_statements(node->left, level + 1);
      //do_statements(node->right, level + 1);

      }
    //}
}

void extern_defs(node_t *root)
{
  node_t *node = root;
  printf("root = %s\n------\n", root->right->text);
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
  printf("root text: %s\n", syntax_tree->right->text);
  //assert(syntax_tree->right != NULL);
  extern_defs(syntax_tree);
  //do_statements(syntax_tree->right,0);  
  return 0;
}
