#include <stdio.h>

#include "haffman_tree_builder.h"
#include "haffman_tree_packer.h"
#include "stack.h"
#include "get_directory_structure.h"
#include <string.h>

#define STRUCTURE_BUFFER_MULTIPLIER 1024
#define ARCHIVATING_BUFFER_MULTIPLIER 1024

typedef struct node Node;

void add_uinteger_to_buffer(unsigned int number, unsigned char **buffer, int pos_in_buffer)
{
    unsigned char mask = 255;
    (*buffer)[pos_in_buffer] = (number >> 24) & mask;
    (*buffer)[pos_in_buffer + 1] = (number >> 16) & mask;
    (*buffer)[pos_in_buffer + 2] = (number >> 8) & mask;
    (*buffer)[pos_in_buffer + 3] = (number)&mask;
}
//generates code for all symbols in tree, memorizing symbol_codes not only for letters, but for non-leaf nodes too
int codes_generator(Node *root, unsigned char *codes[256], int *codes_length)
{
    Stack *stack = create_stack();
    stack_push(stack, root->left);
    stack_push(stack, root->right);
    while (!stack_is_empty(stack))
    {
        root = pop(stack);
        find_symbol_code(root);
        if (root->is_letter)
        {
            codes[root->letter] = root->symbol_code;
            codes_length[root->letter] = root->depth;
            continue;
        }
        stack_push(stack, root->left);
        stack_push(stack, root->right);
    }
    stack_destructor(stack);
}

int RLE(FILE *in, FILE *out);
int LZ78(unsigned char **buffer, int buffer_length);

unsigned int haffman_archivate(FILE *in, FILE *out)
{
    Node *zero_node;
    Node *haffman_prefix_codes_tree = haffman_tree_builder(in, &zero_node);
    unsigned char *packed_tree = calloc(1024, sizeof(unsigned char));
    int packed_tree_length = 0, buffer_length = 1024;
    haffman_tree_packer(haffman_prefix_codes_tree, &packed_tree_length, &buffer_length, &packed_tree);

    unsigned char *(symbol_codes)[256], coded_sequence_buffer, coding_symbol_buffer, mask = 255, filled_by = 0;
    int *codes_length = calloc(256, sizeof(int)), cnt = 0;

    fseek(in, 0, SEEK_END);
    int end = ftell(in);
    fseek(in, 0, SEEK_SET);

    codes_generator(haffman_prefix_codes_tree, symbol_codes, codes_length);
    for (int i = 0; i < packed_tree_length; i++)
    {
        fputc(packed_tree[i], out);
    }

    while (ftell(in) != end)
    {
        coding_symbol_buffer = fgetc(in);
        for (int i = 0; i < codes_length[coding_symbol_buffer]; i++)
        {
            if (filled_by == 8)
            {
                filled_by = 0;
                fputc(coded_sequence_buffer, out);
                coded_sequence_buffer = 0;
                cnt++;
            }
            coded_sequence_buffer = coded_sequence_buffer << 1;
            coded_sequence_buffer = coded_sequence_buffer | (symbol_codes[coding_symbol_buffer][i] >> 1);
            filled_by++;
        }
    }

    if (filled_by != 0)
    {
        fputc(coded_sequence_buffer, out);
        cnt++;
    }
    fputc(filled_by == 0 ? 8 : filled_by, out);
    cnt++;

    Node *temp_zero_node = zero_node;
    Stack *stack = create_stack();
    stack_push(stack, zero_node->left);
    stack_push(stack, zero_node->right);
    while (!stack_is_empty(stack))
    {
        zero_node = pop(stack);
        if (zero_node != NULL)
        {
            free(zero_node->symbol_code);
            stack_push(stack, zero_node->left);
            stack_push(stack, zero_node->right);
        }
    }
    stack_destructor(stack);

    free(packed_tree);
    free(temp_zero_node);
    free(codes_length);
    return packed_tree_length + cnt;
}

int archivate(char *archivating_file_name, char *archived_file_name, unsigned char archivating_function)
{

    char *cleared_path = calloc(strlen(archivating_file_name) + 1, sizeof(char));
    strcpy(cleared_path, archivating_file_name);
    if (cleared_path[strlen(cleared_path) - 1] == '\\' || cleared_path[strlen(cleared_path) - 1] == '/')
        cleared_path[strlen(cleared_path) - 1] = '\0';
    Directory *dir = get_directory_structure(cleared_path);

    unsigned char *structure_buffer = calloc(STRUCTURE_BUFFER_MULTIPLIER, sizeof(char));
    int structure_buffer_size = STRUCTURE_BUFFER_MULTIPLIER;
    unsigned int current_carriage_pos = 1;
    int base_path_length = strlen(dir->base);
    FILE *out_file = fopen(archived_file_name, "wb");
    if (out_file == NULL)
    {
        return -1;
    }
    structure_buffer[0] = archivating_function;
    for (int i = 0; i < dir->files_count; i++)
    {
        unsigned int filename_length = strlen(dir->files[i]) - base_path_length;
        while (structure_buffer_size - current_carriage_pos < filename_length + 9)
        {
            structure_buffer = realloc(structure_buffer, structure_buffer_size + STRUCTURE_BUFFER_MULTIPLIER);
            structure_buffer_size += STRUCTURE_BUFFER_MULTIPLIER;
        }
        add_uinteger_to_buffer(filename_length, &structure_buffer, current_carriage_pos);

        strncpy(structure_buffer + current_carriage_pos + 4, dir->files[i] + base_path_length, filename_length);
        current_carriage_pos += 4 + filename_length;
        FILE *archivating_file = fopen(dir->files[i], "rb");
        unsigned int compressed_file_size;
        if (archivating_function == 1)
        {
            compressed_file_size = haffman_archivate(archivating_file, out_file);
        }
        else
        {
            compressed_file_size = RLE(archivating_file, out_file);
        }
        fclose(archivating_file);
        add_uinteger_to_buffer(compressed_file_size, &structure_buffer, current_carriage_pos);

        // structure_buffer[current_carriage_pos + 4] = archivating_function;
        current_carriage_pos += 4;
    }
    if (structure_buffer_size - current_carriage_pos < 4)
    {
        structure_buffer = realloc(structure_buffer, (4 - (structure_buffer_size - current_carriage_pos)) * sizeof(char));
    }
    add_uinteger_to_buffer(current_carriage_pos + 4, &structure_buffer, current_carriage_pos);

    current_carriage_pos += 4;
    for (int i = 0; i < current_carriage_pos; i++)
    {
        putc(structure_buffer[i], out_file);
    }

    fclose(out_file);
    free(structure_buffer);
    free(cleared_path);
    destruct_dir(dir);
    return 0;
}

int RLE(FILE *in, FILE *out)
{
    unsigned char last_character = getc(in);
    unsigned char not_duplicating_symbols_counter = 0, duplicating_symbols_counter = 0;
    int current_carriage_position = 0;
    fseek(in, 0, SEEK_END);
    int end = ftell(in);
    fseek(in, 1, SEEK_SET);
    unsigned char temporary;
    while (ftell(in) != end)
    {
        temporary = getc(in);
        if (temporary == last_character)
        {
            if (not_duplicating_symbols_counter > 1)
            {
                putc(((not_duplicating_symbols_counter - 2) << 1) + 1, out);
                fseek(in, -not_duplicating_symbols_counter - 1, SEEK_CUR);
                for (int j = 0; j < not_duplicating_symbols_counter - 1; j++)
                {
                    unsigned char hmm = getc(in);
                    putc(hmm, out);
                }
                getc(in);
                getc(in);
                current_carriage_position += not_duplicating_symbols_counter;
            }
            not_duplicating_symbols_counter = 0;
            if (duplicating_symbols_counter < 129)
            {
                if (duplicating_symbols_counter == 0)
                {
                    duplicating_symbols_counter = 1;
                }
                duplicating_symbols_counter++;
            }
            else
            {
                duplicating_symbols_counter = 0;
                putc(127 << 1, out);
                putc(last_character, out);
                current_carriage_position += 2;
            }
        }
        else
        {
            if (duplicating_symbols_counter != 0)
            {
                putc((duplicating_symbols_counter - 2) << 1, out);
                duplicating_symbols_counter = 0;
                putc(last_character, out);
                last_character = temporary;
                not_duplicating_symbols_counter = 1;
                current_carriage_position += 2;
            }
            else
            {
                if (not_duplicating_symbols_counter < 128)
                {
                    if (not_duplicating_symbols_counter == 0)
                    {
                        not_duplicating_symbols_counter++;
                    }
                    not_duplicating_symbols_counter++;
                    last_character = temporary;
                }
                else
                {
                    putc((127 << 1) + 1, out);
                    fseek(in, -129, SEEK_CUR);
                    for (int j = 0; j < not_duplicating_symbols_counter; j++)
                    {
                        putc(getc(in), out);
                    }
                    getc(in);
                    current_carriage_position += 129;
                    last_character = temporary;
                    not_duplicating_symbols_counter = 1;
                }
            }
        }
    }

    if (not_duplicating_symbols_counter != 0)
    {

        putc(((not_duplicating_symbols_counter - 1) << 1) + 1, out);
        fseek(in, -not_duplicating_symbols_counter, SEEK_CUR);
        for (int j = 0; j < not_duplicating_symbols_counter; j++)
        {
            putc(getc(in), out);
        }
        current_carriage_position += not_duplicating_symbols_counter + 1;
    }
    else if (duplicating_symbols_counter != 0)
    {

        putc((duplicating_symbols_counter - 2) << 1, out);
        duplicating_symbols_counter = 0;
        putc(last_character, out);
        not_duplicating_symbols_counter = 1;
        current_carriage_position += 2;
    }
    return current_carriage_position;
}
