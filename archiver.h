#include <stdio.h>

#include "haffman_tree_builder.h"
#include "haffman_tree_packer.h"
#include "stack.h"
#include "get_directory_structure.h"

#ifndef ARCHIVER_H
#define ARCHIVER_H

typedef struct node Node;
int haffman_archivate(FILE *in, FILE *out);
int archivate(const char *name, const char *archived_file_path, unsigned char archivating_function);
int RLE(FILE *in, FILE *out);
#endif