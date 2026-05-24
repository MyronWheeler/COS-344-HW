#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Skybox.h"
#include "Objects.h"
#include "HoleConfig.h"
#include "HoleFactory.h"
#include "CourseData.h"
#include "ShadowMap.h"
#include "Drone.h"

// ---- globals ---------------------------------------------------------------

static Camera *g_camera      = nullptr;
static bool    g_isNight     = false;
static bool    g_nWasPressed = false;

// ---- GLFW callbacks --------------------------------------------------------

static void keyCallback(GLFWwindow *window, int key, int /*sc*/, int action, int /*mod*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_P && action == GLFW_PRESS && g_camera) {
        glm::vec3 p = g_camera->getPosition();
        std::cout << "Drone position: ("
                  << p.x << ", " << p.y << ", " << p.z << ")\n";
    }
}

static void scrollCallback(GLFWwindow * /*w*/, double /*x*/, double y) {
    if (g_camera) g_camera->processMouseScroll(static_cast<float>(y));
}

static void framebufferSizeCallback(GLFWwindow * /*w*/, int w, int h) {
    glViewport(0, 0, w, h);
}

// ---- Light parameters for day / night modes --------------------------------

struct LightMode {
    glm::vec3 dir;   // direction light travels (toward scene, normalised)
    glm::vec3 color;
};

static LightMode dayLight() {
    return { glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f)),
             glm::vec3(1.00f, 0.95f, 0.80f) };
}

static LightMode nightLight() {
    return { glm::normalize(glm::vec3(-0.2f, -1.0f, -0.1f)),
             glm::vec3(0.15f, 0.20f, 0.35f) };
}

// ---- Pole-light positions: ring around the 68×47 m course ------------------

static const int   NUM_POLE_LIGHTS = 8;
static const float POLE_Y          = 6.0f;

static const glm::vec3 POLE_LIGHT_POS[NUM_POLE_LIGHTS] = {
    {-30.0f, POLE_Y, -22.0f},
    {  0.0f, POLE_Y, -22.0f},
    { 30.0f, POLE_Y, -22.0f},
    { 34.0f, POLE_Y,   0.0f},
    { 30.0f, POLE_Y,  22.0f},
    {  0.0f, POLE_Y,  22.0f},
    {-30.0f, POLE_Y,  22.0f},
    {-34.0f, POLE_Y,   0.0f},
};

// ---- Set all frame-constant uniforms on the main shader --------------------

static void setMainShaderUniforms(Shader          &shader,
                                  const glm::mat4 &view,
                                  const glm::mat4 &projection,
                                  const glm::mat4 &lsm,
                                  const LightMode &light,
                                  const Camera    &camera,
                                  bool             isNight)
{
    shader.use();
    shader.setMat4 ("view",             view);
    shader.setMat4 ("projection",       projection);
    shader.setMat4 ("lightSpaceMatrix", lsm);
    shader.setInt  ("shadowMap",        1);   // unit 1
    shader.setInt  ("objectTexture",    0);   // unit 0
    shader.setInt  ("useTexture",       0);
    shader.setFloat("objectAlpha",      1.0f);

    shader.setVec3("dirLightDirection", light.dir);
    shader.setVec3("dirLightColor",     light.color);
    shader.setVec3("viewPos",           camera.getPosition());
    shader.setInt ("isNight",           isNight ? 1 : 0);

    glm::vec3 plColor = isNight
        ? glm::vec3(1.0f, 0.72f, 0.30f)
        : glm::vec3(0.0f);
    shader.setVec3("pointLightColor", plColor);
    shader.setInt ("numPointLights",  NUM_POLE_LIGHTS);
    for (int i = 0; i < NUM_POLE_LIGHTS; ++i) {
        std::ostringstream oss;
        oss << "pointLightPositions[" << i << "]";
        shader.setVec3(oss.str(), POLE_LIGHT_POS[i]);
    }

    shader.setInt  ("spotlightOn",          camera.spotlightOn ? 1 : 0);
    shader.setVec3 ("spotlightPos",         camera.getSpotlightPosition());
    shader.setVec3 ("spotlightDir",         camera.getSpotlightDirection());
    shader.setFloat("spotlightCutoff",      camera.getCutoffAngle());
    shader.setFloat("spotlightOuterCutoff", camera.getCutoffAngle() + 5.0f);
}

// ---- main ------------------------------------------------------------------

int main() {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return EXIT_FAILURE; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "The Vines Mini Golf",
                                          nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback            (window, keyCallback);
    glfwSetScrollCallback         (window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, 1280, 720);

    // ---- Camera / drone controller -----------------------------------------
    Camera camera(glm::vec3(0.0f, 25.0f, 40.0f));
    g_camera = &camera;

    std::cout <<
        "\n=== The Vines Mini Golf — Controls ===\n"
        "  WASD          move forward / back / strafe\n"
        "  Space / Shift ascend / descend\n"
        "  Arrow keys    look around\n"
        "  Scroll        adjust movement speed\n"
        "  N             toggle day / night\n"
        "  F             toggle drone spotlight\n"
        "  P             print drone position\n"
        "  ESC           quit\n"
        "======================================\n\n";

    // ---- Shaders -----------------------------------------------------------
    Shader mainShader  ("shaders/main.vert",   "shaders/main.frag");
    Shader shadowShader("shaders/shadow.vert", "shaders/shadow.frag");
    Shader skyShader   ("shaders/skybox.vert", "shaders/skybox.frag");

    // ---- Shadow map --------------------------------------------------------
    ShadowMap shadowMap;

    // ---- Scene objects -----------------------------------------------------
    Skybox skybox;
    Drone  drone;
    Mesh   groundPlane = Mesh::createPlane(160.0f, 120.0f, 2, 2);

    std::vector<HoleConfig> configs = buildCourseData();
    std::vector<HoleNode>   holes;
    holes.reserve(configs.size());
    for (const auto &cfg : configs)
        holes.push_back(HoleFactory::build(cfg));

    // ---- Per-frame state ---------------------------------------------------
    float windmillSpin = 0.0f;
    float rotorSpin    = 0.0f;
    float lastTime     = static_cast<float>(glfwGetTime());

    // ---- Render loop -------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        float dt  = now - lastTime;
        lastTime  = now;

        // --- Per-frame updates
        windmillSpin += 720.0f * dt;
        rotorSpin    += 720.0f * dt;

        camera.processKeyboard(window, dt);

        bool nDown = (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS);
        if (nDown && !g_nWasPressed) {
            g_isNight = !g_isNight;
            skybox.setNight(g_isNight);
        }
        g_nWasPressed = nDown;

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        float aspect = (fbH > 0) ? static_cast<float>(fbW) / fbH : 1.0f;

        glm::mat4 view       = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(aspect);
        glm::vec3 camPos     = camera.getPosition();
        glm::vec3 camFront   = camera.getFront();

        LightMode light = g_isNight ? nightLight() : dayLight();
        glm::mat4 lsm   = shadowMap.getLightSpaceMatrix(light.dir, glm::vec3(0.0f));

        // ================================================================
        // PASS 1 — shadow depth map
        // ================================================================
        shadowMap.beginShadowPass();
        shadowShader.use();
        shadowShader.setMat4("lightSpaceMatrix", lsm);

        shadowShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)));
        groundPlane.draw();

        for (auto &hole : holes)
            hole.draw(shadowShader, windmillSpin);

        drone.draw(shadowShader, camPos, camFront, rotorSpin);

        shadowMap.endShadowPass();

        // ================================================================
        // PASS 2 — full shading
        // ================================================================
        glViewport(0, 0, fbW, fbH);
        glClearColor(0.05f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Skybox — no depth write, rendered before everything else
        glDepthMask(GL_FALSE);
        skybox.draw(skyShader, view, projection);
        glDepthMask(GL_TRUE);

        // Bind shadow map to texture unit 1 (stays for all main-shader draws)
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());

        // Set all frame-constant uniforms once
        setMainShaderUniforms(mainShader, view, projection, lsm,
                              light, camera, g_isNight);

        // Ground plane — drawn before holes so holes sit on top
        mainShader.setInt ("useTexture",  0);
        mainShader.setVec3("objectColor", glm::vec3(0.08f, 0.28f, 0.05f));
        mainShader.setMat4("model",       glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)));
        groundPlane.draw();

        mainShader.setInt ("useTexture",  0);
        mainShader.setVec3("objectColor", glm::vec3(0.13f, 0.55f, 0.13f));

        // All 18 holes (terrain, streams, ponds, bridges, windmill, obstacles)
        for (auto &hole : holes) {
            mainShader.setVec3("objectColor", glm::vec3(1.0f));
            hole.draw(mainShader, windmillSpin);
        }

        // Drone model
        drone.draw(mainShader, camPos, camFront, rotorSpin);

        // Spotlight cone — transparent overlay, only when spotlight is active
        if (camera.spotlightOn)
            drone.drawSpotlightCone(mainShader, camPos);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_camera = nullptr;
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
