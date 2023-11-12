#pragma once

#include "LightCube.hpp"
#include "Camera.hpp"

#include <sstream>

#include "Core/DirectionalLight.hpp"
#include "Plane.hpp"
#include <memory>
#include <random>

class DenseGrassScene {
public:
    unsigned int VegetationVAO{}, VegetationVBO{};
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>("VertexShader.vert",
                                                                                     "LightingShader.frag");
    Graphics::Texture TerrainGrassTexture = Graphics::Texture("TerrainGrassTexture.jpg", GL_RGB, GL_TEXTURE0);
    Graphics::Texture GrassTexture = Graphics::Texture("grass.png", GL_RGBA, GL_TEXTURE1, GL_CLAMP_TO_EDGE);

    Plane Plane;
    std::vector<glm::vec3> vegetation;

    float VegetationVertices[48] = {
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,

            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,
            1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 0, 1,
    };

    DenseGrassScene() : Plane(LitShader) {
        DirectionalLight.Ambient = glm::vec3(0.2, 0.2, 0.2);

        LitShader->Use();

        glGenVertexArrays(1, &VegetationVAO);
        glBindVertexArray(VegetationVAO);

        glGenBuffers(1, &VegetationVBO);
        glBindBuffer(GL_ARRAY_BUFFER, VegetationVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VegetationVertices), &VegetationVertices[0], GL_DYNAMIC_DRAW);

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

        for (int i = 0; i < 2000; i++) {
            float randomX = disX(gen);
            float randomZ = disZ(gen);

            vegetation.emplace_back(randomX, 0.0f, randomZ);
        }

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

        glBindVertexArray(VegetationVAO);
        LitShader->SetTexture("material.texture_diffuse1", GrassTexture);
        for (auto i: vegetation) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, i);
            LitShader->SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
    }
};
