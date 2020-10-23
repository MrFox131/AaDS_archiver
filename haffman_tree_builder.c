#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int ALPHABET_SIZE = 256;

typedef struct node{
    char is_letter;
    int freq;
    unsigned char letter;
    struct node *left, *right, *parent;
    int last_symbol_code;
    char *symbol_code, depth;
} Node;

Node new_node(int is_letter, int freq, unsigned char letter, Node* left_child, Node* right_child){
    Node node;
    node.is_letter = is_letter;
    node.letter = letter;
    if(is_letter)
        node.freq = freq;
    else {
        node.freq = (left_child==NULL?0:left_child->freq)+(right_child==NULL?0:right_child->freq);
    }
    node.left = left_child;
    node.right = right_child;
    node.symbol_code = NULL;
    node.parent = NULL;
    return node;
}

int comparator(const void *a1, const void *b1 ){
    Node a = **(Node**)a1, b = **(Node**)b1;
    return b.freq-a.freq;
}

void frequency_counter(int *freq, FILE *in){
    unsigned char temp;

    for(int i=0; i<ALPHABET_SIZE; i++){
        freq[i]=0;
    }
    fseek(in, 0, SEEK_END);
    int end = ftell(in);
    fseek(in, 0, SEEK_SET);

    while(ftell(in)!=end){
        temp = fgetc(in);
        freq[temp]++;
    }

    #ifdef _DEBUG
        for (int i=0; i<ALPHABET_SIZE; i++){
            printf("%d ", freq[i]);
        }
    #endif
}

void find_symbol_code(Node *root){
    //root->symbol_code = (char*)calloc(1024, sizeof(char));
    root->symbol_code = (char*)malloc(256*sizeof(char));
    if(root->symbol_code==NULL){
        printf("Memory error: not allocated memory for root->symbol_code");
    }
    if (root->parent==NULL){
        root->depth=0;
        return;
    }
    if(root->parent->symbol_code==NULL){
        find_symbol_code(root->parent);
    }
    root->symbol_code = strncpy(root->symbol_code, root->parent->symbol_code, root->parent->depth);
    root->symbol_code[root->parent->depth] = root->last_symbol_code;
    root->depth = root->parent->depth+1;
    root->symbol_code = (char*)realloc(root->symbol_code, sizeof(char)*(root->depth));
}

Node* haffman_tree_builder(FILE *in){
    int freq[ALPHABET_SIZE], n=0;
    Node *tree = (Node*)calloc(1024, sizeof(Node)); //place for tree nodes
    Node **sorting_tree = (Node**)calloc(1024, sizeof(Node*)); //pointers array for nodes sorting
    
    frequency_counter(freq, in);

    for(int i=0; i<ALPHABET_SIZE; i++){
        if (freq[i]){
            tree[n]=new_node(1, freq[i], i, NULL, NULL);
            sorting_tree[n] = &tree[n];
            n++;
        }
    }
    qsort(sorting_tree, n, sizeof(Node*), comparator);
    int k = n;
    sorting_tree = ( Node **)realloc(sorting_tree, n*sizeof(Node*));
    n--;
    while(n>0){
        Node temp = new_node(0, 0, 0, sorting_tree[n-1], sorting_tree[n]);
        temp.left->last_symbol_code=1;
        temp.right->last_symbol_code=2;
        tree[k]=temp;
        temp.left->parent = &tree[k];
        temp.right->parent = &tree[k];
        sorting_tree[n-1] = &tree[k];
        k++;
        n--;
        qsort(sorting_tree, n+1, sizeof(Node*), comparator);
    }
    tree = ( Node *)realloc(tree, (k+1)*sizeof(Node));
    return sorting_tree[0];
}   