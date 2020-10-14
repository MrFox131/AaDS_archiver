#include <stdio.h>
#include <stdlib.h>
#include "pack.h"
#include "unpack.h"
#include "haffman_tree_builder.h"     
#include "haffman_tree_packer.h"  
#include "archiver.c"

int main(){
    FILE *in, *out;
    in = fopen("main - Copy.exe", "rb");
    out = fopen("output.txt", "w");
    struct node* noda = haffman_tree_builder(in);
    unsigned char *buffer = calloc(1024, sizeof(char));
    int real_length = 0, buffer_length = 1024;
    haffman_tree_packer(noda, &real_length, &buffer_length, buffer);
    haffman_archiver(in, out, real_length, buffer, noda); 
    fclose(out);
    out = fopen("output.txt", "rw");
    int smth=0;
    int  temp = fgetc(out);
    smth += temp<<24;
    temp =  fgetc(out);
    smth += temp<<16;
    temp = fgetc(out);
    smth += temp<<8;
    temp = fgetc(out);
    smth += temp;
    return 0;
}