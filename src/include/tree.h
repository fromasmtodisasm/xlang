#pragma once

typedef struct node_t node_t;

struct node_t {
  int type;
  char *text;
  float value;
  node_t *left, *right;
};

node_t *create_node();
