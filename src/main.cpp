#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "Types.hpp"
#include "Triangle.hpp"
#include "Graphics/Shader.hpp"
#include <iostream>
#include <memory>

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

// 1833 x 1031
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

    GLFWwindow *windowPtr = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Caruti Engine", nullptr, nullptr);
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

    const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    Log::Information("Device: {}", renderer);
    const char *version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    Log::Information("OpenGL version: {}", version);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window.get(), FramebufferSizeCallback);

    return window;
}

int main() {

    auto window = CreateWindow();

    uint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle::Vertices), Triangle::Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader shader("VertexShader.vert", "FragmentShader.frag");

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

        glClearColor(0.1f, 0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        shader.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwPollEvents();
        glfwSwapBuffers(window.get());
    }

    exit(0);
}