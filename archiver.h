#include <stdio.h>

#include "haffman_tree_builder.h"
#include "haffman_tree_packer.h"
#include "stack.h"
#include "get_directory_structure.h"

#ifndef ARCHIVER_H

typedef struct node Node;
int codes_generator(Node *root, unsigned char *codes[256], int *codes_length);
int haffman_archivate(FILE *in, FILE *out, char extended_pipeline);
int archivate(char *name, char *awrchived_file_path, char extended_pipeline);
int RLE(char **buffer, int buffer_length);
#endif