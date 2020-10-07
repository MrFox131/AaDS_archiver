#include <stdio.h>
#include <stdlib.h>

struct node
{
    int is_letter, freq;
    char letter;
    struct node *left, *right;
};

int comparator(const void *a1, const void *b1 ){
    struct node a = **(struct node**)a1, b = **(struct node**)b1;
    return b.freq-a.freq;
}

struct node haffman_tree_builder(FILE *in){
    int freq[256], n=0;
    char temp;
    struct node *tree = calloc(1024, sizeof(struct node));
    struct node **sorting_tree = calloc(1024, sizeof(struct node*));
    for(int i=0; i<256; i++){
        freq[i]=0;
    }
    while(!feof(in)){
        temp = fgetc(in);
        freq[temp]++;
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
    n--;
    while(n>0){
        struct node temp;
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
    return *sorting_tree[0];
}   