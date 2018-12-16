#include <stdlib.h>
#include <stddef.h>
/* A linked list node */
struct list_t
{
    // Any data type can be stored in this node
    void  *data;

    struct list_t *next;
};

void push(struct list_t** head_ref, void *new_data, size_t data_size);
void printList(struct list_t *node, void (*fptr)(void *));
void *foreach_element(struct list_t *node, void *data, void *(*fptr)(const void *, const void*));
