#include <stdlib.h>
#include <string.h>

#ifndef HAFFMAN_TREE_BUILDER_H
struct node
{
    char is_letter;
    int freq;
    unsigned char letter;
    struct node *left, *right, *parent;
    int suffix_code;
    char *symbol_code, depth;
};
#endif

void haffman_tree_packer(struct node* tree, int *real_length, int *buffer_length, unsigned char* buffer){
    int initial_real_length = *real_length;
    if (*buffer_length-*real_length<3){
        *buffer_length+=1024;
        buffer = (char*)realloc(buffer, *buffer_length);
    }
    if (tree == NULL) {
        buffer[*real_length] = '0';
        (*real_length)++;
    } else {
        buffer[*real_length] = '1';
        buffer[*real_length+1] = (*tree).letter;
        *real_length+=2;
        if (!(*tree).is_letter){
            haffman_tree_packer((*tree).left, real_length, buffer_length, buffer);
            haffman_tree_packer((*tree).right, real_length, buffer_length, buffer);
        }
    }
    if (initial_real_length==0){
        buffer = realloc(buffer, (*real_length)*sizeof(char));
    }
}