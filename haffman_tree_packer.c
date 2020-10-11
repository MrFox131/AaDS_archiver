#include <stdlib.h>
#include <string.h>

#ifndef HAFFMAN_TREE_BUILDER_H
struct node
{
    char is_letter;
    int freq;
    unsigned char letter;
    struct node *left, *right;
};
#endif


//TODO:NO RECURSION
void haffman_tree_packer(struct node* tree, int *reallength, int *buffer_length, unsigned char* buffer){
    // char *buffer = calloc(buffer_length, sizeof(char));
    int initial_reallength = *reallength;
    if (*buffer_length-*reallength<3){
        *buffer_length+=1024;
        buffer = (char*)realloc(buffer, *buffer_length);
    }
    if (tree == NULL) {
        buffer[*reallength] = '0';
        (*reallength)++;
    } else {
        buffer[*reallength] = '1';
        buffer[*reallength+1] = (*tree).letter;
        *reallength+=2;
        if (!(*tree).is_letter){
            haffman_tree_packer((*tree).left, reallength, buffer_length, buffer);
            haffman_tree_packer((*tree).right, reallength, buffer_length, buffer);
        }
    }
    if (initial_reallength==0){
        buffer = realloc(buffer, (*reallength)*sizeof(char));
    }
}