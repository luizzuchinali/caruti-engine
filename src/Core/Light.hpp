#pragma once

#include "Entity.hpp"

namespace Core {
    class Light : public Core::Entity {
    public:
        glm::vec3 Ambient{};
        glm::vec3 Diffuse{};
        glm::vec3 Specular{};

    protected:
        explicit Light(
                glm::vec3 position = {0, 0, 0},
                glm::vec3 rotation = {0, 0, 0},
                glm::vec3 scale = {1, 1, 1}
        ) : Entity(position, rotation, scale) {

        }

    };
}