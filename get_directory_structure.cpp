#include <iostream>
#include <filesystem>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <vector>


using namespace std;

typedef struct directory {
    char *base;
    char **files;
    int files_count;
    int files_buf_size;
} Directory;

namespace fs = std::filesystem;

#define INITIAL_DIRECTORY_BUFFERS_SIZE 10

extern "C" Directory* new_directory(){
    Directory *dir = (Directory*)malloc(sizeof(Directory));
    dir->files_buf_size = INITIAL_DIRECTORY_BUFFERS_SIZE;
    dir->files_count = 0;
    dir->files = (char**)calloc(INITIAL_DIRECTORY_BUFFERS_SIZE, sizeof(char*));
    return dir;    
}

int add_new_file(struct directory *dir, const char* name){
    if(dir->files_buf_size == dir->files_count){
        dir->files = (char**)realloc(dir->files, (dir->files_buf_size+INITIAL_DIRECTORY_BUFFERS_SIZE)*sizeof(char**));
        dir->files_buf_size+=INITIAL_DIRECTORY_BUFFERS_SIZE;
    }

    dir->files[dir->files_count] = (char*)calloc(strlen(name)+1, sizeof(char));
    strcpy(dir->files[dir->files_count], name);
    dir->files_count++;
    return 0;
}

extern "C" Directory* get_directory_structure(char* name){
    auto path = fs::absolute(name).make_preferred();
    auto dir = new_directory();
    if(!(fs::exists(name))){
        return NULL;
    }
    auto status = fs::status(path);
    if(status.type()==fs::file_type(2)){
        dir->base = (char*)calloc(strlen(path.string().c_str())+3, sizeof(char));   
        strcpy(dir->base, path.string().c_str());
        if (dir->base[strlen(dir->base)-1]!='\\' && dir->base[strlen(dir->base)-1]!='/')
            dir->base[strlen(dir->base)]=fs::path().preferred_separator;
            dir->base[strlen(dir->base)+1]='\0';
        for (auto &p: fs::recursive_directory_iterator(path)){
            if(fs::status(p.path()).type()==fs::file_type(2)){
                continue;
            } else {
                add_new_file(dir, p.path().string().c_str());
            }
        }
    } else {
        auto base_path = fs::absolute(name).remove_filename().make_preferred();
        dir->base = (char*)calloc(strlen(base_path.string().c_str())+1, sizeof(char));   
        strcpy(dir->base, base_path.string().c_str());
        add_new_file(dir, path.string().c_str());
    }
    return dir;
}

extern "C" void destruct_dir(Directory* dir){
    for(int i(0); i<dir->files_count; i++){
        free(dir->files[i]);
    }
    free(dir->files);
    free(dir);
}

extern "C" int check_if_folder_type(char* name){
    return int(fs::exists(name) && fs::status(name).type()==fs::file_type(2));
}

#ifdef STANDALONE
int main(){
    get_directory_structure(".vscode");
}
#endif