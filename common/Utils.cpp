#include "Utils.h"

#include <iostream>

std::string ReadFile(const std::string &path)
{
    std::ifstream file;
    std::stringstream ss;

    file.open(path);
    if(!file.is_open())
        throw std::runtime_error("Could not open file " + path);
    ss << file.rdbuf();
    file.close();
    return ss.str();
}
