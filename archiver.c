#include <stdio.h>

#include "haffman_tree_builder.h"     
#include "haffman_tree_packer.h" 
#include "stack.h" 

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

int haffman_archivate(FILE *in, FILE* out, int packed_tree_length, unsigned char *packed_tree, Node* haffman_prefix_codes_tree){
    unsigned char *symbol_codes[256], coded_sequence_buffer, coding_symbol_buffer,  mask = 255, filled_by = 0;
    int *codes_length = calloc(256, sizeof(int)), cnt=0;

    codes_generator(haffman_prefix_codes_tree, symbol_codes, codes_length);

    fseek(in, 0, SEEK_SET); 
    packed_tree_length+=4;
    for(int i = 24; i>=0; i-=8){
        fputc(mask&(packed_tree_length>>i), out);
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
    return packed_tree_length+cnt;
}