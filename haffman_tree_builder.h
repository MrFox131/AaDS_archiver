#ifndef HAFFMAN_TREE_BUILDER_H
#define HAFFMAN_TREE_BUILDER_H

struct node
{
    char is_letter;
    int freq;
    unsigned char letter;
    struct node *left, *right, *parent;
    int suffix_code;
    char *symbol_code, depth;
};

struct node* haffman_tree_builder(FILE *in);
#endif