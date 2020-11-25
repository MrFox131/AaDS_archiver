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

int RLE(char **buffer, int buffer_length);

int LZ78(char **buffer, int buffer_length)
{
    return 0;
}

unsigned int haffman_archivate(FILE *in, FILE *out, char extended_pipeline)
{
    Node *zero_node;
    Node *haffman_prefix_codes_tree = haffman_tree_builder(in, &zero_node);
    unsigned char *packed_tree = calloc(1024, sizeof(unsigned char));
    int packed_tree_length = 0, buffer_length = 1024;
    haffman_tree_packer(haffman_prefix_codes_tree, &packed_tree_length, &buffer_length, &packed_tree);

    unsigned char *symbol_codes[256], coded_sequence_buffer, coding_symbol_buffer, mask = 255, filled_by = 0;
    int *codes_length = calloc(256, sizeof(int)), cnt = 0;

    fseek(in, 0, SEEK_END);
    int end = ftell(in);
    fseek(in, 0, SEEK_SET);

    codes_generator(haffman_prefix_codes_tree, symbol_codes, codes_length);
    if (!extended_pipeline)
    {
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

        free(packed_tree);
        free(zero_node);
        free(codes_length);
        return packed_tree_length + cnt;
    }
    else
    {
        char *buffer = (char *)calloc(ARCHIVATING_BUFFER_MULTIPLIER, sizeof(char));
        int buffer_length = ARCHIVATING_BUFFER_MULTIPLIER;
        int current_carriage_position = 0;
        while (buffer_length < packed_tree_length)
        {
            buffer = realloc(buffer, (buffer_length + ARCHIVATING_BUFFER_MULTIPLIER) * sizeof(char));
            buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
        }
        for (int i = 0; i < packed_tree_length; i++)
        {
            buffer[current_carriage_position] = packed_tree[i];
            current_carriage_position++;
        }

        while (ftell(in) != end)
        {
            coding_symbol_buffer = fgetc(in);
            for (int i = 0; i < codes_length[coding_symbol_buffer]; i++)
            {
                if (filled_by == 8)
                {
                    if (current_carriage_position == buffer_length)
                    {
                        buffer = realloc(buffer, (buffer_length + ARCHIVATING_BUFFER_MULTIPLIER) * sizeof(char));
                        buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
                    }
                    filled_by = 0;
                    buffer[current_carriage_position] = coded_sequence_buffer;
                    current_carriage_position++;
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
            buffer[current_carriage_position] = coded_sequence_buffer;
            cnt++;
            current_carriage_position++;
            if (current_carriage_position == buffer_length)
            {
                buffer = realloc(buffer, (buffer_length + ARCHIVATING_BUFFER_MULTIPLIER) * sizeof(char));
                buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
            }
        }
        buffer[current_carriage_position] = filled_by == 0 ? 8 : filled_by;
        cnt++;
        current_carriage_position++;
        buffer = realloc(buffer, current_carriage_position * sizeof(char));
        buffer_length = current_carriage_position;
        buffer_length = RLE(&buffer, buffer_length);
        buffer_length = LZ78(&buffer, buffer_length);
        for (int i = 0; i < buffer_length; i++)
        {
            putc(buffer[i], out);
        }
        return buffer_length;
    }
}

int archivate(char *archivating_file_name, char *archived_file_name, char extended_pipeline)
{

    char *cleared_path = calloc(strlen(archivating_file_name) + 1, sizeof(char));
    strcpy(cleared_path, archivating_file_name);
    if (cleared_path[strlen(cleared_path) - 1] == '\\' || cleared_path[strlen(cleared_path) - 1] == '/')
        cleared_path[strlen(cleared_path) - 1] = '\0';
    Directory *dir = get_directory_structure(cleared_path);

    unsigned char *structure_buffer = calloc(STRUCTURE_BUFFER_MULTIPLIER, sizeof(char));
    int structure_buffer_size = STRUCTURE_BUFFER_MULTIPLIER;
    unsigned int current_carriage_pos = 0;
    int base_path_length = strlen(dir->base);
    FILE *out_file = fopen(archived_file_name, "wb");
    if (out_file == NULL)
    {
        return -1;
    }

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
        unsigned int compressed_file_size = haffman_archivate(archivating_file, out_file, extended_pipeline);
        fclose(archivating_file);
        add_uinteger_to_buffer(compressed_file_size, &structure_buffer, current_carriage_pos);

        structure_buffer[current_carriage_pos + 4] = 1;
        current_carriage_pos += 5;
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

int RLE(char **buffer, int buffer_length)
{
    unsigned char **RLE_buffer;
    RLE_buffer = malloc(sizeof(char *));
    *RLE_buffer = calloc(ARCHIVATING_BUFFER_MULTIPLIER, sizeof(char));
    int RLE_buffer_length = ARCHIVATING_BUFFER_MULTIPLIER;
    unsigned char last_character = (*buffer)[0];
    unsigned char not_duplicating_symbols_counter = 0, duplicating_symbols_counter = 0;
    int current_carriage_position = 0;
    for (int i = 1; i < buffer_length; i++)
    {

        if ((*buffer)[i] == last_character)
        {
            if (not_duplicating_symbols_counter > 1)
            {

                if (RLE_buffer_length - current_carriage_position < not_duplicating_symbols_counter)
                {
                    *RLE_buffer = realloc(RLE_buffer, sizeof(char) * (RLE_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER));
                    RLE_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
                }
                (*RLE_buffer)[current_carriage_position++] = ((not_duplicating_symbols_counter - 2) << 1) + 1;
                for (int j = 0; j < not_duplicating_symbols_counter - 1; j++)
                {
                    (*RLE_buffer)[j + current_carriage_position] = (*buffer)[i - not_duplicating_symbols_counter + j];
                }
                current_carriage_position += not_duplicating_symbols_counter - 1;
            }
            not_duplicating_symbols_counter = 0;
            if (duplicating_symbols_counter < 129)
            {
                if (duplicating_symbols_counter == 0)
                {
                    duplicating_symbols_counter = 1;
                }
                duplicating_symbols_counter++;
                continue;
            }
            else
            {
                duplicating_symbols_counter = 0;
                if (RLE_buffer_length - current_carriage_position < 2)
                {
                    *RLE_buffer = realloc(RLE_buffer, sizeof(char) * (RLE_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER));
                    RLE_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
                }
                (*RLE_buffer)[current_carriage_position] = 127 << 1;
                (*RLE_buffer)[current_carriage_position + 1] = last_character;
                current_carriage_position += 2;
            }
        }
        else
        {
            if (duplicating_symbols_counter != 0)
            {
                if (RLE_buffer_length - current_carriage_position < 2)
                {
                    *RLE_buffer = realloc(RLE_buffer, sizeof(char) * (RLE_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER));
                    RLE_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
                }
                (*RLE_buffer)[current_carriage_position] = (duplicating_symbols_counter - 2) << 1;
                duplicating_symbols_counter = 0;
                (*RLE_buffer)[current_carriage_position + 1] = last_character;
                last_character = (*buffer)[i];
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
                    last_character = (*buffer)[i];
                }
                else
                {
                    (*RLE_buffer)[current_carriage_position++] = (127 << 1) + 1;
                    for (int j = 0; j < not_duplicating_symbols_counter; j++)
                    {
                        (*RLE_buffer)[j + current_carriage_position] = (*buffer)[i - not_duplicating_symbols_counter + j];
                    }
                    current_carriage_position += 128;
                    last_character = (*buffer)[i];
                    not_duplicating_symbols_counter = 0;
                }
            }
        }
    }

    if (not_duplicating_symbols_counter != 0)
    {
        if (RLE_buffer_length - current_carriage_position < not_duplicating_symbols_counter)
        {
            *RLE_buffer = realloc(RLE_buffer, sizeof(char) * (RLE_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER));
            RLE_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
        }
        (*RLE_buffer)[current_carriage_position++] = ((not_duplicating_symbols_counter - 1) << 1) + 1;
        for (int j = 0; j < not_duplicating_symbols_counter; j++)
        {
            (*RLE_buffer)[j + current_carriage_position] = (*buffer)[buffer_length - not_duplicating_symbols_counter + j];
        }
        current_carriage_position += not_duplicating_symbols_counter;
    }
    else if (duplicating_symbols_counter != 0)
    {
        if (RLE_buffer_length - current_carriage_position < 2)
        {
            *RLE_buffer = realloc(RLE_buffer, sizeof(char) * (RLE_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER));
            RLE_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
        }
        (*RLE_buffer)[current_carriage_position] = (duplicating_symbols_counter - 1) << 1;
        duplicating_symbols_counter = 0;
        (*RLE_buffer)[current_carriage_position + 1] = last_character;
        not_duplicating_symbols_counter = 1;
        current_carriage_position += 2;
    }
    free(*buffer);
    *buffer = *RLE_buffer;
    free(RLE_buffer);
    return current_carriage_position;
}

int RLE_restoration(unsigned char **buffer, int buffer_length)
{
    unsigned char **Restored_buffer = malloc(sizeof(char *));
    *Restored_buffer = calloc(ARCHIVATING_BUFFER_MULTIPLIER, sizeof(char));
    int Restored_buffer_length = ARCHIVATING_BUFFER_MULTIPLIER;
    int buffer_carriage_position = 0;
    int Restored_buffer_carriage_position = 0;
    while (buffer_carriage_position < buffer_length)
    {
        if ((*buffer)[buffer_carriage_position] % 2 == 0)
        {
            int duplicating_sequence_length = ((*buffer)[buffer_carriage_position] >> 1) + 2;
            if (Restored_buffer_length - Restored_buffer_carriage_position < duplicating_sequence_length)
            {
                *Restored_buffer = realloc(*Restored_buffer, Restored_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER);
                Restored_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
            }
            for (int i = 0; i < duplicating_sequence_length; i++)
            {
                (*Restored_buffer)[Restored_buffer_carriage_position + i] = (*buffer)[buffer_carriage_position + 1];
            }
            buffer_carriage_position+=2;
            Restored_buffer_carriage_position+=duplicating_sequence_length;
        } else {
            int not_duplicating_sequence_length = ((*buffer)[buffer_carriage_position]>>1)+1;
            if (Restored_buffer_length - Restored_buffer_carriage_position < not_duplicating_sequence_length)
            {
                *Restored_buffer = realloc(*Restored_buffer, Restored_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER);
                Restored_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
            }
            for(int i=0; i< not_duplicating_sequence_length; i++){
                (*Restored_buffer)[Restored_buffer_carriage_position+i] = (*buffer)[buffer_carriage_position+1+i];
            }
            buffer_carriage_position+=not_duplicating_sequence_length+1;
            Restored_buffer_carriage_position += not_duplicating_sequence_length;
        }
    }
    free(*buffer);
    *buffer = *Restored_buffer;
    free(Restored_buffer);
    return Restored_buffer_carriage_position;
}