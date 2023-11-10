#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "LightCube.hpp"
#include "Graphics/Shader.hpp"
#include "imgui.h"
#include "glm/glm.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.hpp"
#include "Graphics/Model.hpp"
#include "Core/DirectionalLight.hpp"

#include <cmath>
#include <memory>
#include <sstream>

constexpr int WINDOW_WIDTH = 2560, WINDOW_HEIGHT = 1440;
auto MainCamera = Camera(glm::vec3(0, 0, -20));

void GLFWWindowDeleter(GLFWwindow* window) {
    glfwDestroyWindow(window);
}

void FramebufferSizeCallback([[maybe_unused]] GLFWwindow* windowPtr, const int width, const int height) {
    glViewport(0, 0, width, height);
}

void HandleInputCallback(GLFWwindow* windowPtr, const int key, [[maybe_unused]] int scanCode, const int action,
                         [[maybe_unused]] int mods) {
    if (const auto isKeyPRelease = key == GLFW_KEY_P && action == GLFW_RELEASE;
        isKeyPRelease && glfwGetInputMode(windowPtr, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else if (isKeyPRelease && glfwGetInputMode(windowPtr, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwGetKey(windowPtr, GLFW_KEY_R) == GLFW_RELEASE)
        MainCamera.LockRotation = !MainCamera.LockRotation;
}

std::shared_ptr<GLFWwindow> CreateWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* windowPtr = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Caruti Engine",
        nullptr,
        nullptr
    );

    if (windowPtr == nullptr) {
        Log::Error("Failed to create GLFW window");
        glfwTerminate();
        exit(-1);
    }

    std::shared_ptr<GLFWwindow> window(windowPtr, GLFWWindowDeleter);
    glfwMakeContextCurrent(window.get());
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        Log::Error("Failed to init GLAD");
        exit(-1);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window.get(), FramebufferSizeCallback);
    glfwSetKeyCallback(window.get(), HandleInputCallback);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO&io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    io.FontGlobalScale = 1.50f;

    float xscale, yscale;
    glfwGetWindowContentScale(window.get(), &xscale, &yscale);
    io.DisplayFramebufferScale = ImVec2(xscale, yscale);

    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init();

    return window;
}

void HandleInput(const std::shared_ptr<GLFWwindow>&window, Camera&camera, const float deltaTime) {
    if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE)) {
        GLFWWindowDeleter(window.get());
        exit(0);
    }

    if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Forward, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Backward, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Left, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Right, deltaTime);

    double xPos, yPos;
    glfwGetCursorPos(window.get(), &xPos, &yPos);
    camera.ProcessMouseMovement(xPos, yPos);
}

int main() {
    const auto window = CreateWindow();
    float lastTime = glfwGetTime();
    Core::DirectionalLight directionalLight;

    auto lightSourceShader = std::make_shared<Graphics::Shader>("VertexShader.vert", "LightSourceShader.frag");
    auto litShader = std::make_shared<Graphics::Shader>("VertexShader.vert", "LightingShader.frag");

    LightCube lightCubes[] = {
        LightCube(lightSourceShader, glm::vec3(-50, 50, -50), glm::vec3(0), glm::vec3(30)),
        LightCube(lightSourceShader, glm::vec3(-40, 50, 50), glm::vec3(0), glm::vec3(30)),
        LightCube(lightSourceShader, glm::vec3(55, 50, -50), glm::vec3(0), glm::vec3(30)),
        LightCube(lightSourceShader, glm::vec3(55, 50, 50), glm::vec3(0), glm::vec3(30))
    };
    Graphics::Model sponza("resources/models/sponza/sponza.obj");

    float amplitude = 3.6;
    float freq = 0.05;

    for (int i = 0; i < sizeof(lightCubes) / sizeof(LightCube); i++) {
        lightCubes[i].Id = "Point Light " + std::to_string(i);
    }

    // unsigned int depthMapFBO;
    // glGenBuffers(1, &depthMapFBO);
    //
    // constexpr unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    // unsigned int depthMap;
    // glGenTextures(1, &depthMap);
    // glBindTexture(GL_TEXTURE_2D, depthMap);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
    //              nullptr);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //
    // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window.get())) {
        const float currentTime = glfwGetTime();
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        directionalLight.UIRender();
        for (auto&lightCube: lightCubes) {
            lightCube.UIRender();
        }

        if (!ImGui::Begin("Scene Settings")) {
            ImGui::End();
        }

        ImGui::InputFloat("Amplitude", &amplitude);
        ImGui::InputFloat("Frequency", &freq);

        ImGui::End();

        HandleInput(window, MainCamera, deltaTime);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        litShader->Use();
        litShader->SetVec3("cameraPos", MainCamera.Position);
        litShader->SetFloat("material.shininess", 32.0f);

        litShader->SetVec3("dirLight.direction", directionalLight.Direction);
        litShader->SetVec3("dirLight.ambient", directionalLight.Ambient);
        litShader->SetVec3("dirLight.diffuse", directionalLight.Diffuse);
        litShader->SetVec3("dirLight.specular", directionalLight.Specular);

        const auto offset = glm::sin(2 * glm::pi<float>() * freq * currentTime) * amplitude;
        for (int i = 0; i < sizeof(lightCubes) / sizeof(LightCube); i++) {
            lightCubes[i].Position.x += offset;
            lightCubes[i].Update(deltaTime);
            lightCubes[i].Render(MainCamera.GetCameraMatrix());

            std::ostringstream name;
            name << "pointLights[" << i << "].";
            auto nameStr = name.str();

            litShader->Use();
            litShader->SetVec3(nameStr + "position", lightCubes[i].Position);
            litShader->SetVec3(nameStr + "ambient", lightCubes[i].Ambient);
            litShader->SetVec3(nameStr + "diffuse", lightCubes[i].Diffuse);
            litShader->SetVec3(nameStr + "specular", lightCubes[i].Specular);
            litShader->SetFloat(nameStr + "constant", lightCubes[i].Constant);
            litShader->SetFloat(nameStr + "linear", lightCubes[i].Linear);
            litShader->SetFloat(nameStr + "quadratic", lightCubes[i].Quadratic);
        }

        litShader->SetVec3("spotLight.position", MainCamera.Position);
        litShader->SetVec3("spotLight.direction", MainCamera.Front);
        litShader->SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        litShader->SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        litShader->SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        litShader->SetFloat("spotLight.constant", 1.0f);
        litShader->SetFloat("spotLight.linear", 0.09f);
        litShader->SetFloat("spotLight.quadratic", 0.032f);
        litShader->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        litShader->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        constexpr glm::mat4 model = glm::mat4(1.0f);
        litShader->SetMat4("model", model);
        litShader->SetMat4("camera", MainCamera.GetCameraMatrix());
        sponza.Draw(*litShader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(window.get());
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    exit(0);
}
