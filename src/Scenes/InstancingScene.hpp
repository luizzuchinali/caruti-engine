#pragma once

#include "Plane.hpp"
#include "Camera.hpp"
#include "LightCube.hpp"

#include "Core/DirectionalLight.hpp"
#include "Core/Skybox.hpp"
#include "Graphics/Model.hpp"
#include "PerlinNoise.hpp"

#include <sstream>
#include <memory>
#include <random>

class InstancingScene {
public:
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert",
            "LightingShader.frag"
    );

    Core::Skybox Skybox = Core::Skybox(std::vector<std::string>{
            "resources/textures/skybox/space/right.png",
            "resources/textures/skybox/space/left.png",
            "resources/textures/skybox/space/top.png",
            "resources/textures/skybox/space/bottom.png",
            "resources/textures/skybox/space/front.png",
            "resources/textures/skybox/space/back.png"
    });

    Graphics::Model Planet = Graphics::Model("resources/models/planet/planet.obj");
    Graphics::Model Rock = Graphics::Model("resources/models/rock/rock.obj");

    unsigned int Amount = 2000;
    glm::mat4 *ModelMatrices = new glm::mat4[Amount];

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{ seed };

    InstancingScene() {
        DirectionalLight.Direction = glm::vec3(-0.2, -1, -1);
        DirectionalLight.Ambient = glm::vec3(0.1, 0.1, 0.1);
        DirectionalLight.Diffuse = glm::vec3(0.8, 0.8, 0.8);

        srand(glfwGetTime());
    }

    void Show(const float deltaTime, [[maybe_unused]] const float currentTime, Camera &camera) {
        DirectionalLight.UIRender();

        Skybox.Render();

        LitShader->Use();
        LitShader->SetVec3("cameraPos", camera.Position);
        LitShader->SetFloat("material.shininess", 32.0f);
        LitShader->SetVec3("dirLight.direction", DirectionalLight.Direction);
        LitShader->SetVec3("dirLight.ambient", DirectionalLight.Ambient);
        LitShader->SetVec3("dirLight.diffuse", DirectionalLight.Diffuse);
        LitShader->SetVec3("dirLight.specular", DirectionalLight.Specular);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        LitShader->SetMat4("model", model);
        Planet.Draw(*LitShader);

        float radius = 50.0;
        float offset = 2.5f;
        float orbitSpeed = fmax(sin(glfwGetTime() * 0.05), 0);
        for (unsigned int i = 0; i < Amount; i++) {
            model = glm::mat4(1.0f);

            float angle = (float) i/ (float) Amount * 360.0f;
            float displacement = (i % (int) (2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle + orbitSpeed) * radius + displacement;

            displacement = (i % (int) (2 * offset * 100)) / 100.0f - offset;
            float y = displacement * 0.4f;
            displacement = perlin.noise1D(i) / 100.0f - offset;
            float z = cos(angle + orbitSpeed) * radius + displacement;
            model = glm::translate(model, glm::vec3(x, y, z));

            float scale = (i % 20) / 100.0f + 0.05;
            model = glm::scale(model, glm::vec3(scale));

            float rotAngle = (i % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

            ModelMatrices[i] = model;
        }

        for (unsigned int i = 0; i < Amount; i++) {
            LitShader->SetMat4("model", ModelMatrices[i]);
            Rock.Draw(*LitShader);
        }
    }
};
