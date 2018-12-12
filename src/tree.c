#include "tree.h"
#include <malloc.h>

node_t *create_node()
{
  node_t *node;

  node = malloc(sizeof(node_t));
  node->left = NULL;
  node->right = NULL;
  node->value = 0;
  return node;
}
