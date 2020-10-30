#include<stdlib.h>
#include<stdio.h>

#ifndef HAFFMAN_TREE_RESTORER_H
#define HAFFMAN_TREE_RESTORER_H
typedef struct restored_node {
    char letter;
    struct restored_node *left, *right;
} RestoredNode;

RestoredNode* haffman_tree_restoration(FILE *in);

#endif