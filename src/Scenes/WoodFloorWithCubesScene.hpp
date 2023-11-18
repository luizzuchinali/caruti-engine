#pragma once

#include "LightCube.hpp"
#include "Core/DirectionalLight.hpp"
#include "Floor.hpp"
#include "Camera.hpp"


#include <sstream>
#include <memory>
#include <random>

class WoodFloorWithCubesScene {
public:
    Core::DirectionalLight DirectionalLight;
    std::shared_ptr<Graphics::Shader> LitShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert",
            "LitShader.frag"
    );

    Floor Floor;
    Graphics::Texture WoodFloorTexture = Graphics::Texture(
            "resources/textures/wood_floor_deck/wood_floor_deck_diff.jpg",
            GL_TEXTURE0
    );

    std::shared_ptr<Graphics::Shader> LightSourceShader = std::make_shared<Graphics::Shader>(
            "VertexShader.vert", "LightSourceShader.frag"
    );
    LightCube LightCubes[1] = {
            LightCube(LightSourceShader, {5, 2, 0})
    };

    WoodFloorWithCubesScene() : Floor({-20, 0, -20}) {
        DirectionalLight.Ambient = {0, 0, 0};
        DirectionalLight.Diffuse = {0, 0, 0};
        DirectionalLight.Specular = {0, 0, 0};

        for (int i = 0; i < sizeof(LightCubes) / sizeof(LightCube); i++) {
            LightCubes[i].Id = "Point Light " + std::to_string(i);
        }
    }

    void Show(const float deltaTime, [[maybe_unused]] const float currentTime, Camera &camera) {
        DirectionalLight.UIRender();

        LitShader->Use();
        LitShader->SetVec3("cameraPos", camera.Position);

        LitShader->SetVec3("dirLight.direction", DirectionalLight.Direction);
        LitShader->SetVec3("dirLight.ambient", DirectionalLight.Ambient);
        LitShader->SetVec3("dirLight.diffuse", DirectionalLight.Diffuse);
        LitShader->SetVec3("dirLight.specular", DirectionalLight.Specular);

        LitShader->SetInt("pointLightCount", sizeof(LightCubes) / sizeof(LightCube));

        float yoffset = glm::sin(currentTime) * 0.1;
        for (int i = 0; i < sizeof(LightCubes) / sizeof(LightCube); i++) {
            LightCubes[i].UIRender();

            LightCubes[i].Position.y += yoffset;
            LightCubes[i].Update(deltaTime);
            LightCubes[i].Render(*LightSourceShader);

            std::ostringstream name;
            name << "pointLights[" << i << "].";
            auto nameStr = name.str();

            LitShader->Use();
            LitShader->SetVec3(nameStr + "position", LightCubes[i].Position);
            LitShader->SetVec3(nameStr + "ambient", LightCubes[i].Ambient);
            LitShader->SetVec3(nameStr + "diffuse", LightCubes[i].Diffuse);
            LitShader->SetVec3(nameStr + "specular", LightCubes[i].Specular);
            LitShader->SetFloat(nameStr + "constant", LightCubes[i].Constant);
            LitShader->SetFloat(nameStr + "linear", LightCubes[i].Linear);
            LitShader->SetFloat(nameStr + "quadratic", LightCubes[i].Quadratic);
        }

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


        LitShader->SetTexture("material.texture_diffuse1", WoodFloorTexture);
        LitShader->SetFloat("material.shininess", 2.0f);
        Floor.Update(deltaTime);
        Floor.Render(*LitShader);
    }
};
