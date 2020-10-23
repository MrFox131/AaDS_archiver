#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "haffman_tree_builder.h"

void haffman_tree_packer(struct node* tree, int *real_length, int *buffer_length, unsigned char* buffer){
    if (*buffer_length-*real_length<3){
        *buffer_length+=1024;
        buffer = (char*)realloc(buffer, *buffer_length);
    }
    if (tree == NULL) {
        buffer[*real_length] = '0';
        (*real_length)++;
    } else {
        buffer[*real_length] = '1';
        if (tree->is_letter){
            buffer[*real_length+1]=1;
            buffer[*real_length+2] = tree->letter;
            *real_length+=3;
        } else {
            buffer[*real_length+1]=0;
            *real_length+=2;
        }
        if (tree->is_letter){
            haffman_tree_packer(tree->left, real_length, buffer_length, buffer);
            haffman_tree_packer(tree->right, real_length, buffer_length, buffer);
        }
    }
    if (tree->parent==NULL){
        buffer = realloc(buffer, *real_length * sizeof(char));
    }
}