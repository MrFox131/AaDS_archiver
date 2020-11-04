#include <stdio.h>
#include <stdlib.h>

typedef struct  restored_node{
    char letter;
    struct restored_node *left, *right;
} RestoredNode;

RestoredNode* haffman_tree_restoration(FILE *in){
    if(!getc(in)){
        return NULL;
    }
    RestoredNode* node = malloc(sizeof(RestoredNode));
    if (getc(in)){
        node->letter = getc(in);
        node->left = NULL;
        node->right = NULL;
    } else {
        node->left = haffman_tree_restoration(in);
        node->right = haffman_tree_restoration(in);
    }
    return node;
}