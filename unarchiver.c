#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "haffman_tree_restorer.h"
#include "check_directory_properties.h"
#include "create_parent_dirs.h"

#define ARCHIVE_STRUCTURE_BUFFER_MULTIPLIER 10

typedef struct file
{
    char *name;
    int offset;
    unsigned int compressed_size;
    unsigned char archivating_algo;
} File;

typedef struct archive_structure
{
    File *files;
    int buffer_length;
    int files_counter;
    int archivating_function; //1- haffman, 2 - rle
} Archive_structure;

int haffman_unarchivate(FILE *in, FILE *out, unsigned int offset, unsigned int size)
{
    fseek(in, offset, SEEK_SET);
    RestoredNode *const tree_root = haffman_tree_restoration(in);
    size -= ftell(in) - offset;
    char buffer;
    RestoredNode *curr_node = tree_root;
    for (int i = 0; i < size - 2; i++)
    {
        buffer = getc(in);
        for (int j = 7; j >= 0; j--)
        {
            if ((buffer >> j) % 2)
            {
                curr_node = curr_node->right;
            }
            else
            {
                curr_node = curr_node->left;
            }
            if (curr_node->right == NULL && curr_node->left == NULL)
            {
                putc(curr_node->letter, out);
                curr_node = tree_root;
            }
        }
    }
    buffer = getc(in);
    char last_byte_significant_bits_count = getc(in);
    for (int j = last_byte_significant_bits_count - 1; j >= 0; j--)
    {
        if ((buffer >> j) % 2)
        {
            curr_node = curr_node->right;
        }
        else
        {
            curr_node = curr_node->left;
        }
        if (curr_node->right == NULL && curr_node->left == NULL)
        {
            putc(curr_node->letter, out);
            curr_node = tree_root;
        }
    }
    fclose(out);
    return 0;
}

int RLE_restoration(FILE *in, FILE *out, unsigned int offset, unsigned int size);

Archive_structure *get_archive_structure(FILE *in)
{
    fseek(in, -4, SEEK_END);

    unsigned int structure_buffer_reading_offest = 0;
    for (int i = 24; i >= 0; i -= 8)
    {
        unsigned char smth = getc(in);
        structure_buffer_reading_offest = ((structure_buffer_reading_offest >> i) | smth) << i;
    }
    fseek(in, -structure_buffer_reading_offest, SEEK_END);

    Archive_structure *archive_structure = malloc(sizeof(Archive_structure));
    archive_structure->files = calloc(ARCHIVE_STRUCTURE_BUFFER_MULTIPLIER, sizeof(File));
    archive_structure->files_counter = 0;
    archive_structure->buffer_length = ARCHIVE_STRUCTURE_BUFFER_MULTIPLIER;
    archive_structure->archivating_function = getc(in);
    structure_buffer_reading_offest--;
    int offset = 0;

    while (structure_buffer_reading_offest > 4)
    {
        unsigned int filename_length = 0;
        for (int i = 24; i >= 0; i -= 8)
        {
            unsigned char temporary = getc(in);
            filename_length = ((filename_length >> i) | temporary) << i;
        }
        if (archive_structure->buffer_length == archive_structure->files_counter)
        {
            archive_structure->files = realloc(archive_structure->files, (archive_structure->buffer_length + ARCHIVE_STRUCTURE_BUFFER_MULTIPLIER) * sizeof(File));
            archive_structure->buffer_length += ARCHIVE_STRUCTURE_BUFFER_MULTIPLIER;
        }
        archive_structure->files[archive_structure->files_counter].name = calloc(filename_length + 1, sizeof(char));
        fgets(archive_structure->files[archive_structure->files_counter].name, filename_length + 1, in);
        archive_structure->files[archive_structure->files_counter].offset = offset;

        unsigned int compressed_size = 0;
        for (int i = 24; i >= 0; i -= 8)
        {
            unsigned char temporary = getc(in);
            compressed_size = ((compressed_size >> i) | temporary) << i;
        }
        archive_structure->files[archive_structure->files_counter].compressed_size = compressed_size;
        offset += compressed_size;
        structure_buffer_reading_offest -= filename_length + 8;
        archive_structure->files_counter++;
    }
    archive_structure->files = realloc(archive_structure->files, archive_structure->files_counter * sizeof(File));
    return archive_structure;
}

int unarchivate(FILE *in, char *output_path)
{
    int smth = check_directory_properties(output_path);
    if (smth)
    {
        return smth;
    }

    Archive_structure *archive_structure = get_archive_structure(in);
    char *path = malloc(strlen(output_path) + 2);
    strcpy(path, output_path);
    int base_path_length = strlen(output_path);
    if (output_path[base_path_length - 1] != '/' && output_path[base_path_length - 1] != '\\')
    {
        path[base_path_length] = '\\';
        path[base_path_length + 1] = '\0';
        base_path_length += 1;
    }
    int (*function)(FILE *, FILE *, unsigned int, unsigned int);
    if (archive_structure->archivating_function == 1)
    {
        function = haffman_unarchivate;
    }
    else
    {
        function = RLE_restoration;
    }
    for (int i = 0; i < archive_structure->files_counter; i++)
    {
        path = realloc(path, base_path_length + strlen(archive_structure->files[i].name) + 1);
        create_parent_dirs(path);
        strcpy(path + base_path_length, archive_structure->files[i].name);
        FILE *out = fopen(path, "wb");
        function(in, out, archive_structure->files[i].offset, archive_structure->files[i].compressed_size);

        fclose(out);
        free(archive_structure->files[i].name);
    }
    free(archive_structure->files);
    free(archive_structure);
    free(path);

    return 0;
}

int RLE_restoration(FILE *in, FILE *out, unsigned int offset, unsigned int size)
{
    unsigned char temporary;
    fseek(in, offset, SEEK_SET);
    for (int i = 0; i < size;)
    {
        temporary = getc(in);
        if (temporary % 2 == 0)
        {
            int duplicating_sequence_length = (temporary >> 1) + 2;
            unsigned char temp = getc(in);
            for (int j = 0; j < duplicating_sequence_length; j++)
            {
                putc(temp, out);
            }
            i += 2;
        }
        else
        {
            int not_duplicating_sequence_length = (temporary >> 1) + 1;
            for (int j = 0; j < not_duplicating_sequence_length; j++)
            {
                putc(getc(in), out);
            }
            i += not_duplicating_sequence_length + 1;
        }
    }
    return 0;
}