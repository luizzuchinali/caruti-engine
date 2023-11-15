#pragma once

#include "glm/glm.hpp"
#include "Shader.hpp"
#include <string>
#include <vector>

namespace Graphics {
    struct Vertex {
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
    };

    struct TextureIdentifier {
        unsigned int Id;
        std::string Type;
        std::string Path;
    };

    class Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
        std::vector<TextureIdentifier> Textures;
        unsigned int VAO{}, VBO{}, EBO{};

        Mesh(const std::vector<Vertex> &vertices,
             const std::vector<unsigned int> &indices,
             const std::vector<TextureIdentifier> &textures);

        void Draw(Shader &shader);

    private:
        void SetupMesh();
    };
}

