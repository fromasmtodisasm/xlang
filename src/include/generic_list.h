#include <stdlib.h>
#include <stddef.h>
/* A linked list node */
struct Node
{
    // Any data type can be stored in this node
    void  *data;

    struct Node *next;
};

void push(struct Node** head_ref, void *new_data, size_t data_size);
void printList(struct Node *node, void (*fptr)(void *));
void *foreach_element(struct Node *head_ref, void *(*fptr)(void *, void *), void *data);

