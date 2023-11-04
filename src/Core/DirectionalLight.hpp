#pragma once

#include "Entity.hpp"
#include "Light.hpp"
#include "imgui.h"

namespace Core {
    class DirectionalLight : public Core::Light {
    public:
        glm::vec3 Direction;

        explicit DirectionalLight(
                glm::vec3 direction = {-0.2f, -1.0f, -0.3f},
                glm::vec3 ambient = {0.2, 0.2, 0.2},
                glm::vec3 diffuse = {0.4, 0.4, 0.4},
                glm::vec3 specular = {0.5f, 0.5f, 0.5f}
        ) : Direction(direction), Light(
                {0, 0, 0},
                {0, 0, 0},
                {1, 1, 1}
        ) {
            Ambient = ambient;
            Diffuse = diffuse;
            Specular = specular;
        }

        void UIRender() override {
            if (!ImGui::Begin("Light Settings")) {
                ImGui::End();
            }

            if (ImGui::CollapsingHeader("Directional Light")) {
                ImGui::InputFloat3("Direction", &Direction[0]);
                ImGui::InputFloat3("Ambient", &Ambient[0]);
                ImGui::InputFloat3("Diffuse", &Diffuse[0]);
                ImGui::InputFloat3("Specular", &Specular[0]);
            }

            ImGui::End();
        }
    };
}