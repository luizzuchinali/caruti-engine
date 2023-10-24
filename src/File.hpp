#pragma once

#include <string>
#include <iostream>
#include <fstream>

class File {
public:
    static std::string GetAllLines(const std::string &path);
};