#include <stdio.h>
#include <stdlib.h>

struct node
{
    char is_letter;
    int freq;
    unsigned char letter;
    struct node *left, *right;
};

int comparator(const void *a1, const void *b1 ){
    struct node a = **(struct node**)a1, b = **(struct node**)b1;
    return b.freq-a.freq;
}

struct node* haffman_tree_builder(FILE *in){
    int freq[256], n=0;
    unsigned char temp;
    struct node *tree = (struct node*)calloc(1024, sizeof(struct node));
    struct node **sorting_tree = (struct node**)calloc(1024, sizeof(struct node*));
    for(int i=0; i<256; i++){
        freq[i]=0;
    }
    while(!feof(in)){
        temp = fgetc(in);
        if(temp!=255){
            freq[temp]++;
        }
    }
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
        temp.letter=0;
        temp.left=sorting_tree[n-1];
        temp.right=sorting_tree[n];
        temp.freq = (*(temp.left)).freq+(*(temp.right)).freq;
        temp.is_letter=0;
        tree[k]=temp;
        sorting_tree[n-1] = &tree[k];
        k++;
        n--;
        qsort(sorting_tree, n+1, sizeof(struct node*), comparator);
    }
    tree = ( struct node *)realloc(tree, k*sizeof(struct node));
    return sorting_tree[0];
}   