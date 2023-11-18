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
#include <execution>

class InstancingScene {
public:
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert",
            "LitShader.frag"
    );

    std::shared_ptr<Graphics::Shader> InstancingLitShader = std::make_shared<Graphics::Shader>(
            "InstancingShader.vert",
            "LitShader.frag"
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

    unsigned int Amount = 50000;
    glm::mat4 *ModelMatrices = new glm::mat4[Amount];

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};

    unsigned int VBO{};

    InstancingScene() {
        DirectionalLight.Direction = glm::vec3(-0.2, -1, -1);
        DirectionalLight.Ambient = glm::vec3(0.1, 0.1, 0.1);
        DirectionalLight.Diffuse = glm::vec3(0.8, 0.8, 0.8);

        srand(glfwGetTime());

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, Amount * sizeof(glm::mat4), &ModelMatrices[0], GL_DYNAMIC_DRAW);

        for (auto &Mesh: Rock.Meshes) {
            unsigned int VAO = Mesh.VAO;
            glBindVertexArray(VAO);

            std::size_t vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, nullptr);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (1 * vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (2 * vec4Size));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (3 * vec4Size));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
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

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        LitShader->SetMat4("model", model);
        Planet.Draw(*LitShader);

        InstancingLitShader->Use();
        InstancingLitShader->SetVec3("cameraPos", camera.Position);
        InstancingLitShader->SetFloat("material.shininess", 32.0f);
        InstancingLitShader->SetVec3("dirLight.direction", DirectionalLight.Direction);
        InstancingLitShader->SetVec3("dirLight.ambient", DirectionalLight.Ambient);
        InstancingLitShader->SetVec3("dirLight.diffuse", DirectionalLight.Diffuse);
        InstancingLitShader->SetVec3("dirLight.specular", DirectionalLight.Specular);

        float radius = 100.0;
        float offset = 25.0f;
        float orbitSpeed = fmax(sin(glfwGetTime() * 0.05), 0);

        std::for_each(
                std::execution::par,
                ModelMatrices,
                ModelMatrices + Amount,
                [&](glm::mat4 &model) {
                    const unsigned int i = std::distance(ModelMatrices, &model);
                    model = glm::mat4(1.0f);

                    float angle = (float) i / (float) Amount * 360.0f;
                    float displacement = ((perlin.noise1D(i) * 2 * offset * 100)) / 100.0f - offset;
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
                });

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                Amount * sizeof(glm::mat4),
                &ModelMatrices[0]
        );

        InstancingLitShader->Use();
        for (auto &Meshe: Rock.Meshes) {
            glBindVertexArray(Meshe.VAO);
            glDrawElementsInstanced(
                    GL_TRIANGLES, Meshe.Indices.size(), GL_UNSIGNED_INT, 0, Amount
            );
        }
//
//        for (unsigned int i = 0; i < Amount; i++) {
//            LitShader->SetMat4("model", ModelMatrices[i]);
//            Rock.Draw(*LitShader);
//        }
    }
};
