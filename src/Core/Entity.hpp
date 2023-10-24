#pragma once

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

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
            Model = glm::scale(Model, Scale);
            Model = glm::translate(Model, Position);
            Model = glm::rotate(Model, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
            Model = glm::rotate(Model, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
            Model = glm::rotate(Model, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
        }

        virtual void Render() const = 0;

        Entity() = default;

        virtual ~Entity() = default;
    };

}