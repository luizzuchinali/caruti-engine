#pragma once

#include <string>
#include "Types.hpp"
#include "File.hpp"
#include "glad/glad.h"
#include "Log.hpp"

class Shader {
public:
    Shader(const char *vertexName, const char *fragmentName);

    void Use() const;

    void SetBool(const std::string &name, bool value) const;

    void SetInt(const std::string &name, int value) const;

    void SetFloat(const std::string &name, float value) const;

    ~Shader() {
        glDeleteProgram(_id);
    }

private:
    unsigned int _id{};

    static uint CreateShader(GLenum type, const std::string &fileName);

    void CreateProgram(uint vertex, uint fragment);
};