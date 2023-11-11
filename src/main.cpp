#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "Graphics/Shader.hpp"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.hpp"
#include "Core/DirectionalLight.hpp"
#include "Plane.hpp"

#include <memory>
#include <vector>
#include <sstream>
#include <random>

constexpr int WINDOW_WIDTH = 2560, WINDOW_HEIGHT = 1440;
auto MainCamera = Camera(glm::vec3(0, 15, -15));

void GLFWWindowDeleter(GLFWwindow *window) {
    glfwDestroyWindow(window);
}

void FramebufferSizeCallback([[maybe_unused]] GLFWwindow *windowPtr, const int width, const int height) {
    glViewport(0, 0, width, height);
}

void HandleInputCallback(GLFWwindow *windowPtr, const int key, [[maybe_unused]] int scanCode, const int action,
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

    GLFWwindow *windowPtr = glfwCreateWindow(
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
    glEnable(GL_BLEND);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
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

void HandleInput(const std::shared_ptr<GLFWwindow> &window, Camera &camera, const float deltaTime) {
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
    directionalLight.Ambient = glm::vec3(0.6, 0.6, 0.6);
    // SponzaScene sponzaScene{};

    auto litShader = std::make_shared<Graphics::Shader>("VertexShader.vert", "LightingShader.frag");
    litShader->Use();

    auto terrainGrassTexture = Graphics::Texture("TerrainGrassTexture.jpg", GL_RGB, GL_TEXTURE0);
    auto grassTexture = Graphics::Texture("grass.png", GL_RGBA, GL_TEXTURE1, GL_CLAMP_TO_EDGE);

    Plane plane(litShader);

    float vegetationVertices[] = {
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.0f, -0.5f, 0.0f, 0.0f, 0.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f,

            0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f,
            1.0f, 0.5f, 0.0f, 1.0f, 1.0f
    };

    unsigned int vegetationVAO, vegetationVBO;
    glGenVertexArrays(1, &vegetationVAO);
    glBindVertexArray(vegetationVAO);

    glGenBuffers(1, &vegetationVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vegetationVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vegetationVertices), &vegetationVertices[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    std::vector<glm::vec3> vegetation;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(-5.3f, 4.3f);
    std::uniform_real_distribution<float> disZ(-5.0f, 5.0f);

    for (int i = 0; i < 2000; i++) {
        float randomX = disX(gen);
        float randomZ = disZ(gen);

        vegetation.emplace_back(randomX, 0.0f, randomZ);
    }

    while (!glfwWindowShouldClose(window.get())) {
        const float currentTime = glfwGetTime();
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        directionalLight.UIRender();

        HandleInput(window, MainCamera, deltaTime);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // sponzaScene.Show(deltaTime, currentTime, MainCamera);

        litShader->Use();
        litShader->SetVec3("cameraPos", MainCamera.Position);
        litShader->SetFloat("material.shininess", 32.0f);

        litShader->SetVec3("dirLight.direction", directionalLight.Direction);
        litShader->SetVec3("dirLight.ambient", directionalLight.Ambient);
        litShader->SetVec3("dirLight.diffuse", directionalLight.Diffuse);
        litShader->SetVec3("dirLight.specular", directionalLight.Specular);

        litShader->SetTexture("material.texture_diffuse1", terrainGrassTexture);
        plane.Update(deltaTime);
        plane.Render(MainCamera.GetCameraMatrix());

        glBindVertexArray(vegetationVAO);
        litShader->SetTexture("material.texture_diffuse1", grassTexture);
        for (auto i: vegetation) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, i);
            litShader->SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);

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
