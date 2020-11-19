#include <stdio.h>

int pack(char *name, FILE *in, FILE *out)
{
    fprintf(out, "%s\n", name);
    fseek(in, 0, SEEK_END);
    do
    {
        fseek(in, -1, SEEK_CUR);
        fputc(fgetc(in), out);
        fseek(in, -1, SEEK_CUR);
    } while (ftell(in) != 0);
    return 0;
}