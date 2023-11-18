#pragma once

#include "Core/Entity.hpp"
#include "Graphics/Shader.hpp"
#include "glm/ext/matrix_transform.hpp"

class Floor : public Core::Entity {
public:
    unsigned int VBO{}, VAO{};

    std::shared_ptr<Graphics::Shader> Shader;

    explicit Floor(
            const std::shared_ptr<Graphics::Shader> &shader,
            glm::vec3 position = glm::vec3(0, 0, 0),
            glm::vec3 rotation = glm::vec3(0, 0, 0),
            glm::vec3 scale = glm::vec3(10, 1, 10)
    ) : Shader(shader), Entity(position, rotation, scale) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Floor::Vertices), Floor::Vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void Render() override {
        Shader->Use();
        Shader->SetMat4("model", Model);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
    }

    constexpr const static float Vertices[48] = {
            // positions          // texture Coords
            5.0f, -0.5f, 5.0f,   5.0f, 0.0f, 0, 1, 0,
            -5.0f, -0.5f, 5.0f,  0.0f, 0.0f, 0, 1, 0,
            -5.0f, -0.5f, -5.0f, 0.0f, 5.0f, 0, 1, 0,

            5.0f, -0.5f, 5.0f,   5.0f, 0.0f, 0, 1, 0,
            -5.0f, -0.5f, -5.0f, 0.0f, 5.0f, 0, 1, 0,
            5.0f, -0.5f, -5.0f,  5.0f, 5.0f, 0, 1, 0,
    };
};
