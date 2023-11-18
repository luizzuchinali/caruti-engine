#pragma once

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Graphics/Shader.hpp"

namespace Core {

    class Entity {
    public:
        glm::vec3 Position = glm::vec3(0, 0, 0);
        glm::vec3 Rotation = glm::vec3(0, 0, 0);
        glm::vec3 Scale = glm::vec3(1, 1, 1);
        glm::mat4 Model = glm::mat4(1);

    protected:
        explicit Entity(
                glm::vec3 position = glm::vec3(0, 0, 0),
                glm::vec3 rotation = glm::vec3(0, 0, 0),
                glm::vec3 scale = glm::vec3(1, 1, 1)
        ) : Position(position), Rotation(rotation), Scale(scale) {
        }

    public:

        virtual void Update(const float &deltaTime) {
            Model = glm::mat4(1);
            Model = translate(Model, Position);
            Model = rotate(Model, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
            Model = rotate(Model, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
            Model = rotate(Model, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
            Model = scale(Model, Scale);
        }

        virtual void Render(Graphics::Shader &shader) {
        }

        virtual void UIRender() {

        }

        Entity() = default;

        virtual ~Entity() = default;
    };

}