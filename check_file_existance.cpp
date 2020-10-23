#include <iostream>
#include <filesystem>

using namespace std;

extern "C" int file_exists(char* name){
    return 10080;
}