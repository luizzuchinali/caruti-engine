#pragma once

#include "Entity.hpp"
#include "Light.hpp"
#include "imgui.h"

namespace Core {

    struct PointLightProps : public LightProps {
        float Constant = 1;
        float Linear = 0.007;
        float Quadratic = 0.0002;
    };

    class PointLight : public Core::Light {
    public:
        std::string Id;
        float Constant;
        float Linear;
        float Quadratic;

        explicit PointLight(PointLightProps props = PointLightProps(), glm::vec3 position = {0, 0, 0}) : Light(
                position,
                {0, 0, 0},
                {1, 1, 1}
        ) {
            Ambient = props.Ambient;
            Diffuse = props.Diffuse;
            Specular = props.Specular;
            Constant = props.Constant;
            Linear = props.Linear;
            Quadratic = props.Quadratic;
        }

        void UIRender() override {
            if (!ImGui::Begin("Light Settings")) {
                ImGui::End();
                return;
            }

            if (ImGui::CollapsingHeader(Id.c_str())) {
                ImGui::PushID(Id.c_str());
                ImGui::InputFloat3("Ambient", &Ambient[0]);
                ImGui::InputFloat3("Diffuse", &Diffuse[0]);
                ImGui::InputFloat3("Specular", &Specular[0]);
                ImGui::InputFloat("Constant", &Constant);
                ImGui::InputFloat("Linear", &Linear, 0, 0, "%.6f");
                ImGui::InputFloat("Quadratic", &Quadratic, 0, 0, "%.6f");
                ImGui::PopID();
            }

            ImGui::End();
        }
    };
}