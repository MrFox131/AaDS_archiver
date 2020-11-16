#include <stdio.h>

#include "haffman_tree_builder.h"     
#include "haffman_tree_packer.h" 
#include "stack.h" 
#include "get_directory_structure.h"
#include <string.h>

#define STRUCTURE_BUFFER_MULTIPLIER 1024

typedef struct node Node;

//generates code for all symbols in tree, memorizing symbol_codes not only for letters, but for non-leaf nodes too
int codes_generator(Node* root, unsigned char *codes[256], int *codes_length){
    Stack* stack = create_stack();
    push(stack, root->left);
    push(stack, root->right);
    while (!is_empty(stack)){
        root = pop(stack);
        find_symbol_code(root);
        if (root->is_letter){
            codes[root->letter] = root->symbol_code;
            codes_length[root->letter]=root->depth;
            continue;
        }
        push(stack, root->left);
        push(stack, root->right);
    }
    stack_destructor(stack);
}

int haffman_archivate(FILE *in, FILE* out){
    Node* haffman_prefix_codes_tree = haffman_tree_builder(in);
    unsigned char *packed_tree = calloc(1024, sizeof(unsigned char));
    int packed_tree_length = 0, buffer_length = 1024;
    haffman_tree_packer(haffman_prefix_codes_tree, &packed_tree_length, &buffer_length, &packed_tree);

    unsigned char *symbol_codes[256], coded_sequence_buffer, coding_symbol_buffer,  mask = 255, filled_by = 0;
    int *codes_length = calloc(256, sizeof(int)), cnt=0;

    codes_generator(haffman_prefix_codes_tree, symbol_codes, codes_length);

    fseek(in, 0, SEEK_SET); 
    for (int i=0; i<packed_tree_length; i++){
        fputc(packed_tree[i], out);
    }
    
    fseek(in, 0, SEEK_END);
    int end = ftell(in);
    fseek(in, 0, SEEK_SET);

    while(ftell(in)!=end){
        coding_symbol_buffer = fgetc(in);
        for(int i=0; i<codes_length[coding_symbol_buffer]; i++){
            if(filled_by==8) {
                filled_by = 0;
                fputc(coded_sequence_buffer, out);
                coded_sequence_buffer = 0;
                cnt++;
            }
            coded_sequence_buffer = coded_sequence_buffer << 1;
            coded_sequence_buffer = coded_sequence_buffer|(symbol_codes[coding_symbol_buffer][i] >> 1);
            filled_by++; 
        }
    }
    
    if (filled_by!=0){
        fputc(coded_sequence_buffer, out);
        cnt++;
    }
    fputc(filled_by==0?8:filled_by, out);
    cnt++;

    free(packed_tree);
    //free(haffman_prefix_codes_tree);
    // for(int i=0; i<256; i++){
    //     free(symbol_codes[i]);
    // }
    free(codes_length);
    return packed_tree_length+cnt;
}

int archivate(char* archivating_file_name, char* archived_file_name){
    
    char* cleared_path = calloc(strlen(archivating_file_name)+1, sizeof(char));
    strcpy(cleared_path, archivating_file_name);
    if(cleared_path[strlen(cleared_path)-1]=='\\' || cleared_path[strlen(cleared_path)-1]=='/')
        cleared_path[strlen(cleared_path)-1]='\0';
    Directory* dir = get_directory_structure(cleared_path);
    
    
    unsigned char* structure_buffer = calloc(STRUCTURE_BUFFER_MULTIPLIER, sizeof(char));
    int structure_buffer_size = STRUCTURE_BUFFER_MULTIPLIER;
    int current_carriage_pos=0;
    int base_path_length = strlen(dir->base);
    FILE *out_file = fopen(archived_file_name, "wb");
    if(out_file==NULL){
        return -1;
    }

    for(int i=0; i<dir->files_count; i++){
        int filename_length = strlen(dir->files[i]) - base_path_length;
        while(structure_buffer_size-current_carriage_pos < filename_length + 9){
            structure_buffer = realloc(structure_buffer, structure_buffer_size+STRUCTURE_BUFFER_MULTIPLIER);
            structure_buffer_size += STRUCTURE_BUFFER_MULTIPLIER;
        }
        structure_buffer[current_carriage_pos] = filename_length>>24 & (unsigned char)255; 
        structure_buffer[current_carriage_pos+1] = filename_length>>16 & (unsigned char)255;
        structure_buffer[current_carriage_pos+2] = filename_length>>8 & (unsigned char)255;
        structure_buffer[current_carriage_pos+3] = filename_length & (unsigned char)255;
        strncpy(structure_buffer+current_carriage_pos+4, dir->files[i]+base_path_length, filename_length);
        current_carriage_pos += 4 + filename_length;
        FILE *archivating_file = fopen(dir->files[i], "rb");
        int compressed_file_size = haffman_archivate(archivating_file, out_file);
        fclose(archivating_file);
        structure_buffer[current_carriage_pos] = (unsigned char)255 & (compressed_file_size>>24); 
        structure_buffer[current_carriage_pos+1] = (unsigned char)255 & compressed_file_size>>16;
        structure_buffer[current_carriage_pos+2] = (unsigned char)255 & compressed_file_size>>8;
        structure_buffer[current_carriage_pos+3] = (unsigned char)255 & compressed_file_size;
        structure_buffer[current_carriage_pos+4] = 1;
        current_carriage_pos+=5;
    }
    if(structure_buffer_size-current_carriage_pos<4){
        structure_buffer = realloc(structure_buffer, (4-(structure_buffer_size-current_carriage_pos))*sizeof(char));
    }
    structure_buffer[current_carriage_pos] = (unsigned char)255 & (current_carriage_pos+4)>>24; 
    structure_buffer[current_carriage_pos+1] = (unsigned char)255 & (current_carriage_pos+4)>>16;
    structure_buffer[current_carriage_pos+2] = (unsigned char)255 & (current_carriage_pos+4)>>8;
    structure_buffer[current_carriage_pos+3] = (unsigned char)255 & (current_carriage_pos+4);
    current_carriage_pos+=4;
    for(int i=0; i<current_carriage_pos; i++){
        putc(structure_buffer[i], out_file);
    }

    fclose(out_file);
    free(structure_buffer);
    free(cleared_path);
    destruct_dir(dir);
    return 0;
}