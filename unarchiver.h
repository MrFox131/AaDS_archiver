#ifndef UNARCHIVER_H
#define UNARCHIVER_H

#include <stdio.h>
#include <stdlib.h>
#include "haffman_tree_restorer.h"

typedef struct file
{
    char *name;
    int offset;
    int compressed_size;
    unsigned char archivating_algo;
} File;

typedef struct archive_structure
{
    File *files;
    int buffer_length;
    int files_counter;
} Archive_structure;

int haffman_unarchivate(FILE *in, FILE *out, int offset, int size);
int unarchivate(const char* in_file, const char *output_path);
Archive_structure *get_archive_structure(FILE *in);
void RLE_restoration(FILE *in, FILE *out, unsigned int offset, unsigned int size);

#endif