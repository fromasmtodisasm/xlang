#include "tree.h"
#include <malloc.h>

node_t *create_node() {
  node_t *node;

  node = malloc(sizeof(node_t));
  node->left = NULL;
  node->right = NULL;
  node->value = 0;
  return node;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *btree_insert(struct btree_node_t **root, void *item, unsigned int size, int (*compare_node)(const void*,const void*)) {
    // Insert the root
    if (*root == NULL) {
        *root = malloc(sizeof(struct btree_node_t));
        if (!(*root)) {
            fprintf(stderr,"malloc() fail\n");
            return NULL;
        }
        (*root)->left = (*root)->right = NULL;
        (*root)->item = malloc(size);
        if (!((*root)->item)) {
            fprintf(stderr,"malloc() fail\n");
            free(*root);
            return NULL;
        }
        memcpy((*root)->item,item,size);
    } else {
        if (compare_node((*root)->item,item) > 0) {
            //Insert left
            btree_insert(&(*root)->left,item,size,compare_node);
        } else {
            //Insert right
            btree_insert(&(*root)->right,item,size,compare_node);
        }
    }
    return *root;
}

static void btree_free_node(struct btree_node_t *node) {
    free(node->item);
    free(node);
}

static struct btree_node_t* find_min_node(struct btree_node_t *node) {
    node = node->right;
    while (node) node = node->left;
    return node;
}

static struct btree_node_t* find_max_node(struct btree_node_t *node) {
    node = node->left;
    while (node) node = node->right;
    return node;
}

struct btree_node_t* btree_delete_node(struct btree_node_t *root, void *item, unsigned int size, int (*compare_node)(const void*,const void*)) {
    if (root == NULL) return root;
    else if (compare_node(item,root->item) < 0) root->left = btree_delete_node(root->left,item,size,compare_node);
    else if (compare_node(item,root->item) > 0) root->right = btree_delete_node(root->right,item,size,compare_node);
    else {
        // 1. Deleting a node with two children
        if ( root->left && root->right ) {
            struct btree_node_t *min_node = find_min_node(root);
            if (!min_node) {
                min_node = find_max_node(root);
            }
            memcpy(root->item,min_node->item,size);
            root->right = btree_delete_node(root->right,min_node->item,size,compare_node);
        } else if (root->left) {
            // 2. Deleting a node with one child (left)
            struct btree_node_t *node_delete = root;
            root = root->left;
            btree_free_node(node_delete);
        } else if (root->right) {
            // 2. Deleting a node with one child (right)
            struct btree_node_t *node_delete = root;
            root = root->right;
            btree_free_node(node_delete);
        } else {
            // 3. Deleting a leaf node
            btree_free_node(root);
            root = NULL;
        }
    }
    return root;
}

void btree_print(struct btree_node_t *root, void (*print_node)(const void *)) {
    if (root) {
        print_node(root->item);
        btree_print(root->left,print_node);
        btree_print(root->right,print_node);
    }
}

void btree_free(struct btree_node_t *root) {
    if (root) {
        free(root->item);
        btree_free(root->left);
        btree_free(root->right);
        free(root);
    }
}

void *btree_search(struct btree_node_t *root, void *item, int (*compare_node)(const void*,const void*)) {
    if (root == NULL) return NULL;
    else if (compare_node(item,root->item) > 0) return btree_search(root->right, item, compare_node);
    else if (compare_node(item,root->item) < 0) return btree_search(root->left, item, compare_node);
    else return root->item;
}
