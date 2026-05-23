#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>
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
#include "ShadowMap.h"

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

// ---- Light parameters for day / night modes --------------------------------

struct LightMode {
    glm::vec3 dir;    // direction light travels (toward scene, normalised)
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
// Heights are at lamp-head height (Y = 6).  Positions chosen to give even
// coverage without clustering on any one side.

static const int   NUM_POLE_LIGHTS = 8;
static const float POLE_Y          = 6.0f;

static const glm::vec3 POLE_LIGHT_POS[NUM_POLE_LIGHTS] = {
    {-30.0f, POLE_Y, -22.0f},   // NW corner
    {  0.0f, POLE_Y, -22.0f},   // N centre
    { 30.0f, POLE_Y, -22.0f},   // NE corner
    { 34.0f, POLE_Y,   0.0f},   // E centre
    { 30.0f, POLE_Y,  22.0f},   // SE corner
    {  0.0f, POLE_Y,  22.0f},   // S centre
    {-30.0f, POLE_Y,  22.0f},   // SW corner
    {-34.0f, POLE_Y,   0.0f},   // W centre
};

// ---- Helper: apply all frame-constant uniforms to the main shader ----------

static void setMainShaderUniforms(Shader              &shader,
                                  const glm::mat4     &view,
                                  const glm::mat4     &projection,
                                  const glm::mat4     &lsm,
                                  const LightMode     &light,
                                  const Camera        &camera,
                                  bool                 isNight)
{
    shader.use();
    shader.setMat4("view",            view);
    shader.setMat4("projection",      projection);
    shader.setMat4("lightSpaceMatrix",lsm);
    shader.setInt ("shadowMap",       1);   // unit 1
    shader.setInt ("objectTexture",   0);   // unit 0
    shader.setInt ("useTexture",      0);

    shader.setVec3("dirLightDirection", light.dir);
    shader.setVec3("dirLightColor",     light.color);
    shader.setVec3("viewPos",           camera.getPosition());
    shader.setInt ("isNight",           isNight ? 1 : 0);

    // Point lights (active in night mode; colour is zero in day so no cost)
    glm::vec3 plColor = isNight
        ? glm::vec3(1.0f, 0.72f, 0.30f)   // warm orange lamp glow
        : glm::vec3(0.0f);
    shader.setVec3("pointLightColor",  plColor);
    shader.setInt ("numPointLights",   NUM_POLE_LIGHTS);
    for (int i = 0; i < NUM_POLE_LIGHTS; ++i) {
        std::string name = "pointLightPositions[" + std::to_string(i) + "]";
        shader.setVec3(name, POLE_LIGHT_POS[i]);
    }

    // Drone spotlight
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
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, 1280, 720);

    // ---- Camera -------------------------------------------------------------
    Camera camera(glm::vec3(0.0f, 40.0f, 50.0f));
    g_camera = &camera;

    // ---- Shaders ------------------------------------------------------------
    Shader mainShader  ("shaders/main.vert",   "shaders/main.frag");
    Shader shadowShader("shaders/shadow.vert", "shaders/shadow.frag");
    Shader skyShader   ("shaders/skybox.vert", "shaders/skybox.frag");
    Shader waterShader ("shaders/water.vert",  "shaders/water.frag");

    // ---- Shadow map ---------------------------------------------------------
    ShadowMap shadowMap;

    // ---- Scene objects ------------------------------------------------------
    Skybox skybox;
    Water  courseStream(4.0f, 47.0f, glm::vec3(32.0f, 0.0f, 0.0f));

    std::vector<HoleConfig> configs = buildCourseData();
    std::vector<HoleNode>   holes;
    holes.reserve(configs.size());
    for (const auto &cfg : configs)
        holes.push_back(HoleFactory::build(cfg));

    Rock      rock1(glm::vec3(1.2f, 0.9f, 1.4f));
    Barrel    barrel1(false);
    Billboard sign1("textures/billboard.png", camera.getPosition());

    glm::vec3 hole1World = configs[0].position;
    glm::mat4 rockM   = glm::translate(glm::mat4(1.0f), hole1World + glm::vec3(-2.0f, 0.4f, -1.0f));
    glm::mat4 barrelM = glm::translate(glm::mat4(1.0f), hole1World + glm::vec3( 2.0f, 0.35f, 0.0f));

    float lastTime = static_cast<float>(glfwGetTime());

    // ---- Render loop --------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        float dt  = now - lastTime;
        lastTime  = now;

        camera.processKeyboard(window, dt);
        sign1.updateCamera(camera.getPosition());

        // N — day/night toggle (debounced)
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
        glm::vec3 viewPos    = camera.getPosition();

        LightMode light = g_isNight ? nightLight() : dayLight();
        glm::mat4 lsm   = shadowMap.getLightSpaceMatrix(light.dir, glm::vec3(0.0f));

        // ================================================================
        // PASS 1 — shadow depth map
        // ================================================================
        shadowMap.beginShadowPass();

        shadowShader.use();
        shadowShader.setMat4("lightSpaceMatrix", lsm);

        // Course geometry
        for (auto &hole : holes)
            hole.draw(shadowShader);

        // Test objects (billboard excluded — orientation meaningless for shadow)
        shadowShader.setVec3("objectColor", glm::vec3(1.0f));
        rock1.draw(shadowShader, rockM);
        barrel1.draw(shadowShader, barrelM);

        shadowMap.endShadowPass();

        // ================================================================
        // PASS 2 — full shading
        // ================================================================
        glViewport(0, 0, fbW, fbH);
        glClearColor(0.05f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Skybox (no depth write, drawn first)
        glDepthMask(GL_FALSE);
        skybox.draw(skyShader, view, projection);
        glDepthMask(GL_TRUE);

        // Bind shadow map to texture unit 1 (stays bound for all main draws)
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());

        // Frame-constant main-shader uniforms
        setMainShaderUniforms(mainShader, view, projection, lsm,
                              light, camera, g_isNight);

        // All 18 holes
        for (auto &hole : holes)
            hole.draw(mainShader);

        // Test objects near hole 1
        mainShader.setInt("useTexture", 0);
        mainShader.setVec3("objectColor", glm::vec3(0.55f, 0.50f, 0.45f));
        rock1.draw(mainShader, rockM);

        mainShader.setVec3("objectColor", glm::vec3(0.60f, 0.40f, 0.20f));
        barrel1.draw(mainShader, barrelM);

        mainShader.setVec3("objectColor", glm::vec3(1.0f));
        {
            glm::mat4 signM = glm::translate(glm::mat4(1.0f),
                                             hole1World + glm::vec3(0.0f, 1.0f, -3.5f));
            signM = glm::scale(signM, glm::vec3(2.0f));
            sign1.draw(mainShader, signM);
        }

        // Water (alpha-blended, drawn last; uses its own shader)
        courseStream.draw(waterShader, now, view, projection,
                         glm::vec3(0.0f) - light.dir * 50.0f,  // light world pos
                         viewPos);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_camera = nullptr;
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
