#include <iostream>
#include <filesystem>
#include <string>
#include <stdlib.h>
#include <cstring>

using namespace std;

namespace fs = std::filesystem;

bool check_directory_write_permission(char *name);

extern "C" int check_directory_properties(char *name)
{
    if (!fs::exists(name))
    {
        if (!fs::create_directories(name))
        {
            return -1; //unable to create directory
        }
        return 0;
    }
    if (!check_directory_write_permission(name))
    {
        return -2; //unable to write to the directory
    }
}

bool check_directory_write_permission(char *name)
{
    char *name_ = (char *)malloc((strlen(name) + 10) * sizeof(char));
    strcpy(name_, name);
    strcpy(name_ + strlen(name), "/testfile");
    FILE *testfile = fopen(name_, "w");
    if (testfile == NULL)
    {
        return false;
    }
    fclose(testfile);
    remove(name_);
    return true;
}