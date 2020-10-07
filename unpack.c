#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int unpack(FILE *in, char path[512]){
    char name[256];
    fgets(name, 255, in);
    name[strlen(name)-1]='\0'; // kill '\n' with null-terminator
    strcat(path, name);
    FILE *out = fopen(path, "w");
    if (out == NULL) {
        return -1; //outer file not opened
    }
    int pos = ftell(in); // initial position in the stream, where reversed file begins
    fseek(in, 0, SEEK_END); // set position to the end of the file
    do { //minimum 1 bite must be read
        fseek(in, -1, SEEK_CUR);//moving one byte lefter
        fputc(fgetc(in), out);//reading byte
        fseek(in, -1, SEEK_CUR);//moveing one byte lefter
    } while(ftell(in)!=pos); // while current position is not initial position
    return 0;
}