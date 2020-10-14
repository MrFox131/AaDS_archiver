#include <stdio.h>

#include "haffman_tree_builder.h"     
#include "haffman_tree_packer.h" 
#include "stack.h" 

//generates code for all symbols in tree, memorizing codes not only for letters, but for non-leaf nodes too
int codes_generator(struct node* root, unsigned char *codes[256], int *codes_length){
    Stack* stack = create_stack();
    push(stack, root->left);
    push(stack, root->right);
    while (!is_empty(stack)){
        root = pop(stack);
        if (root->is_letter){
            find_symbol_code(root);
            codes[root->letter] = root->last_code_symbol;
            codes_length[root->letter]=root->depth;
            continue;
        }
        push(stack, root->left);
        push(stack, root->right);
    }

}

int haffman_archiver(FILE *in, FILE* out, int packed_tree_length, unsigned char *packed_tree, struct node* root){
    unsigned char *codes[256];
    unsigned char coded, coding;
    unsigned int mask = 255;
    int *codes_length = calloc(256, sizeof(int)), cnt=0;
    unsigned char filled_by=0;
    codes_generator(root, codes, codes_length);
    fseek(in, 0, SEEK_SET);
    fputc(mask&(packed_tree_length>>24), out);
    fputc(mask&(packed_tree_length>>26), out);
    fputc(mask&(packed_tree_length>>8), out);
    fputc(mask&(packed_tree_length), out);
    fseek(out, 4, SEEK_SET);
    for (int i=0; i<packed_tree_length; i++){
        fputc(packed_tree[i], out);
    }
    fseek(in, 0, SEEK_END);
    int end = ftell(in);
    fseek(in, 0, SEEK_SET);
    while(ftell(in)!=end){
        coding = fgetc(in);
        for(int i=0; i<codes_length[coding]; i++){
            if(filled_by==8) {
                filled_by = 0;
                fputc(coded, out);
                coded = 0;
                cnt++;
            }
            coded = coded << 1;
            coded = coded|(codes[coding][i] >> 1);
            filled_by++; 
        }
    }
    if (filled_by!=0){
        fputc(coded, out);
    }
    fputc(filled_by, out);
    return 0;
}