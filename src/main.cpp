#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Log.hpp"
#include "Triangle.hpp"
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

    return window;
}

int main() {

    auto window = CreateWindow();

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle::Vertices), Triangle::Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle::Indices), Triangle::Indices, GL_STATIC_DRAW);

    Shader shader("VertexShader.vert", "FragmentShader.frag");
    Texture brickTex("BrickTexture.jpg", GL_RGB, GL_TEXTURE0);

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
        glClear(GL_COLOR_BUFFER_BIT);

        brickTex.ActivateAndBind();
        shader.Use();

        auto transform = glm::translate(glm::mat4(1), glm::vec3(0.5, 0, 0));
        transform = glm::rotate(transform, (float) glfwGetTime(), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, glm::vec3(0.5, 0.5, 0.5));

        shader.SetMat4("uTransform", transform);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


        glfwPollEvents();
        glfwSwapBuffers(window.get());
    }

    exit(0);
}