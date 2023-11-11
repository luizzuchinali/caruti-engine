#pragma once
#include "LightCube.hpp"
#include "Camera.hpp"

#include <sstream>

class SponzaScene {
public:
    Core::DirectionalLight DirectionalLight;

    std::shared_ptr<Graphics::Shader> LightSourceShader = std::make_shared<Graphics::Shader>(
        "VertexShader.vert", "LightSourceShader.frag");
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
        "VertexShader.vert", "LightingShader.frag");

    LightCube LightCubes[4] = {
        LightCube(LightSourceShader, glm::vec3(-50, 50, -50), glm::vec3(0), glm::vec3(30)),
        LightCube(LightSourceShader, glm::vec3(-40, 50, 50), glm::vec3(0), glm::vec3(30)),
        LightCube(LightSourceShader, glm::vec3(55, 50, -50), glm::vec3(0), glm::vec3(30)),
        LightCube(LightSourceShader, glm::vec3(55, 50, 50), glm::vec3(0), glm::vec3(30))
    };

    Graphics::Model Sponza = Graphics::Model("resources/models/sponza/sponza.obj");
    float Amplitude = 3.6;
    float Freq = 0.05;


    SponzaScene() {
        for (int i = 0; i < sizeof(LightCubes) / sizeof(LightCube); i++) {
            LightCubes[i].Id = "Point Light " + std::to_string(i);
        }
    }

    void Show(const float deltaTime, const float currentTime, Camera&camera) {
        DirectionalLight.UIRender();
        for (auto&lightCube: LightCubes) {
            lightCube.UIRender();
        }

        if (!ImGui::Begin("Scene Settings")) {
            ImGui::End();
        }

        ImGui::InputFloat("Amplitude", &Amplitude);
        ImGui::InputFloat("Frequency", &Freq);

        ImGui::End();


        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        LitShader->Use();
        LitShader->SetVec3("cameraPos", camera.Position);
        LitShader->SetFloat("material.shininess", 32.0f);

        LitShader->SetVec3("dirLight.direction", DirectionalLight.Direction);
        LitShader->SetVec3("dirLight.ambient", DirectionalLight.Ambient);
        LitShader->SetVec3("dirLight.diffuse", DirectionalLight.Diffuse);
        LitShader->SetVec3("dirLight.specular", DirectionalLight.Specular);

        const auto offset = glm::sin(2 * glm::pi<float>() * Freq * currentTime) * Amplitude;
        for (int i = 0; i < sizeof(LightCubes) / sizeof(LightCube); i++) {
            LightCubes[i].Position.x += offset;
            LightCubes[i].Update(deltaTime);
            LightCubes[i].Render(camera.GetCameraMatrix());

            std::ostringstream name;
            name << "pointLights[" << i << "].";
            auto nameStr = name.str();

            LitShader->Use();
            LitShader->SetVec3(nameStr + "position", LightCubes[i].Position);
            LitShader->SetVec3(nameStr + "ambient", LightCubes[i].Ambient);
            LitShader->SetVec3(nameStr + "diffuse", LightCubes[i].Diffuse);
            LitShader->SetVec3(nameStr + "specular", LightCubes[i].Specular);
            LitShader->SetFloat(nameStr + "constant", LightCubes[i].Constant);
            LitShader->SetFloat(nameStr + "linear", LightCubes[i].Linear);
            LitShader->SetFloat(nameStr + "quadratic", LightCubes[i].Quadratic);
        }

        LitShader->SetVec3("spotLight.position", camera.Position);
        LitShader->SetVec3("spotLight.direction", camera.Front);
        LitShader->SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        LitShader->SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        LitShader->SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        LitShader->SetFloat("spotLight.constant", 1.0f);
        LitShader->SetFloat("spotLight.linear", 0.09f);
        LitShader->SetFloat("spotLight.quadratic", 0.032f);
        LitShader->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        LitShader->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        constexpr glm::mat4 model = glm::mat4(1.0f);
        LitShader->SetMat4("model", model);
        LitShader->SetMat4("camera", camera.GetCameraMatrix());
        Sponza.Draw(*LitShader);
    }
};
