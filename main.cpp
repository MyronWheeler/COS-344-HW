#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "MathHelpers.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <stdlib.h>

#include "Shader.h"
#include "Mesh.h"
#include "TextureLoader.h"
#include "Camera.h"
#include "Skybox.h"
#include "Objects.h"
#include "HoleConfig.h"
#include "HoleFactory.h"
#include "CourseData.h"
#include "ShadowMap.h"
#include "Drone.h"



struct DecorInstance {
    std::string type;
    glm::vec3   worldPos;
    glm::vec3   scale;
    float       rotation;
};



static const glm::vec3 STREAM_WAYPOINTS[] = {
    
    {-55.0f,  0.08f, -15.0f},
    {-38.0f,  0.08f, -14.0f},
    {-26.0f,  0.08f, -16.0f},
    {-14.0f,  0.08f, -13.0f},
    { -4.0f,  0.08f, -15.0f},
    {  6.0f,  0.08f, -14.0f},
    
    { 10.0f,  0.08f,  -8.0f},
    {  8.0f,  0.08f,   0.0f},
    {  6.0f,  0.08f,   8.0f},
    
    {  4.0f,  0.08f,  14.0f},
    { -6.0f,  0.08f,  15.0f},
    {-16.0f,  0.08f,  14.0f},
    {-26.0f,  0.08f,  16.0f},
    {-38.0f,  0.08f,  14.0f},
    {-55.0f,  0.08f,  15.0f},
};
static const int NUM_STREAM_WAYPOINTS = 15;



static Camera *g_camera      = nullptr;
static bool    g_isNight     = false;
static bool    g_nWasPressed = false;



static void keyCallback(GLFWwindow *window, int key, int , int action, int ) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_P && action == GLFW_PRESS && g_camera) {
        glm::vec3 p = g_camera->getPosition();
        std::cout << "Drone position: ("
                  << p.x << ", " << p.y << ", " << p.z << ")\n";
    }
}

static void scrollCallback(GLFWwindow * , double , double y) {
    if (g_camera) g_camera->processMouseScroll(static_cast<float>(y));
}

static void framebufferSizeCallback(GLFWwindow * , int w, int h) {
    glViewport(0, 0, w, h);
}



struct LightMode {
    glm::vec3 dir;   
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



static const int NUM_POLE_LIGHTS = 18;


static const glm::vec3 POLE_LIGHT_POSITIONS[NUM_POLE_LIGHTS] = {
    {-52.0f, 5.0f, -28.0f},
    { 52.0f, 5.0f, -28.0f},
    {-52.0f, 5.0f,  28.0f},
    { 52.0f, 5.0f,  28.0f},
    {-52.0f, 5.0f,   0.0f},
    { 52.0f, 5.0f,   0.0f},
    {  0.0f, 5.0f, -42.0f},
    {  0.0f, 5.0f,  42.0f},
    {-52.0f, 5.0f, -14.0f},
    {-52.0f, 5.0f,  14.0f},
    { 52.0f, 5.0f, -14.0f},
    { 52.0f, 5.0f,  14.0f},
    {-22.0f, 5.0f, -42.0f},
    { 22.0f, 5.0f, -42.0f},
    {-22.0f, 5.0f,  42.0f},
    { 22.0f, 5.0f,  42.0f},
    {  0.0f, 5.0f, -14.0f},
    {  0.0f, 5.0f,  14.0f},
};



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
    shader.setInt  ("shadowMap",        1);   
    shader.setInt  ("objectTexture",    0);   
    shader.setInt  ("useTexture",       0);
    shader.setInt  ("useAlpha",         0);
    shader.setFloat("objectAlpha",      1.0f);

    shader.setVec3("dirLightDirection", light.dir);
    shader.setVec3("dirLightColor",     light.color);
    shader.setVec3("viewPos",           camera.getPosition());
    shader.setInt ("isNight",           isNight ? 1 : 0);

    
    shader.setVec3("pointLightColor", glm::vec3(1.0f, 0.90f, 0.65f));
    shader.setInt ("numPointLights",  NUM_POLE_LIGHTS);
    for (int i = 0; i < NUM_POLE_LIGHTS; ++i) {
        std::ostringstream oss;
        oss << "pointLightPositions[" << i << "]";
        shader.setVec3(oss.str(), POLE_LIGHT_POSITIONS[i]);
    }

    shader.setInt  ("spotlightOn",          camera.spotlightOn ? 1 : 0);
    shader.setVec3 ("spotlightPos",         camera.getSpotlightPosition());
    shader.setVec3 ("spotlightDir",         camera.getSpotlightDirection());
    shader.setFloat("spotlightCutoff",      camera.getCutoffAngle());
    shader.setFloat("spotlightOuterCutoff", camera.getCutoffAngle() + 5.0f);
}



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

    
    Shader mainShader  ("shaders/main.vert",   "shaders/main.frag");
    Shader shadowShader("shaders/shadow.vert", "shaders/shadow.frag");
    Shader skyShader   ("shaders/skybox.vert", "shaders/skybox.frag");
    Shader waterShader ("shaders/water.vert",  "shaders/water.frag");

    
    ShadowMap shadowMap;

    
    Skybox skybox;
    Drone  drone;
    Mesh   groundPlane = Mesh::createPlane(260.0f, 200.0f, 2, 2);

    std::vector<HoleConfig> configs = buildCourseData();
    std::vector<HoleNode>   holes;
    holes.reserve(configs.size());
    for (const auto &cfg : configs)
        holes.push_back(HoleFactory::build(cfg));

    
    static const DecorInstance COURSE_DECOR[] = {
        
        {"Barrel",     {-54.0f, 0.3f, -10.0f}, {1.2f, 1.2f, 1.2f},  0.0f},
        {"Barrel",     {-54.0f, 0.3f,  10.0f}, {1.2f, 1.2f, 1.2f}, 45.0f},
        {"Barrel",     { 54.0f, 0.3f, -10.0f}, {1.2f, 1.2f, 1.2f}, 20.0f},
        {"Barrel",     { 54.0f, 0.3f,  10.0f}, {1.2f, 1.2f, 1.2f}, 60.0f},
        {"Barrel",     {  8.0f, 0.3f, -38.0f}, {1.2f, 1.2f, 1.2f}, 30.0f},
        {"Barrel",     { -8.0f, 0.3f, -38.0f}, {1.2f, 1.2f, 1.2f},  0.0f},
        {"Barrel",     {  8.0f, 0.3f,  38.0f}, {1.2f, 1.2f, 1.2f}, 15.0f},
        {"Barrel",     { -8.0f, 0.3f,  38.0f}, {1.2f, 1.2f, 1.2f}, 45.0f},
        
        {"Rock",       {-16.0f, 0.3f, -13.0f}, {1.5f, 1.1f, 1.3f},  0.0f},
        {"Rock",       {-13.0f, 0.3f, -11.0f}, {1.0f, 0.8f, 1.2f}, 20.0f},
        {"Rock",       { 16.0f, 0.3f,  13.0f}, {1.4f, 1.0f, 1.1f},  0.0f},
        {"Rock",       { 13.0f, 0.3f,  11.0f}, {1.1f, 0.9f, 1.3f}, 35.0f},
        {"Rock",       {-54.0f, 0.3f,  -5.0f}, {1.8f, 1.2f, 1.5f},  0.0f},
        {"Rock",       { 54.0f, 0.3f,   5.0f}, {1.6f, 1.1f, 1.4f},  0.0f},
        {"Rock",       {  5.0f, 0.3f, -16.0f}, {1.3f, 1.0f, 1.2f},  0.0f},
        {"Rock",       { -5.0f, 0.3f,  16.0f}, {1.2f, 0.9f, 1.1f},  0.0f},
        
        {"LogBarrier", {-54.0f, 0.2f, -18.0f}, {1.5f, 1.0f, 1.0f},  0.0f},
        {"LogBarrier", {-54.0f, 0.2f,  18.0f}, {1.5f, 1.0f, 1.0f},  0.0f},
        {"LogBarrier", { 54.0f, 0.2f, -18.0f}, {1.5f, 1.0f, 1.0f},  0.0f},
        {"LogBarrier", { 54.0f, 0.2f,  18.0f}, {1.5f, 1.0f, 1.0f},  0.0f},
        
        {"LightPole",  {-52.0f, 0.0f, -28.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  { 52.0f, 0.0f, -28.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {-52.0f, 0.0f,  28.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  { 52.0f, 0.0f,  28.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {-52.0f, 0.0f,   0.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  { 52.0f, 0.0f,   0.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {  0.0f, 0.0f, -42.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {  0.0f, 0.0f,  42.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {-52.0f, 0.0f, -14.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {-52.0f, 0.0f,  14.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  { 52.0f, 0.0f, -14.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  { 52.0f, 0.0f,  14.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {-22.0f, 0.0f, -42.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  { 22.0f, 0.0f, -42.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {-22.0f, 0.0f,  42.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  { 22.0f, 0.0f,  42.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {  0.0f, 0.0f, -14.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
        {"LightPole",  {  0.0f, 0.0f,  14.0f}, {1.0f, 1.0f, 1.0f},  0.0f},
    };
    static const int NUM_DECOR = 38;

    Rock      decorRock(glm::vec3(1.0f, 1.0f, 1.0f));
    Barrel    decorBarrel(false);
    LightPole decorPole;
    LogBarrier decorLog;

    auto drawDecor = [&](Shader &shader) {
        for (int i = 0; i < NUM_DECOR; ++i) {
            const DecorInstance &d = COURSE_DECOR[i];
                        glm::mat4 m = makeTranslate(d.worldPos)
                                                * makeRotate(degToRad(d.rotation), glm::vec3(0.0f, 1.0f, 0.0f))
                                                * makeScale(d.scale);
            if      (d.type == "Barrel")     decorBarrel.draw(shader, m);
            else if (d.type == "Rock")       decorRock.draw(shader, m);
            else if (d.type == "LightPole")  decorPole.draw(shader, m);
            else if (d.type == "LogBarrier") decorLog.draw(shader, m);
        }
    };

    
    Mesh streamMesh = []() {
        const float STREAM_WIDTH = 3.5f;
        const float HALF_W       = STREAM_WIDTH * 0.5f;
        std::vector<Vertex>       verts;
        std::vector<unsigned int> idx;
        float uvLen = 0.0f;

        for (int i = 0; i < NUM_STREAM_WAYPOINTS; ++i) {
            glm::vec3 pos = STREAM_WAYPOINTS[i];

            
            glm::vec3 dir(1.0f, 0.0f, 0.0f);
            if (i < NUM_STREAM_WAYPOINTS - 1)
                dir = glm::normalize(STREAM_WAYPOINTS[i + 1] - pos);
            if (i > 0) {
                glm::vec3 prev = glm::normalize(pos - STREAM_WAYPOINTS[i - 1]);
                dir = glm::normalize(dir + prev);
            }

            glm::vec3 right = glm::normalize(
                glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f))) * HALF_W;

            if (i > 0)
                uvLen += glm::length(STREAM_WAYPOINTS[i] - STREAM_WAYPOINTS[i - 1])
                         / STREAM_WIDTH;

            glm::vec3 up(0.0f, 1.0f, 0.0f);
            verts.push_back({pos - right, up, {0.0f, uvLen}});  
            verts.push_back({pos + right, up, {1.0f, uvLen}});  
        }

        
        for (int i = 0; i < NUM_STREAM_WAYPOINTS - 1; ++i) {
            unsigned int b = static_cast<unsigned int>(i * 2);
            idx.push_back(b + 0); idx.push_back(b + 1); idx.push_back(b + 2);
            idx.push_back(b + 1); idx.push_back(b + 3); idx.push_back(b + 2);
        }
        return Mesh(verts, idx);
    }();

    Mesh pavTopBot = Mesh::createPlane(140.0f, 12.0f, 2, 2);
    Mesh pavSide   = Mesh::createPlane( 12.0f, 84.0f, 2, 2);
    Mesh pavCorner = Mesh::createPlane( 12.0f, 12.0f, 2, 2);
    
    
    GLuint pavementTex = TextureLoader::load("textures/paving.png");

    auto drawPavement = [&](Shader &shader) {
        const float Y = 0.06f;
        const float SIDE_EPS   = 0.0005f; 
        const float CORNER_EPS = 0.0010f; 

        
        shader.setMat4("model", makeTranslate(glm::vec3(  0.0f, Y, -42.0f)));
        pavTopBot.draw();
        shader.setMat4("model", makeTranslate(glm::vec3(  0.0f, Y,  42.0f)));
        pavTopBot.draw();

        
        shader.setMat4("model", makeTranslate(glm::vec3(-64.0f, Y + SIDE_EPS,   0.0f)));
        pavSide.draw();
        shader.setMat4("model", makeTranslate(glm::vec3( 64.0f, Y + SIDE_EPS,   0.0f)));
        pavSide.draw();

        
        const glm::vec3 cs[4] = {
            glm::vec3(-64.0f, Y + CORNER_EPS, -42.0f), glm::vec3( 64.0f, Y + CORNER_EPS, -42.0f),
            glm::vec3(-64.0f, Y + CORNER_EPS,  42.0f), glm::vec3( 64.0f, Y + CORNER_EPS,  42.0f)
        };
        for (int ci = 0; ci < 4; ++ci) {
            shader.setMat4("model", makeTranslate(cs[ci]));
            pavCorner.draw();
        }
    };

    float windmillSpin = 0.0f;
    float rotorSpin    = 0.0f;
    float lastTime     = static_cast<float>(glfwGetTime());

    
    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        float dt  = now - lastTime;
        lastTime  = now;

        
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

        
        
        
        shadowMap.beginShadowPass();
        shadowShader.use();
        shadowShader.setMat4("lightSpaceMatrix", lsm);

        shadowShader.setMat4("model", makeTranslate(glm::vec3(0.0f, 0.05f, 0.0f)));
        groundPlane.draw();

        for (auto &hole : holes)
            hole.draw(shadowShader, windmillSpin);

        drawDecor(shadowShader);
        drawPavement(shadowShader);

        shadowShader.setMat4("model", glm::mat4(1.0f));
        streamMesh.draw();

        drone.draw(shadowShader, camPos, camFront, rotorSpin);

        shadowMap.endShadowPass();

        
        
        
        glViewport(0, 0, fbW, fbH);
        glClearColor(0.05f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glDepthMask(GL_FALSE);
        skybox.draw(skyShader, view, projection);
        glDepthMask(GL_TRUE);

        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());

        
        setMainShaderUniforms(mainShader, view, projection, lsm,
                              light, camera, g_isNight);

        
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        mainShader.setInt ("useTexture",  0);
        mainShader.setVec3("objectColor", glm::vec3(0.06f, 0.22f, 0.04f));
        mainShader.setMat4("model",       makeTranslate(glm::vec3(0.0f, 0.05f, 0.0f)));
        groundPlane.draw();

        mainShader.setInt ("useTexture",  0);
        mainShader.setVec3("objectColor", glm::vec3(0.13f, 0.55f, 0.13f));

        
        for (auto &hole : holes) {
            mainShader.setVec3("objectColor", glm::vec3(1.0f));
            hole.draw(mainShader, windmillSpin);
        }

        drawDecor(mainShader);

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pavementTex);
        mainShader.setInt ("objectTexture", 0);
        mainShader.setInt ("useTexture",  1);
        mainShader.setVec3("objectColor", glm::vec3(1.0f));
        drawPavement(mainShader);
        
        mainShader.setInt ("useTexture",  0);

        
        {
            glm::vec3 activeLightPos = -light.dir * 100.0f;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            waterShader.use();
            waterShader.setMat4 ("view",       view);
            waterShader.setMat4 ("projection", projection);
            waterShader.setMat4 ("model",      glm::mat4(1.0f));
            waterShader.setFloat("time",       now);
            waterShader.setVec3 ("lightPos",   activeLightPos);
            waterShader.setVec3 ("viewPos",    camPos);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/water_normal.png"));
            waterShader.setInt("normalMap", 0);
            streamMesh.draw();
            glDisable(GL_BLEND);
        }

        
        drone.draw(mainShader, camPos, camFront, rotorSpin);

        
        if (camera.spotlightOn) {
            mainShader.setInt("useAlpha", 1);
            drone.drawSpotlightCone(mainShader, camPos);
            mainShader.setInt("useAlpha", 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_camera = nullptr;
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
