#pragma once

#include "LightCube.hpp"
#include "Camera.hpp"

#include <sstream>

#include "Core/DirectionalLight.hpp"
#include "Plane.hpp"
#include "Cube.hpp"
#include <memory>
#include <random>

class FramebufferScene {
public:
    unsigned int FBO{}, TexColorBuffer{}, RBO{};
    unsigned int VegetationVAO{}, VegetationVBO{};
    unsigned int ScreenVAO{}, ScreenVBO{};
    Core::DirectionalLight DirectionalLight;

    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>("VertexShader.vert",
                                                                                     "LitShader.frag");
    std::shared_ptr<Graphics::Shader> Shader = std::make_shared<Graphics::Shader>("FramebufferScreenShader.vert",
                                                                                  "FramebufferScreenShader.frag");
    Graphics::Texture TerrainGrassTexture = Graphics::Texture("TerrainGrassTexture.jpg", GL_TEXTURE0);
    Graphics::Texture GrassTexture = Graphics::Texture("resources/textures/grass.png", GL_TEXTURE0, GL_CLAMP_TO_EDGE);

    Plane Plane;
    Cube Cube;
    std::vector<glm::vec3> vegetation;

    float vegetationVertices[48] = {
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,

            0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0, 0, 1,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0, 0, 1,
            1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0, 0, 1,
    };

    float ScreenVertices[24] = {
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };

    FramebufferScene() {
        DirectionalLight.Ambient = glm::vec3(0.2, 0.2, 0.2);
        glGenVertexArrays(1, &VegetationVAO);
        glBindVertexArray(VegetationVAO);

        glGenBuffers(1, &VegetationVBO);
        glBindBuffer(GL_ARRAY_BUFFER, VegetationVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vegetationVertices), &vegetationVertices[0], GL_DYNAMIC_DRAW);

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

        for (int i = 0; i < 500; i++) {
            float randomX = disX(gen);
            float randomZ = disZ(gen);

            vegetation.emplace_back(randomX, 0.0f, randomZ);
        }

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glGenTextures(1, &TexColorBuffer);
        glBindTexture(GL_TEXTURE_2D, TexColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2560, 1440, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TexColorBuffer, 0);

        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 2560, 1440);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Information("FRAMEBUFFER: Framebuffer is not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenVertexArrays(1, &ScreenVAO);
        glGenBuffers(1, &ScreenVBO);
        glBindVertexArray(ScreenVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ScreenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenVertices), &ScreenVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
        glBindVertexArray(0);
    }

    void Show(const float deltaTime, [[maybe_unused]] const float currentTime, Camera &camera) {
        DirectionalLight.UIRender();

        // first pass
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

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

        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glDisable(GL_DEPTH_TEST);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(ScreenVAO);
        Shader->Use();
        Shader->SetInt("screenTexture", 0);
        glBindTexture(GL_TEXTURE_2D, TexColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
};
