#pragma once

#include "Plane.hpp"
#include "Camera.hpp"
#include "LightCube.hpp"

#include "Core/DirectionalLight.hpp"
#include "Core/Skybox.hpp"
#include "Graphics/Model.hpp"
#include "Cube.hpp"

#include <sstream>
#include <memory>
#include <random>

class EnvironmentMappingScene {
public:
    Core::DirectionalLight DirectionalLight;
    Core::Skybox Skybox = Core::Skybox(std::vector<std::string>{
            "resources/textures/skybox/scythian_tombs/right.png",
            "resources/textures/skybox/scythian_tombs/left.png",
            "resources/textures/skybox/scythian_tombs/top.png",
            "resources/textures/skybox/scythian_tombs/bottom.png",
            "resources/textures/skybox/scythian_tombs/front.png",
            "resources/textures/skybox/scythian_tombs/back.png"
    });

    unsigned int VegetationVAO{}, VegetationVBO{};
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert",
            "LitShader.frag"
    );
    Graphics::Texture TerrainGrassTexture = Graphics::Texture(
            "resources/textures/forrest_ground_01/forrest_ground_01_diff_1k.jpg",
            GL_TEXTURE0
    );
    Graphics::Texture GrassTexture = Graphics::Texture("grass.png", GL_TEXTURE1, GL_CLAMP_TO_EDGE);
    Plane GrassPlane;
    std::vector<glm::vec3> vegetation;

    float VegetationVertices[48] = {
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,

            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,
            1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 0, 1,
    };

    std::shared_ptr<Graphics::Shader> ReflectionShader = std::make_shared<Graphics::Shader>(
            "ReflectionShader.vert",
            "ReflectionShader.frag"
    );

    Cube ReflectionCube;

    std::shared_ptr<Graphics::Shader> RefractionShader = std::make_shared<Graphics::Shader>(
            "RefractionShader.vert",
            "RefractionShader.frag"
    );
    Cube RefractionCube;


    EnvironmentMappingScene() : ReflectionCube(ReflectionShader), RefractionCube(RefractionShader),
                                GrassPlane(LitShader) {
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

        for (int i = 0; i < 2000; i++) {
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
        GrassPlane.Update(deltaTime);
        GrassPlane.Render();

        glBindVertexArray(VegetationVAO);
        LitShader->SetTexture("material.texture_diffuse1", GrassTexture);
        for (auto i: vegetation) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, i);
            LitShader->SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);


        ReflectionShader->Use();
        ReflectionShader->SetVec3("cameraPos", camera.Position);
        ReflectionShader->SetInt("skybox", 0);

        ReflectionCube.Rotation = {180, glm::sin(currentTime * 0.05) * 360, 180};
        ReflectionCube.Position = {0, 5, 0};
        ReflectionCube.Update(deltaTime);
        ReflectionCube.Render();

        RefractionShader->Use();
        RefractionShader->SetVec3("cameraPos", camera.Position);
        RefractionShader->SetInt("skybox", 0);

        RefractionCube.Rotation = {180, glm::sin(currentTime * 0.05) * 360, 180};
        RefractionCube.Position = {3, 5, 0};
        RefractionCube.Update(deltaTime);
        RefractionCube.Render();
    }
};

