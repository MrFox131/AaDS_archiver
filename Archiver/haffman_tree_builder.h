#ifndef HAFFMAN_TREE_BUILDER_H
#define HAFFMAN_TREE_BUILDER_H
#include <stdio.h>
typedef struct node
{
    char is_letter;
    int freq;
    unsigned char letter;
    struct node *left, *right, *parent;
    int last_symbol_code;
    char *symbol_code, depth;
} Node;

Node *haffman_tree_builder(FILE *in, Node **zero_node);
void find_symbol_code(Node *root);

#endif
