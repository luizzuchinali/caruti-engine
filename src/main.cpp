#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.hpp"
#include "Core/DirectionalLight.hpp"
#include "Scenes/DenseGrassScene.hpp"
#include "Scenes/SemiTransparentTexturesScene.hpp"
#include "Scenes/FramebufferScene.hpp"
#include "Scenes/CubemapScene.hpp"
#include "Scenes/EnvironmentMappingScene.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Scenes/InstancingScene.hpp"

#include <memory>
#include <sstream>

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
//    glDepthFunc(GL_LESS);
//    glEnable(GL_MULTISAMPLE);

//    glEnable(GL_CULL_FACE);
//    glFrontFace(GL_CW);
//    //    glFrontFace(GL_CCW);
//    glCullFace(GL_BACK);

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

    // SponzaScene sponzaScene{};
    // DenseGrassScene denseGrassScene{};
    // SemiTransparentTexturesScene semiTransparentTexturesScene{};
    // FramebufferScene framebufferScene{};
    // CubeMapScene cubemapScene{};
    // EnvironmentMappingScene environmentMappingScene{};
    InstancingScene instancingScene{};

    unsigned int matricesUBO, matricesBindingPort = 0;
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, matricesBindingPort, matricesUBO);
    glBufferSubData(
            GL_UNIFORM_BUFFER,
            sizeof(glm::mat4),
            sizeof(glm::mat4),
            glm::value_ptr(Camera::GetProjectionMatrix())
    );
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    while (!glfwWindowShouldClose(window.get())) {
        const float currentTime = glfwGetTime();
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        HandleInput(window, MainCamera, deltaTime);

        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(MainCamera.GetViewMatrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // sponzaScene.Show(deltaTime, currentTime, MainCamera);
        // denseGrassScene.Show(deltaTime, currentTime, MainCamera);
        // semiTransparentTexturesScene.Show(deltaTime, currentTime, MainCamera);
        // framebufferScene.Show(deltaTime, currentTime, MainCamera);
        // cubemapScene.Show(deltaTime, currentTime, MainCamera);
        // environmentMappingScene.Show(deltaTime, currentTime, MainCamera);
        instancingScene.Show(deltaTime, currentTime, MainCamera);

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
