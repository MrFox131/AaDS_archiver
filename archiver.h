#include <stdio.h>

#include "haffman_tree_builder.h"
#include "haffman_tree_packer.h"
#include "stack.h"
#include "get_directory_structure.h"

#ifndef ARCHIVER_H
#define ARCHIVER_H

typedef struct node Node;
int haffman_archivate(FILE *in, FILE *out, char extended_pipeline);
int archivate(char *name, char *archived_file_path, unsigned char archivating_function);
int RLE(FILE *in, FILE *out);
// int LZ78(unsigned char **buffer, int buffer_length);
// int LZ78_restoration(unsigned char **buffer, int buffer_length);
#endif