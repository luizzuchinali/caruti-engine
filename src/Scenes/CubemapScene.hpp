#pragma once

#include "Plane.hpp"
#include "Camera.hpp"
#include "LightCube.hpp"

#include "Core/DirectionalLight.hpp"
#include "Core/Skybox.hpp"
#include "Graphics/Model.hpp"

#include <sstream>
#include <memory>
#include <random>

class CubeMapScene {
public:
    unsigned int VegetationVAO{}, VegetationVBO{};
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert",
            "LitShader.frag"
    );

    Core::Skybox Skybox = Core::Skybox(std::vector<std::string>{
            "resources/textures/skybox/bsky/right.png",
            "resources/textures/skybox/bsky/left.png",
            "resources/textures/skybox/bsky/top.png",
            "resources/textures/skybox/bsky/bottom.png",
            "resources/textures/skybox/bsky/front.png",
            "resources/textures/skybox/bsky/back.png"
    });

    Graphics::Texture TerrainGrassTexture = Graphics::Texture(
            "resources/textures/forrest_ground_01/forrest_ground_01_diff_1k.jpg",
            GL_TEXTURE0
    );

    Graphics::Texture GrassTexture = Graphics::Texture("grass.png", GL_TEXTURE1, GL_CLAMP_TO_EDGE);

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


    CubeMapScene() {
        DirectionalLight.Direction = glm::vec3(-0.2, -1, -1);
        DirectionalLight.Ambient = glm::vec3(0.4, 0.4, 0.4);
        DirectionalLight.Diffuse = glm::vec3(0.8, 0.8, 0.8);

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

        for (int i = 0; i < 300; i++) {
            float randomX = disX(gen);
            float randomZ = disZ(gen);

            vegetation.emplace_back(randomX, 0.0f, randomZ);
        }

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


        LitShader->SetTexture("material.texture_diffuse1", TerrainGrassTexture);
        Plane.Update(deltaTime);
        Plane.Render(*LitShader);

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
