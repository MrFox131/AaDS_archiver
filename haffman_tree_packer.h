#ifndef HAFFMAN_TREE_PACKER_H
#define HAFFMAN_TREE_PACKER_H
#include "haffman_tree_builder.h"
#include "haffman_tree_packer.c"
void haffman_tree_packer(struct node* tree, int *reallength, int *buffer_length, unsigned char* buffer);

#endif