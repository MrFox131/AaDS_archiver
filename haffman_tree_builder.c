#include <stdio.h>
#include <stdlib.h>

struct node
{
    char is_letter;
    int freq;
    unsigned char letter;
    struct node *left, *right, *parent;
    int suffix_code;
    char *symbol_code, depth;
};

int comparator(const void *a1, const void *b1 ){
    struct node a = **(struct node**)a1, b = **(struct node**)b1;
    return b.freq-a.freq;
}

void find_symbol_code(struct node *root){
    root->symbol_code = calloc(1024, sizeof(char));
    if (root->parent==NULL){
        root->depth=0;
        return;
    }
    if(root->parent->symbol_code==NULL){
        find_symbol_code(root->parent);
    }
    root->symbol_code = strncpy(root->symbol_code, root->parent->symbol_code, root->parent->depth);
    root->symbol_code[root->parent->depth] = root->suffix_code;
    root->depth = root->parent->depth+1;
}

struct node* haffman_tree_builder(FILE *in){
    int freq[256], n=0;
    unsigned char temp;
    struct node *tree = (struct node*)calloc(1024, sizeof(struct node));
    struct node **sorting_tree = (struct node**)calloc(1024, sizeof(struct node*));
    for(int i=0; i<256; i++){
        freq[i]=0;
    }
    fseek(in, 0, SEEK_END);
    int end = ftell(in);
    fseek(in, 0, SEEK_SET);
    while(ftell(in)!=end){
        temp = fgetc(in);
        if(temp!=255){
            freq[temp]++;
        }
    }
    #ifdef _DEBUG
        for (int i=0; i<256; i++){
            printf("%d ", freq[i]);
        }
    #endif
    for(int i=0; i<256; i++){
        if (freq[i]){
            tree[n].is_letter = 1;
            tree[n].letter=i;
            tree[n].freq=freq[i];
            tree[n].left = NULL;
            tree[n].right = NULL;
            sorting_tree[n] = &tree[n];
            n++;
        }
    }
    qsort(sorting_tree, n, sizeof(struct node*), comparator);
    int k = n;
    sorting_tree = ( struct node **)realloc(sorting_tree, n*sizeof(struct node*));
    n--;
    while(n>0){
        struct node temp;
        temp.symbol_code = NULL;
        temp.parent=NULL;
        temp.letter=0;
        temp.left=sorting_tree[n-1];
        sorting_tree[n-1]->suffix_code=1;
        temp.right=sorting_tree[n];
        sorting_tree[n]->suffix_code=2;
        temp.freq = (*(temp.left)).freq+(*(temp.right)).freq;
        temp.is_letter=0;
        tree[k]=temp;
        sorting_tree[n]->parent = &tree[k];
        sorting_tree[n-1]->parent = &tree[k];
        sorting_tree[n-1] = &tree[k];
        k++;
        n--;
        qsort(sorting_tree, n+1, sizeof(struct node*), comparator);
    }
    tree = ( struct node *)realloc(tree, k*sizeof(struct node));
    return sorting_tree[0];
}   