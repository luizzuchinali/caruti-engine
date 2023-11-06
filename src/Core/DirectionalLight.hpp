#pragma once

#include "Entity.hpp"
#include "Light.hpp"
#include "imgui.h"

namespace Core {
    struct DirectionalLightProps : LightProps {
        glm::vec3 Direction = {-0.2f, -1.0f, -0.3f};
        glm::vec3 Ambient = {0.2, 0.2, 0.2};
        glm::vec3 Diffuse = {0.4, 0.4, 0.4};
        glm::vec3 Specular = {0.5f, 0.5f, 0.5f};
    };

    class DirectionalLight : public Core::Light {
    public:
        glm::vec3 Direction;

        explicit DirectionalLight(DirectionalLightProps props = DirectionalLightProps()) :
                Direction(props.Direction),
                Light({0, 0, 0},
                      {0, 0, 0},
                      {1, 1, 1}
                ) {
            Ambient = props.Ambient;
            Diffuse = props.Diffuse;
            Specular = props.Specular;
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