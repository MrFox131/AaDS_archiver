#ifndef GET_DIRECTORY_STRUCTURE_H
#define GET_DIRECTORY_STRUCTURE_H

typedef struct directory {
    char* base;
    char **files;
    int files_count;
    int files_buf_size;
} Directory;

Directory* get_directory_structure(char* name);
void destruct_dir(Directory* dir);
int check_if_folder_type(char* name);

#endif