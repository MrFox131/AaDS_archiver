#ifndef HAFFMAN_TREE_PACKER_H
#define HAFFMAN_TREE_PACKER_H
#include "haffman_tree_builder.h"

typedef struct node Node;
void haffman_tree_packer(Node* tree, int *real_length, int *buffer_length, unsigned char* buffer);

#endif