#pragma once

#include "Entity.hpp"

namespace Core {
    struct LightProps {
        glm::vec3 Ambient = {0.05, 0.05, 0.05};
        glm::vec3 Diffuse = {0.8, 0.8, 0.8};
        glm::vec3 Specular = {0.8, 0.8, 0.8};
    };

    class Light : public Core::Entity {
    public:
        glm::vec3 Ambient{};
        glm::vec3 Diffuse{};
        glm::vec3 Specular{};

    protected:
        explicit Light(glm::vec3 position = {0, 0, 0},
                       glm::vec3 rotation = {0, 0, 0},
                       glm::vec3 scale = {1, 1, 1}
        ) : Entity(position, rotation, scale) {
        }

    };
}