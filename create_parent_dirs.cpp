#include <filesystem>
#include <cstring>
using namespace std;

namespace fs = std::filesystem;

extern "C" void create_parent_dirs(char* path){
    auto path_ = fs::path(path);
    fs::create_directories(path_.remove_filename());
}