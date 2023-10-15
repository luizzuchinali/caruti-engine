#include "Shader.hpp"

Shader::Shader(const char *vertexName, const char *fragmentName) {

    uint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexName);
    uint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentName);

    CreateProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Use() const {
    glUseProgram(_id);
}

void Shader::SetBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(_id, name.c_str()), (int) value);
}

void Shader::SetInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}

uint Shader::CreateShader(GLenum type, const std::string &fileName) {
    const std::string basePath = "resources/shaders/";
    std::string strShaderCode = File::GetAllLines(basePath + fileName);
    const char *shaderCode = strShaderCode.c_str();

    uint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        if (type == GL_VERTEX_SHADER)
            Log::Error("SHADER::VERTEX::COMPILATION_FAILED: {}", info);
        else
            Log::Error("SHADER::FRAGMENT::COMPILATION_FAILED: {}", info);
    }
    return shader;
}

void Shader::CreateProgram(unsigned int vertex, unsigned int fragment) {
    _id = glCreateProgram();
    glAttachShader(_id, vertex);
    glAttachShader(_id, fragment);
    glLinkProgram(_id);

    int success;
    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(_id, 512, nullptr, info);
        Log::Error("SHADER::PROGRAM::LINK_FAILED: {}", info);
    }
}