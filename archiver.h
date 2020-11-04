#include <stdio.h>

#include "haffman_tree_builder.h"     
#include "haffman_tree_packer.h" 
#include "stack.h" 

#ifndef ARCHIVER_H

typedef struct node Node;
int codes_generator(Node* root, unsigned char *codes[256], int *codes_length);
int haffman_archivate(FILE *in, FILE* out, int packed_tree_length, unsigned char *packed_tree, Node* root);

#endif