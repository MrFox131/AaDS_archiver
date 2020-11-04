
#include <stdio.h>
#include <stdlib.h>
#include "haffman_tree_restorer.h"

int haffman_unarchivate(FILE *in, FILE *out, int offset, int size){
    fseek(in, offset, SEEK_SET);
    RestoredNode* const tree = haffman_tree_restoration(in);
    size-=ftell(in) - offset;
    char buffer;
    RestoredNode *curr_node = tree;
    for (int i=0; i < size-2; i++){
        buffer = getc(in);
        for (int j=7; j>=0; j--){
            if((buffer>>j)%2){
                curr_node = curr_node->right;
            } else {
                curr_node = curr_node->left;
            }
            if(curr_node->right==NULL && curr_node->left==NULL){
                putc(curr_node->letter, out);
                curr_node = tree;
            }
        }
    }
    buffer = getc(in);
    char last_byte_significant_bits_count = getc(in);
            for (int j=last_byte_significant_bits_count-1; j>=0; j--){
            if((buffer>>j)%2){
                curr_node = curr_node->right;
            } else {
                curr_node = curr_node->left;
            }
            if(curr_node->right==NULL && curr_node->left==NULL){
                putc(curr_node->letter, out);
                curr_node = tree;
            }
    }
    fclose(out);
    return 0;
}