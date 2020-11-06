#include <iostream>
#include <filesystem>
#include <string>
#include <stdlib.h>
#include <cstring>


using namespace std;

extern "C" typedef struct directory {
    char **files;
    char **directories;
    int dirs_count;
    int files_count;
    int files_buf_size;
    int dirs_buf_size;
} Directory;

namespace fs = std::filesystem;

extern "C" void get_directory_structure(char* name, Directory* dir, int (*add_new_directory)(Directory*, int, const char*), int (*add_new_file)(Directory*, int, const char*)){

    if(!(fs::exists(name))){
        dir->files=NULL;
        dir->directories = NULL;
        return;
    }
    auto status = fs::status(name);
    if(status.type()==fs::file_type(2)){
        for (auto &p: fs::directory_iterator(name)){
            if(fs::status(p.path()).type()==fs::file_type(2)){
                add_new_directory(dir, p.path().string().length(), p.path().string().c_str());
            } else {
                add_new_file(dir, p.path().string().length(), p.path().string().c_str());
                }
        }
    } else {
        free(dir->directories);
        add_new_file(dir, strlen(name), name);
    }
}

#ifdef STANDALONE
int main(){
    get_directory_structure(".vscode");
}
#endif