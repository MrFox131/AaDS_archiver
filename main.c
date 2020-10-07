#include <stdio.h>
#include<stdlib.h>
#include "pack.h"
#include "unpack.h"
#include "haffman_tree_builder.h"

int main(){
    // char* name=calloc(8, sizeof(char));
    // name = "input.txt";
    FILE *in, *out;
    in = fopen("input.txt", "r");
    // out = fopen("output.txt", "w");
    // pack(name, in, out);
    // fclose(out);
    // fclose(in);
    // FILE *out = fopen("output.txt", "r");
    // char path[512]="";
    // unpack(out, path);
   
    struct node noda =  haffman_tree_builder(in);
    return 0;
}