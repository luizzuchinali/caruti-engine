#pragma once

#include "LightCube.hpp"
#include "Camera.hpp"

#include <sstream>

#include "Core/DirectionalLight.hpp"
#include "Plane.hpp"
#include <memory>
#include <random>
#include <map>

class SemiTransparentTexturesScene {
public:
    unsigned int VAO{}, VBO{};
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>("VertexShader.vert",
                                                                                     "LightingShader.frag");
    Graphics::Texture TerrainGrassTexture = Graphics::Texture("TerrainGrassTexture.jpg", GL_RGB, GL_TEXTURE0);
    Graphics::Texture WindowTexture = Graphics::Texture("blending_transparent_window.png", GL_RGBA, GL_TEXTURE1);

    Plane Plane;
    std::vector<glm::vec3> Windows;

    float Vertices[48] = {
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,

            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,
            1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 0, 1
    };

    SemiTransparentTexturesScene() : Plane(LitShader) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        DirectionalLight.Ambient = glm::vec3(0.2, 0.2, 0.2);

        LitShader->Use();

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), &Vertices[0], GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 5));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> disX(-5.3f, 4.3f);
        std::uniform_real_distribution<float> disZ(-5.0f, 5.0f);

        for (int i = 0; i < 50; i++) {
            float randomX = disX(gen);
            float randomZ = disZ(gen);

            Windows.emplace_back(randomX, 0.0f, randomZ);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Show(const float deltaTime, [[maybe_unused]] const float currentTime, Camera &camera) {
        DirectionalLight.UIRender();

        LitShader->Use();
        LitShader->SetVec3("cameraPos", camera.Position);
        LitShader->SetFloat("material.shininess", 32.0f);

        LitShader->SetVec3("dirLight.direction", DirectionalLight.Direction);
        LitShader->SetVec3("dirLight.ambient", DirectionalLight.Ambient);
        LitShader->SetVec3("dirLight.diffuse", DirectionalLight.Diffuse);
        LitShader->SetVec3("dirLight.specular", DirectionalLight.Specular);

        LitShader->SetTexture("material.texture_diffuse1", TerrainGrassTexture);
        Plane.Update(deltaTime);
        Plane.Render(camera.GetCameraMatrix());

        std::map<float, glm::vec3> sortedWindows;
        for (auto Window: Windows) {
            float distance = glm::length(camera.Position - Window);
            sortedWindows[distance] = Window;
        }

        glBindVertexArray(VAO);
        LitShader->SetTexture("material.texture_diffuse1", WindowTexture);
        for (auto it = sortedWindows.rbegin(); it != sortedWindows.rend(); ++it) {
            float distance = it->first;
            glm::vec3 window = it->second;

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, window);
            LitShader->SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
    }
};
