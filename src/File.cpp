#include <sstream>
#include "File.hpp"
#include <Log.hpp>

std::string File::GetAllLines(const std::string &path) {
    std::ifstream file(path);
    std::string content;

    if (file.is_open()) {
        std::stringstream stream;
        stream << file.rdbuf();
        content = stream.str();
        file.close();
    } else {
        Log::Information("FILE::LOAD_FAILED");
    }
    return content;
}