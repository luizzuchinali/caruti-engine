#pragma once

#include "LightCube.hpp"
#include "Camera.hpp"
#include "Graphics/Model.hpp"
#include "Core/DirectionalLight.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <sstream>

class SponzaDirLightShadowScene {
public:
    Core::DirectionalLight DirectionalLight{};

    std::shared_ptr<Graphics::Shader> LightSourceShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert", "LightSourceShader.frag");
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert", "LitShader.frag");


    Graphics::Model Sponza = Graphics::Model("resources/models/sponza/sponza.obj");

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
    std::shared_ptr<Graphics::Shader> DebugQuadShader = std::make_shared<Graphics::Shader>(
            "DebugQuad.vert",
            "DebugQuad.frag"
    );
    Graphics::Model SunModel = Graphics::Model("resources/models/Sun.glb");

    SponzaDirLightShadowScene() {
        DirectionalLight.Ambient = {0.1, 0.1, 0.1};
        DirectionalLight.Diffuse = {0.7, 0.7, 0.7};
        DirectionalLight.Specular = {0.5, 0.5, 0.5};

        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMapTexture);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

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

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f));
        shader.SetMat4("model", model);
        shader.SetFloat("material.shininess", 2.0f);
        Sponza.Draw(*LitShader);
    }


    float near_plane = 1.0f, far_plane = 100, directionScalar = 30;
    glm::vec3 eyePositionOffset = glm::vec3(40);

    void Show(const float deltaTime, const float currentTime, Camera &camera) {
        DirectionalLight.UIRender();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        float x = glm::sin(currentTime * 0.5);
//        float z = glm::cos(currentTime * 0.5);
//        DirectionalLight.Direction.x = x;
//        DirectionalLight.Direction.z = z;

        ImGui::Begin("Light Settings");

        ImGui::SliderFloat("Near Plane", &near_plane, 0.1f, 10.0f);
        ImGui::SliderFloat("Far Plane", &far_plane, 50.0f, 5000.0f);
        ImGui::SliderFloat("Direction Scalar", &directionScalar, -3000.0f, 5000.0f);

        ImGui::SliderFloat3("Eye Position", glm::value_ptr(eyePositionOffset), 0, 1000.0f);
        ImGui::End();

        glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
        glm::vec3 lightDirectionOffset = -DirectionalLight.Direction * glm::vec3(directionScalar);
        glm::vec3 eyePosition = lightDirectionOffset + eyePositionOffset;

        glm::mat4 lightView = glm::lookAt(eyePosition, glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        DepthShader->Use();
        DepthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        RenderScene(deltaTime, currentTime, *DepthShader);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, eyePosition);
        LightSourceShader->Use();
        LightSourceShader->SetMat4("model", model);
        SunModel.Draw(*LightSourceShader);


        LitShader->Use();
        LitShader->SetVec3("cameraPos", camera.Position);
        LitShader->SetFloat("material.shininess", 32.0f);

        LitShader->SetVec3("dirLight.direction", DirectionalLight.Direction);
        LitShader->SetVec3("dirLight.ambient", DirectionalLight.Ambient);
        LitShader->SetVec3("dirLight.diffuse", DirectionalLight.Diffuse);
        LitShader->SetVec3("dirLight.specular", DirectionalLight.Specular);
        LitShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE1);
        LitShader->SetInt("shadowMap", 1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        RenderScene(deltaTime, currentTime, *LitShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
