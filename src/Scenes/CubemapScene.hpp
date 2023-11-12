#pragma once

#include "LightCube.hpp"
#include "Camera.hpp"

#include <sstream>

#include "Core/DirectionalLight.hpp"
#include "Plane.hpp"
#include "Graphics/Model.hpp"
#include <memory>
#include <random>

class CubeMapScene {
public:
    unsigned int VegetationVAO{}, VegetationVBO{};
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>("VertexShader.vert",
                                                                                     "LightingShader.frag");

    unsigned int SkyboxVAO{}, SkyboxVBO{}, SkyboxTexture{};
    std::shared_ptr<Graphics::Shader> SkyboxShader = std::make_shared<Graphics::Shader>("Skybox.vert", "Skybox.frag");

    Graphics::Texture TerrainGrassTexture = Graphics::Texture("forrest_ground_01/forrest_ground_01_diff_1k.jpg", GL_TEXTURE0);
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

    float SkyboxVertices[108] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };


    CubeMapScene() : Plane(LitShader) {
        DirectionalLight.Direction = glm::vec3(-0.2, -1, -1);
        DirectionalLight.Ambient = glm::vec3(0.4, 0.4, 0.4);
        DirectionalLight.Diffuse = glm::vec3(0.8, 0.8, 0.8);

        LitShader->Use();
        std::vector<std::string> faces{
                "resources/textures/skybox/skycl/right.png",
                "resources/textures/skybox/skycl/left.png",
                "resources/textures/skybox/skycl/top.png",
                "resources/textures/skybox/skycl/bottom.png",
                "resources/textures/skybox/skycl/front.png",
                "resources/textures/skybox/skycl/back.png"
        };
        SkyboxTexture = LoadCubemap(faces);

        glGenVertexArrays(1, &SkyboxVAO);
        glBindVertexArray(SkyboxVAO);

        glGenBuffers(1, &SkyboxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, SkyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), &SkyboxVertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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

        glDepthFunc(GL_LEQUAL);
        SkyboxShader->Use();
        SkyboxShader->SetMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        SkyboxShader->SetMat4("projection", Camera::GetProjectionMatrix());
        glBindVertexArray(SkyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

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

    static unsigned int LoadCubemap(std::vector<std::string> faces) {
        stbi_set_flip_vertically_on_load(false);
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format;
                if (nrChannels == 1)
                    format = GL_RED;
                else if (nrChannels == 3)
                    format = GL_RGB;
                else if (nrChannels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            } else {
                Log::Error("Cubemap tex failed to load at path: {}", faces[i]);
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
};
