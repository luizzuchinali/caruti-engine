#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "Box.hpp"
#include "Graphics/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

using namespace Graphics;

const int WINDOW_WIDTH = 1920, WINDOW_HEIGHT = 1080;

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

    return window;
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

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window.get())) {
        if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE)) {
            GLFWWindowDeleter(window.get());
            exit(0);
        }

        if (glfwGetKey(window.get(), GLFW_KEY_A)) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        }

        if (glfwGetKey(window.get(), GLFW_KEY_S)) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        if (glfwGetKey(window.get(), GLFW_KEY_D)) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        brickTex.ActivateAndBind();
        shader.Use();

        auto model = glm::rotate(glm::mat4(1), (float) glfwGetTime(), glm::vec3(1, 1, 0));
        auto view = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));

        auto projection = glm::perspective(
                glm::radians(45.0f),
                (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT,
                0.1f,
                100.0f
        );

        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwPollEvents();
        glfwSwapBuffers(window.get());
    }

    exit(0);
}