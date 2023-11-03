#pragma once

#include <string>
#include "File.hpp"
#include "Texture.hpp"
#include "Log.hpp"
#include "glm/glm.hpp"

namespace Graphics {
    class Shader {
    public:
        Shader(const char *vertexName, const char *fragmentName);

        void Use() const;

        void SetBool(const std::string &name, bool value) const;

        void SetInt(const std::string &name, int value) const;

        void SetFloat(const std::string &name, float value) const;

        void SetTexture(const char *uName, const Texture &texture) const;

        void SetMat4(const std::string &name, glm::mat4 matrix) const;

        void SetVec3(const std::string &name, glm::vec3 &vec) const;

        void SetVec3(const std::string &name, float x, float y, float z) const;

        void SetVec4(const std::string &name, glm::vec4 &vec) const;

        void SetVec4(const std::string &name, float x, float y, float z, float w) const;

        ~Shader() {
            glDeleteProgram(_id);
        }

    private:
        unsigned int _id{};

        static unsigned int CreateShader(GLenum type, const std::string &fileName);

        void CreateProgram(unsigned int vertex, unsigned int fragment);

    };
}
