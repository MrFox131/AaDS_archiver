#include <stdio.h>

#include "haffman_tree_builder.h"
#include "haffman_tree_packer.h"
#include "stack.h"
#include "get_directory_structure.h"

#ifndef ARCHIVER_H

typedef struct node Node;
int codes_generator(Node *root, unsigned char *codes[256], int *codes_length);
int haffman_archivate(FILE *in, FILE *out);
int archivate(char *name, char *awrchived_file_path);

#endif