#include <sstream>
#include "File.hpp"

std::string File::GetAllLines(const std::string &path) {
    std::ifstream file(path);
    std::string content;

    if (file.is_open()) {
        std::stringstream stream;
        stream << file.rdbuf();
        content = stream.str();
        file.close();
    } else {
        std::cerr << "ERROR::FILE::LOAD_FAILED\n" << path << std::endl;
    }
    return content;
}