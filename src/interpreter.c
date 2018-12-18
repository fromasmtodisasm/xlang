/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <stddef.h>
#include <stdio.h>

#include "common.h"
#include "interpreter.h"
//#include "syntax_parser.h"
void print_statements(node_t *node, int leve);

void print_tree(node_t *node, int level) {
  if (node != NULL)
  {
    if (node->type != lcBLOCK) {
      assert(node->text != NULL);
      int i = level;
      for (; i > 0; printf(" "), i--);
      printf("node is < %s >\n", node->text);

      print_tree(node->left, level + 1);
      print_tree(node->right, level + 1);
    }
    else { 
      //printf("this is block %s\n", node->text); 
      print_statements(node->right, level + 1); 
    }
  }
}

void print_statements(node_t *node, int leve) {
  node_t *tmp = NULL;
  for (tmp = node; tmp->right != NULL; tmp = tmp->left)
  {
    puts("test");
    assert(tmp != NULL);
    //assert(tmp->text != NULL);
    //printf("%s\n", tmp->right->text);
    print_tree(tmp->right, 0);
  }
}
int interprete(node_t *syntax_tree) {
  assert(syntax_tree != NULL);
  printf("Start interpreting...\n"); 
  assert(syntax_tree->text != NULL);
  printf("root text: %s\n", syntax_tree->text);
  print_statements(syntax_tree,0);  
  return 0;
}
