#include <stdio.h>
#include<stdlib.h>
#include "pack.h"
#include "unpack.h"
#include "haffman_tree_builder.h"       

int main(){
    FILE *in, *out;
    in = fopen("input.txt", "r");
    struct node noda = haffman_tree_builder(in);
    return 0;
}