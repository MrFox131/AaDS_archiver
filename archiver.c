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

    unsigned char *symbol_codes[256], coded_sequence_buffer, coding_symbol_buffer, mask = 255, filled_by = 0;
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

    free(packed_tree);
    free(zero_node);
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
        if(archivating_function==1){
            compressed_file_size = haffman_archivate(archivating_file, out_file);
        } else {
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

// typedef struct word
// {
//     unsigned int index;
//     unsigned char symbol;
// } Word;

// int LZ78(unsigned char **buffer, int buffer_length)
// {
//     unsigned char **LZ78_buffer = malloc(sizeof(char *));
//     *LZ78_buffer = calloc(ARCHIVATING_BUFFER_MULTIPLIER, sizeof(char));
//     int current_carriage_position = 0;
//     int LZ78_buffer_length = ARCHIVATING_BUFFER_MULTIPLIER;
//     Word *dictionary = (Word *)calloc(200000, sizeof(Word));
//     int count_of_prefixes = 0;
//     int current_prefix = 0;
//     for (int i = 0; i < buffer_length; i++)
//     {
//         if (i % 10000 == 0)
//             printf("%d|||%d\n", i, count_of_prefixes);
//         int j = 0;
//         for (; j < count_of_prefixes; j++)
//         {
//             if (current_prefix == dictionary[j].index && (*buffer)[i] == dictionary[j].symbol)
//             {
//                 break;
//             }
//         }
//         if (j == count_of_prefixes)
//         {
//             if (count_of_prefixes < 50000)
//             {
//                 dictionary[count_of_prefixes].index = current_prefix;
//                 dictionary[count_of_prefixes].symbol = (*buffer)[i];
//                 //current_prefix = 0;
//                 count_of_prefixes++;
//             }
//             if (LZ78_buffer_length - current_carriage_position < 5)
//             {
//                 *LZ78_buffer = realloc(*LZ78_buffer, LZ78_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER);
//                 LZ78_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
//             }
//             add_uinteger_to_buffer(current_prefix, LZ78_buffer, current_carriage_position);
//             current_carriage_position += 4;
//             (*LZ78_buffer)[current_carriage_position++] = (*buffer)[i];
//             current_prefix = 0;
//         }
//         else
//         {
//             current_prefix = j + 1;
//         }
//     }
//     if (current_prefix != 0)
//     {
//         if (count_of_prefixes < 50000)
//         {
//             dictionary[count_of_prefixes].index = current_prefix;
//             dictionary[count_of_prefixes].symbol = '\0';
//             current_prefix = 0;
//             count_of_prefixes++;
//         }
//         if (LZ78_buffer_length - current_carriage_position < 5)
//         {
//             *LZ78_buffer = realloc(*LZ78_buffer, LZ78_buffer_length + ARCHIVATING_BUFFER_MULTIPLIER);
//             LZ78_buffer_length += ARCHIVATING_BUFFER_MULTIPLIER;
//         }
//         add_uinteger_to_buffer(current_prefix, LZ78_buffer, current_carriage_position);
//         current_carriage_position += 4;
//         (*LZ78_buffer)[current_carriage_position++] = '\0';
//     }
//     free(*buffer);
//     *buffer = *LZ78_buffer;
//     free(LZ78_buffer);
//     free(dictionary);
//     return current_carriage_position;
// }

// int LZ78_restoration(unsigned char **buffer, int buffer_length)
// {
//     char **dictionary = (char **)calloc(200001, sizeof(char *));
//     dictionary[0] = malloc(sizeof(char));
//     dictionary[0][0] = '\0';
//     unsigned char **Restored_buffer = malloc(sizeof(char *));
//     *Restored_buffer = calloc(ARCHIVATING_BUFFER_MULTIPLIER, sizeof(char));
//     int Restored_buffer_size = ARCHIVATING_BUFFER_MULTIPLIER;
//     int Restored_buffer_pos = 0;
//     int prefixes_count = 1;
//     int current_carriage_position = 0;
//     while (current_carriage_position < buffer_length)
//     {
//         unsigned int temporary = 0;
//         temporary = ((temporary >> 24) | (*buffer)[current_carriage_position]) << 24;
//         temporary = ((temporary >> 16) | (*buffer)[current_carriage_position + 1]) << 16;
//         temporary = ((temporary >> 8) | (*buffer)[current_carriage_position + 2]) << 8;
//         temporary = ((temporary) | (*buffer)[current_carriage_position + 3]);
//         if (prefixes_count < 50001)
//         {
//             int prev_len = strlen(dictionary[temporary]);
//             dictionary[prefixes_count] = calloc(prev_len + 2, sizeof(char));
//             strcpy(dictionary[prefixes_count], dictionary[temporary]);
//             dictionary[prefixes_count][prev_len] = (*buffer)[current_carriage_position + 4];
//             dictionary[prefixes_count][prev_len + 1] = '\0';
//             prefixes_count++;
//         }
//         while (Restored_buffer_size - Restored_buffer_pos < strlen(dictionary[temporary]) + 1)
//         {
//             (*Restored_buffer) = realloc(*Restored_buffer, Restored_buffer_size + ARCHIVATING_BUFFER_MULTIPLIER);
//             Restored_buffer_size += ARCHIVATING_BUFFER_MULTIPLIER;
//         }
//         memcpy((*Restored_buffer) + Restored_buffer_pos, dictionary[temporary], strlen(dictionary[temporary]));
//         (*Restored_buffer)[Restored_buffer_pos + strlen(dictionary[temporary])] = (*buffer)[current_carriage_position + 4];
//         Restored_buffer_pos += strlen(dictionary[temporary]) + 1;
//         current_carriage_position += 5;
//     }
//     free(*buffer);
//     *buffer = *Restored_buffer;
//     free(Restored_buffer);
//     return Restored_buffer_pos;
// }