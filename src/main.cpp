#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "Cube.hpp"
#include "Graphics/Shader.hpp"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.hpp"

#include <memory>

const int WINDOW_WIDTH = 2560, WINDOW_HEIGHT = 1440;
auto MainCamera = Camera(glm::vec3(0, 0, -20));

void GLFWWindowDeleter(GLFWwindow *window) {
    glfwDestroyWindow(window);
}

void FramebufferSizeCallback(GLFWwindow *windowPtr, int width, int height) {
    glViewport(0, 0, width, height);
}

void HandleInputCallback(GLFWwindow *windowPtr, int key, [[maybe_unused]] int scanCode, int action,
                         [[maybe_unused]] int mods) {

    auto isKeyPRelease = key == GLFW_KEY_P && action == GLFW_RELEASE;
    if (isKeyPRelease && glfwGetInputMode(windowPtr, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
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
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        Log::Error("Failed to init GLAD");
        exit(-1);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window.get(), FramebufferSizeCallback);
    glfwSetKeyCallback(window.get(), HandleInputCallback);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

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

void HandleInput(const std::shared_ptr<GLFWwindow> &window, Camera &camera, float deltaTime) {
    if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE)) {
        GLFWWindowDeleter(window.get());
        exit(0);
    }

    if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::Forward, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::Backward, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::Left, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::Right, deltaTime);

    double xPos, yPos;
    glfwGetCursorPos(window.get(), &xPos, &yPos);
    camera.ProcessMouseMovement(xPos, yPos);

}

int main() {

    auto window = CreateWindow();
    float lastTime = glfwGetTime();
    float deltaTime;

//    auto lightSourceShader = std::make_shared<Graphics::Shader>("VertexShader.vert", "LightSourceShader.frag");
//    Cube lightSourceCube(lightSourceShader, glm::vec3(2, 0, -5));

    auto lightReceiveShader = std::make_shared<Graphics::Shader>("VertexShader.vert", "LightingShader.frag");

    lightReceiveShader->Use();
    Graphics::Texture crateTex("crate/Crate_Diffuse.jpg", GL_RGB, GL_TEXTURE0);
    lightReceiveShader->SetTexture("material.diffuse", crateTex);

    Graphics::Texture crateSpecularTex("crate/Crate_SpecularMap.png", GL_RGBA, GL_TEXTURE1);
    lightReceiveShader->SetTexture("material.specular", crateSpecularTex);

    Cube cubes[] = {
            Cube(lightReceiveShader, glm::vec3(-1.7f, 2.0f, -7.5f)),
            Cube(lightReceiveShader, glm::vec3(0, 0, 0)),
            Cube(lightReceiveShader, glm::vec3(0, -4, 0)),
            Cube(lightReceiveShader, glm::vec3(0, 4, 0)),
            Cube(lightReceiveShader, glm::vec3(0, 6, 0)),
            Cube(lightReceiveShader, glm::vec3(0, -6, 0)),
            Cube(lightReceiveShader, glm::vec3(2, 2, 2)),
            Cube(lightReceiveShader, glm::vec3(-2, -2, -2)),
    };

    static float lightAmbient[3] = {0.1, 0.1, 0.1};
    static float lightDiffuse[3] = {1, 1, 1};
    static float lightSpecular[3] = {1.0, 1.0, 1.0};

    while (!glfwWindowShouldClose(window.get())) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
//      ImGui::ShowDemoWindow();

//        ImGui::SeparatorText("Light options");
//        ImGui::InputFloat3("Ambient", lightAmbient);
//        ImGui::InputFloat3("Diffuse", lightDiffuse);
//        ImGui::InputFloat3("Specular", lightSpecular);

        HandleInput(window, MainCamera, deltaTime);

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        lightSourceCube.Position.x = std::sin((float) glfwGetTime()) * 5;
//        lightSourceCube.Position.y = std::sin((float) glfwGetTime()) * 1;
//        lightSourceCube.Position.z = std::cos((float) glfwGetTime()) * 5;
//        lightSourceCube.Update(deltaTime);
//        lightSourceCube.Render(MainCamera.GetCameraMatrix());

        lightReceiveShader->Use();
        lightReceiveShader->SetFloat("material.shininess", 64.0f);
        lightReceiveShader->SetVec3("light.ambient", lightAmbient[0], lightAmbient[1], lightAmbient[2]);
        lightReceiveShader->SetVec3("light.diffuse", lightDiffuse[0], lightDiffuse[1], lightDiffuse[2]);
        lightReceiveShader->SetVec3("light.specular", lightSpecular[0], lightSpecular[1], lightSpecular[2]);
        lightReceiveShader->SetVec4("light.vector", glm::sin(glfwGetTime()), -1, glm::cos(glfwGetTime()), 0.0);
//        lightReceiveShader->SetVec3("light.position", lightSourceCube.Position);

        lightReceiveShader->SetVec3("cameraPos", MainCamera.Position);

        for (int i = 0; i < sizeof(cubes) / sizeof(Cube); ++i) {
            cubes[i].Position.x = glm::sin(2 * glm::pi<float>() * 0.05 * glfwGetTime() + i) * 3;
            cubes[i].Update(deltaTime);
            cubes[i].Render(MainCamera.GetCameraMatrix());
        }

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