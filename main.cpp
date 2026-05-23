#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#include "Shader.h"
#include "Mesh.h"

static void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void framebufferSizeCallback(GLFWwindow * /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "The Vines Mini Golf", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialise GLEW\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    Shader shader("shaders/main.vert", "shaders/main.frag");
    Mesh   box = Mesh::createBox(4.0f, 0.5f, 8.0f);

    glm::mat4 model      = glm::mat4(1.0f);
    glm::mat4 view       = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f),
                                       glm::vec3(0.0f,  0.0f,  0.0f),
                                       glm::vec3(0.0f,  1.0f,  0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            1280.0f / 720.0f, 0.1f, 200.0f);

    glm::vec3 lightPos(10.0f, 20.0f, 10.0f);
    glm::vec3 viewPos(0.0f, 10.0f, 20.0f);
    glm::vec3 greenColor(0.13f, 0.55f, 0.13f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("model",      model);
        shader.setMat4("view",       view);
        shader.setMat4("projection", projection);
        shader.setVec3("objectColor", greenColor);
        shader.setVec3("lightPos",   lightPos);
        shader.setVec3("viewPos",    viewPos);

        box.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
