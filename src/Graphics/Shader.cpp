#include "Shader.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Graphics {

    Shader::Shader(const char *vertexName, const char *fragmentName) {

        unsigned int vertexShader = CreateShader(GL_VERTEX_SHADER, vertexName);
        unsigned int fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentName);

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

    unsigned int Shader::CreateShader(GLenum type, const std::string &fileName) {
        const std::string basePath = "resources/shaders/";
        std::string strShaderCode = File::GetAllLines(basePath + fileName);
        const char *shaderCode = strShaderCode.c_str();

        unsigned int shader = glCreateShader(type);
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

    void Shader::SetTexture(const char *uName, const Texture &texture) const {
        texture.ActivateAndBind();
        this->SetInt(uName, texture.GetIndex());
    }

    void Shader::SetMat4(const std::string &name, const glm::mat4 matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::SetVec3(const std::string &name, glm::vec3 &vec) const {
        glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &vec[0]);
    }

    void Shader::SetVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
    }


}