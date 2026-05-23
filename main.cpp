#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Water.h"
#include "Objects.h"
#include "HoleConfig.h"
#include "HoleFactory.h"
#include "CourseData.h"

// ---- globals ---------------------------------------------------------------

static Camera *g_camera      = nullptr;
static bool    g_firstMouse  = true;
static float   g_lastX       = 640.0f;
static float   g_lastY       = 360.0f;
static bool    g_isNight     = false;
static bool    g_nWasPressed = false;

// ---- GLFW callbacks --------------------------------------------------------

static void keyCallback(GLFWwindow *window, int key, int /*sc*/, int action, int /*mod*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_P && action == GLFW_PRESS && g_camera) {
        glm::vec3 p = g_camera->getPosition();
        std::cout << "Camera: (" << p.x << ", " << p.y << ", " << p.z << ")\n";
    }
}

static void mouseCallback(GLFWwindow * /*w*/, double xpos, double ypos) {
    if (!g_camera) return;
    float fx = static_cast<float>(xpos);
    float fy = static_cast<float>(ypos);
    if (g_firstMouse) { g_lastX = fx; g_lastY = fy; g_firstMouse = false; return; }
    g_camera->processMouseMovement(fx - g_lastX, -(fy - g_lastY));
    g_lastX = fx; g_lastY = fy;
}

static void scrollCallback(GLFWwindow * /*w*/, double /*x*/, double y) {
    if (g_camera) g_camera->processMouseScroll(static_cast<float>(y));
}

static void framebufferSizeCallback(GLFWwindow * /*w*/, int w, int h) {
    glViewport(0, 0, w, h);
}

// ---- main ------------------------------------------------------------------

int main() {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return EXIT_FAILURE; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "The Vines Mini Golf", nullptr, nullptr);
    if (!window) { std::cerr << "Window creation failed\n"; glfwTerminate(); return EXIT_FAILURE; }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init failed\n"; glfwTerminate(); return EXIT_FAILURE; }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    // ---- Camera ------------------------------------------------------------
    // Start high enough to see the whole course (68 × 47 m)
    Camera camera(glm::vec3(0.0f, 40.0f, 50.0f));
    g_camera = &camera;

    // ---- Shaders -----------------------------------------------------------
    Shader mainShader ("shaders/main.vert",   "shaders/main.frag");
    Shader skyShader  ("shaders/skybox.vert", "shaders/skybox.frag");
    Shader waterShader("shaders/water.vert",  "shaders/water.frag");

    // ---- Persistent scene objects ------------------------------------------
    Skybox skybox;

    // Course-wide stream running along the east edge
    Water courseStream(4.0f, 47.0f, glm::vec3(32.0f, 0.0f, 0.0f));

    // ---- Build all 18 holes ------------------------------------------------
    std::vector<HoleConfig> configs = buildCourseData();
    std::vector<HoleNode>   holes;
    holes.reserve(configs.size());
    for (const auto &cfg : configs)
        holes.push_back(HoleFactory::build(cfg));

    // ---- Test objects near hole 1 (position from CourseData row1 col0) -----
    Rock      rock1(glm::vec3(1.2f, 0.9f, 1.4f));
    Barrel    barrel1(false);
    Billboard sign1("textures/billboard.png", camera.getPosition());

    glm::vec3 hole1World = configs[0].position;

    // ---- Light / misc constants --------------------------------------------
    glm::vec3 lightPos(0.0f, 50.0f, 0.0f);   // overhead sun
    float lastTime = static_cast<float>(glfwGetTime());

    // ---- Render loop -------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float now  = static_cast<float>(glfwGetTime());
        float dt   = now - lastTime;
        lastTime   = now;

        camera.processKeyboard(window, dt);
        sign1.updateCamera(camera.getPosition());

        // N — day/night toggle
        bool nDown = (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS);
        if (nDown && !g_nWasPressed) { g_isNight = !g_isNight; skybox.setNight(g_isNight); }
        g_nWasPressed = nDown;

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        float aspect = (fbH > 0) ? static_cast<float>(fbW) / fbH : 1.0f;

        glm::mat4 view       = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspect);
        glm::vec3 viewPos    = camera.getPosition();

        // Adjust ambient for night mode
        glm::vec3 activeLightPos = g_isNight
            ? camera.getSpotlightPosition()
            : lightPos;

        glClearColor(0.05f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // -- Skybox ----------------------------------------------------------
        glDepthMask(GL_FALSE);
        skybox.draw(skyShader, view, projection);
        glDepthMask(GL_TRUE);

        // -- All 18 holes (terrain + water + bridge) -------------------------
        mainShader.use();
        mainShader.setMat4("view",       view);
        mainShader.setMat4("projection", projection);
        mainShader.setVec3("lightPos",   activeLightPos);
        mainShader.setVec3("viewPos",    viewPos);
        mainShader.setInt ("diffuseTex", 0);

        for (auto &hole : holes)
            hole.draw(mainShader);

        // -- Test objects near hole 1 ----------------------------------------
        {
            mainShader.setVec3("objectColor", glm::vec3(0.55f, 0.50f, 0.45f));
            glm::mat4 rockM = glm::translate(glm::mat4(1.0f),
                                             hole1World + glm::vec3(-2.0f, 0.4f, -1.0f));
            rock1.draw(mainShader, rockM);

            mainShader.setVec3("objectColor", glm::vec3(0.6f, 0.4f, 0.2f));
            glm::mat4 barrelM = glm::translate(glm::mat4(1.0f),
                                               hole1World + glm::vec3(2.0f, 0.35f, 0.0f));
            barrel1.draw(mainShader, barrelM);

            mainShader.setVec3("objectColor", glm::vec3(1.0f));
            glm::mat4 signM = glm::translate(glm::mat4(1.0f),
                                             hole1World + glm::vec3(0.0f, 1.0f, -3.5f));
            signM = glm::scale(signM, glm::vec3(2.0f));
            sign1.draw(mainShader, signM);
        }

        // -- Course-wide stream (alpha-blended, drawn last) ------------------
        courseStream.draw(waterShader, now, view, projection, activeLightPos, viewPos);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_camera = nullptr;
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
