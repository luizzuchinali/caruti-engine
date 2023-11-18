#pragma once

#include "LightCube.hpp"
#include "Core/DirectionalLight.hpp"
#include "Floor.hpp"
#include "Camera.hpp"

#include <sstream>
#include <memory>
#include <random>

class WoodFloorWithCubesSceneWithShadow {
public:
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert",
            "LitShader.frag"
    );

    Floor Floor;
    Graphics::Texture WoodFloorTexture = Graphics::Texture(
            "resources/textures/wood_floor_deck/wood_floor_deck_diff.jpg",
            GL_TEXTURE0, GL_REPEAT, true
    );

    std::shared_ptr<Graphics::Shader> LightSourceShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert", "LightSourceShader.frag"
    );
    LightCube LightCubes[1] = {
            LightCube(LightSourceShader, {5, 2, 0})
    };


    Cube Cubes[9] = {
            Cube({3, 3, 0}),
            Cube({6, 3, 0}),
            Cube({9, 3, 0}),
            Cube({0, 1, 0}),
            Cube({-3, 3, 0}),
            Cube({-6, 3, 0}),
            Cube({-9, 3, 0}),
            Cube({-12, 3, 0}),
            Cube({-15, 3, 0}),
    };

    const unsigned int SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 1440;
    const unsigned int WINDOW_WIDTH = 2560, WINDOW_HEIGHT = 1440;
    unsigned int depthMapFBO{};
    unsigned int depthMapTexture{};
    std::shared_ptr<Graphics::Shader> DepthShader = std::make_shared<Graphics::Shader>(
            "DepthShader.vert",
            "DepthShader.frag"
    );

    unsigned int ScreenVAO{}, ScreenVBO{};
    float ScreenVertices[20] = {
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    std::shared_ptr<Graphics::Shader> DebugQuadShader = std::make_shared<Graphics::Shader>("DebugQuad.vert",
                                                                                           "DebugQuad.frag");

    WoodFloorWithCubesSceneWithShadow() :
            Floor({-20, 0, -20}) {

        DirectionalLight.Ambient = {0.1, 0.1, 0.1};
        DirectionalLight.Diffuse = {1, 1, 1};
        DirectionalLight.Specular = {0.5, 0.5, 0.5};

        for (int i = 0; i < sizeof(LightCubes) / sizeof(LightCube); i++) {
            LightCubes[i].Id = "Point Light " + std::to_string(i);
        }

        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMapTexture);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenVertexArrays(1, &ScreenVAO);
        glGenBuffers(1, &ScreenVBO);
        glBindVertexArray(ScreenVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ScreenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenVertices), &ScreenVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    }

    void RenderScene(float deltaTime, [[maybe_unused]]float currentTime, Graphics::Shader &shader) {
        shader.Use();

        shader.SetTexture("material.texture_diffuse1", WoodFloorTexture);
        shader.SetFloat("material.shininess", 2.0f);
        Floor.Update(deltaTime);
        Floor.Render(shader);

        shader.SetTexture("material.texture_diffuse1", WoodFloorTexture);
        shader.SetFloat("material.shininess", 2.0f);
        for (auto &cube: Cubes) {
            cube.Update(deltaTime);
            cube.Render(shader);
        }
    }

    void Show(const float deltaTime, [[maybe_unused]] const float currentTime, Camera &camera) {
        DirectionalLight.UIRender();

        // 1. first render to depth map
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float near_plane = 1.0f, far_plane = 50;
        glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt({-2.0f, 15.0f, -1.0f}, glm::vec3(0),
                                          glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        DepthShader->Use();
        DepthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        RenderScene(deltaTime, currentTime, *DepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

// 2. then render scene as normal with shadow mapping (using depth map)
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DebugQuadShader->Use();
        DebugQuadShader->SetFloat("near_plane", near_plane);
        DebugQuadShader->SetFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE1);
        DebugQuadShader->SetInt("depthMap", 1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glBindVertexArray(ScreenVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
//        glClear(GL_COLOR_BUFFER_BIT);

        LitShader->Use();
        LitShader->SetVec3("cameraPos", camera.Position);

        LitShader->SetVec3("dirLight.direction", DirectionalLight.Direction);
        LitShader->SetVec3("dirLight.ambient", DirectionalLight.Ambient);
        LitShader->SetVec3("dirLight.diffuse", DirectionalLight.Diffuse);
        LitShader->SetVec3("dirLight.specular", DirectionalLight.Specular);
        LitShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

//        LitShader->SetInt("pointLightCount", sizeof(LightCubes) / sizeof(LightCube));
//
//        float yoffset = glm::sin(currentTime) * 0.1;
//        for (int i = 0; i < sizeof(LightCubes) / sizeof(LightCube); i++) {
//            LightCubes[i].UIRender();
//
//            LightCubes[i].Position.y += yoffset;
//            LightCubes[i].Update(deltaTime);
//            LightCubes[i].Render();
//
//            std::ostringstream name;
//            name << "pointLights[" << i << "].";
//            auto nameStr = name.str();
//
//            LitShader->Use();
//            LitShader->SetVec3(nameStr + "position", LightCubes[i].Position);
//            LitShader->SetVec3(nameStr + "ambient", LightCubes[i].Ambient);
//            LitShader->SetVec3(nameStr + "diffuse", LightCubes[i].Diffuse);
//            LitShader->SetVec3(nameStr + "specular", LightCubes[i].Specular);
//            LitShader->SetFloat(nameStr + "constant", LightCubes[i].Constant);
//            LitShader->SetFloat(nameStr + "linear", LightCubes[i].Linear);
//            LitShader->SetFloat(nameStr + "quadratic", LightCubes[i].Quadratic);
//        }

//        LitShader->SetVec3("spotLight.position", camera.Position);
//        LitShader->SetVec3("spotLight.direction", camera.Front);
//        LitShader->SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//        LitShader->SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
//        LitShader->SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//        LitShader->SetFloat("spotLight.constant", 1.0f);
//        LitShader->SetFloat("spotLight.linear", 0.09f);
//        LitShader->SetFloat("spotLight.quadratic", 0.032f);
//        LitShader->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
//        LitShader->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        glActiveTexture(GL_TEXTURE1);
        LitShader->SetInt("shadowMap", 1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        RenderScene(deltaTime, currentTime, *LitShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
