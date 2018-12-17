#ifndef _TREE_H_
#define _TREE_H_

/***************************************************************************/
/************************** Other Includes *********************************/
/***************************************************************************/

/***************************************************************************/
/***************************** Defines *************************************/
/***************************************************************************/

/***************************************************************************/
/*********************** Typedefs/Structures *******************************/
/***************************************************************************/
typedef struct node_t node_t;

struct node_t {
  int type;
  char *text;
  struct {
    union {
      int i;
      char c;
      char *s;
      float f;
      double d;
      void *b;
    };
  }value;
  node_t *left, *right;
};

node_t *create_node();

struct btree_node_t {
    struct btree_node_t *left;
    struct btree_node_t *right;
    void *item;
};

/***************************************************************************/
/*************************** Prototypes ************************************/
/***************************************************************************/
node_t *create_node();
static void btree_free_node(struct btree_node_t *node);
static struct btree_node_t* find_min_node(struct btree_node_t *node);
static struct btree_node_t* find_max_node(struct btree_node_t *node);
void *btree_search(struct btree_node_t *root, void *item, int (*comp)(const void*,const void*));
void *btree_insert(struct btree_node_t **root, void *item, unsigned int size, int (*comp)(const void*,const void*));
struct btree_node_t* btree_delete_node(struct btree_node_t *root, void *item, unsigned int size, int (*compare_node)(const void*,const void*));
void btree_print(struct btree_node_t *root, void (*print)(const void *));
void btree_free(struct btree_node_t *root);

/***************************************************************************/
/********************* Externally Defined Globals **************************/
/***************************************************************************/

#endif
