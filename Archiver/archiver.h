#include <stdio.h>

#include "haffman_tree_builder.h"
#include "haffman_tree_packer.h"
#include "stack.h"

#ifndef ARCHIVER_H
#define ARCHIVER_H

typedef struct node Node;
/*
 * Архивация файла in с выводом в файл out
 */
int haffman_archivate(_iobuf* *in, char* *out);
int archivate(const char *name, const char *archived_file_path, unsigned char archivating_function);
int RLE(FILE *in, FILE *out);
#endif
