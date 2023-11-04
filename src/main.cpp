#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "Cube.hpp"
#include "Graphics/Shader.hpp"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.hpp"
#include "Graphics/Model.hpp"

#include <cmath>
#include <memory>
#include <sstream>

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

//    auto backpackShader = Graphics::Shader("ModelLoading.vert", "ModelLoading.frag");
//    Graphics::Model backpack("resources/models/backpack/backpack.obj");

//    auto lightSourceShader = std::make_shared<Graphics::Shader>("VertexShader.vert", "LightSourceShader.frag");
//    lightSourceShader->Use();
//    Cube pointLights[] = {
//            Cube(lightSourceShader, glm::vec3(0, 1.5, 0)),
//            Cube(lightSourceShader, glm::vec3(0, 5, 5)),
//            Cube(lightSourceShader, glm::vec3(3, 3, -5)),
//            Cube(lightSourceShader, glm::vec3(0, -5, -3))
//    };

    auto litShader = std::make_shared<Graphics::Shader>("VertexShader.vert", "LightingShader.frag");
    Graphics::Model sponza("resources/models/sponza/sponza.obj");
    litShader->Use();

    while (!glfwWindowShouldClose(window.get())) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
//      ImGui::ShowDemoWindow();

        HandleInput(window, MainCamera, deltaTime);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        litShader->Use();
        litShader->SetVec3("cameraPos", MainCamera.Position);
        litShader->SetFloat("material.shininess", 32.0f);

        litShader->SetVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        litShader->SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        litShader->SetVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        litShader->SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

//        for (int i = 0; i < sizeof(pointLights) / sizeof(Cube); i++) {
//            pointLights[i].Update(deltaTime);
//            pointLights[i].Render(MainCamera.GetCameraMatrix());
//
//            std::ostringstream name;
//            name << "pointLights[" << i << "].";
//            auto nameStr = name.str();
//
//            litShader->Use();
//            litShader->SetVec3(nameStr + "position", pointLights[0].Position);
//            litShader->SetVec3(nameStr + "ambient", 0.05f, 0.05f, 0.05f);
//            litShader->SetVec3(nameStr + "diffuse", 0.8f, 0.8f, 0.8f);
//            litShader->SetVec3(nameStr + "specular", 1.0f, 1.0f, 1.0f);
//            litShader->SetFloat(nameStr + "constant", 1.0f);
//            litShader->SetFloat(nameStr + "linear", 0.09f);
//            litShader->SetFloat(nameStr + "quadratic", 0.032f);
//        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        model = glm::translate(model, glm::vec3(0, 0, 0));
        litShader->SetMat4("model", model);
        litShader->SetMat4("camera", MainCamera.GetCameraMatrix());
        sponza.Draw(*litShader);

//        litShader->SetVec3("spotLight.position", MainCamera.Position);
//        litShader->SetVec3("spotLight.direction", MainCamera.Front);
//        litShader->SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//        litShader->SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
//        litShader->SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//        litShader->SetFloat("spotLight.constant", 1.0f);
//        litShader->SetFloat("spotLight.linear", 0.09f);
//        litShader->SetFloat("spotLight.quadratic", 0.032f);
//        litShader->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
//        litShader->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

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