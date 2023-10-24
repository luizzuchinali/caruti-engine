#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "Box.hpp"
#include "Graphics/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.hpp"

#include <memory>

using namespace Graphics;

const int WINDOW_WIDTH = 2560, WINDOW_HEIGHT = 1440;
auto MainCamera = Camera(glm::vec3(0, 0, -5));

void GLFWWindowDeleter(GLFWwindow *window) {
    glfwDestroyWindow(window);
}

void FramebufferSizeCallback(GLFWwindow *windowPtr, int width, int height) {
    glViewport(0, 0, width, height);
}

std::shared_ptr<GLFWwindow> CreateWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glEnable(GL_DEPTH_TEST);

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
    auto pKeyPress = glfwGetKey(window.get(), GLFW_KEY_P);
    if (pKeyPress && glfwGetInputMode(window.get(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else if (pKeyPress && glfwGetInputMode(window.get(), GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
        glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE)) {
        GLFWWindowDeleter(window.get());
        exit(0);
    }

    if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window.get(), GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(ECameraMovement::RIGHT, deltaTime);

    double xPos, yPos;
    glfwGetCursorPos(window.get(), &xPos, &yPos);
    camera.ProcessMouseMovement(xPos, yPos);
}

int main() {

    auto window = CreateWindow();

    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Box::Vertices), Box::Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader shader("VertexShader.vert", "FragmentShader.frag");
    Texture brickTex("crate2_diffuse.png", GL_RGBA, GL_TEXTURE0);

    shader.Use();
    shader.SetTexture("tex", brickTex);
    auto projection = glm::perspective(
            glm::radians(45.0f),
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
            0.1f,
            100.0f
    );

    shader.SetMat4("projection", projection);

    glBindVertexArray(0);

    glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    float lastTime = glfwGetTime();
    float deltaTime;

    while (!glfwWindowShouldClose(window.get())) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        HandleInput(window, MainCamera, deltaTime);

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        brickTex.ActivateAndBind();
        shader.Use();

        shader.SetMat4("view", MainCamera.GetViewMatrix());

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, currentTime + (float) i, glm::vec3(1.0f, 0.3f, 0.5f));
            shader.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
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