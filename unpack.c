#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int unpack(FILE *in, char path[512])
{
    char name[256];
    fgets(name, 255, in);
    name[strlen(name) - 1] = '\0'; // kill '\n' with null-terminator
    strcat(path, name);
    FILE *out = fopen(path, "w");
    if (out == NULL)
    {
        return -1;
    }
    int pos = ftell(in); // initial position in the stream, where reversed file begins
    fseek(in, 0, SEEK_END);
    do
    {
        fseek(in, -1, SEEK_CUR);
        fputc(fgetc(in), out);
        fseek(in, -1, SEEK_CUR);
    } while (ftell(in) != pos);
    return 0;
}