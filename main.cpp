#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"

// ---- globals used by GLFW callbacks ----------------------------------------

static Camera *g_camera    = nullptr;
static bool    g_firstMouse = true;
static float   g_lastX      = 640.0f;
static float   g_lastY      = 360.0f;

// ---- GLFW callbacks --------------------------------------------------------

static void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_P && action == GLFW_PRESS && g_camera) {
        glm::vec3 p = g_camera->getPosition();
        std::cout << "Camera position: ("
                  << p.x << ", " << p.y << ", " << p.z << ")\n";
    }
}

static void mouseCallback(GLFWwindow * /*window*/, double xpos, double ypos) {
    if (!g_camera) return;

    float fx = static_cast<float>(xpos);
    float fy = static_cast<float>(ypos);

    if (g_firstMouse) {
        g_lastX = fx;
        g_lastY = fy;
        g_firstMouse = false;
    }

    float xoffset =  (fx - g_lastX);
    float yoffset = -(fy - g_lastY);   // invert Y: screen-down = pitch-down
    g_lastX = fx;
    g_lastY = fy;

    g_camera->processMouseMovement(xoffset, yoffset);
}

static void scrollCallback(GLFWwindow * /*window*/, double /*xoffset*/, double yoffset) {
    if (g_camera)
        g_camera->processMouseScroll(static_cast<float>(yoffset));
}

static void framebufferSizeCallback(GLFWwindow * /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
}

// ---- main ------------------------------------------------------------------

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
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialise GLEW\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    // Camera starting at (0, 15, 30) looking toward origin
    Camera camera(glm::vec3(0.0f, 15.0f, 30.0f));
    g_camera = &camera;

    Shader shader("shaders/main.vert", "shaders/main.frag");
    Mesh   box = Mesh::createBox(4.0f, 0.5f, 8.0f);

    glm::mat4 model     = glm::mat4(1.0f);
    glm::vec3 lightPos(10.0f, 20.0f, 10.0f);
    glm::vec3 greenColor(0.13f, 0.55f, 0.13f);

    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float now       = static_cast<float>(glfwGetTime());
        float deltaTime = now - lastTime;
        lastTime        = now;

        camera.processKeyboard(window, deltaTime);

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        float aspect = (fbH > 0) ? static_cast<float>(fbW) / fbH : 1.0f;

        glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("model",      model);
        shader.setMat4("view",       camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix(aspect));
        shader.setVec3("objectColor", greenColor);
        shader.setVec3("lightPos",   lightPos);
        shader.setVec3("viewPos",    camera.getPosition());

        box.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_camera = nullptr;
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
