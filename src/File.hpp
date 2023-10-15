#pragma once

#include <string>
#include <iostream>
#include <fstream>

//TODO: Make this class throw exception if load fails
class File {
public:
    static std::string GetAllLines(const std::string &path);
};