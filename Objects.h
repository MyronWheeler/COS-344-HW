#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "Mesh.h"

class Shader;

// ---------------------------------------------------------------------------
// Rock — sphere with non-uniform scale baked into the draw call
// ---------------------------------------------------------------------------
class Rock {
public:
    Rock(glm::vec3 scale = glm::vec3(1.0f));
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh    mesh;
    GLuint  texture;
    glm::vec3 localScale;
};

// ---------------------------------------------------------------------------
// Barrel — cylinder body + two flat disk caps
// ---------------------------------------------------------------------------
class Barrel {
public:
    // sideways=true lays it on its side
    explicit Barrel(bool sideways = false);
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh   body;
    Mesh   capTop;
    Mesh   capBot;
    GLuint texture;
    bool   sideways;
};

// ---------------------------------------------------------------------------
// LogBarrier — horizontal log (cylinder rotated 90° on Z)
// ---------------------------------------------------------------------------
class LogBarrier {
public:
    LogBarrier();
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh   mesh;
    GLuint texture;
};

// ---------------------------------------------------------------------------
// Billboard — two crossed quads, always faces the camera
// ---------------------------------------------------------------------------
class Billboard {
public:
    Billboard(const std::string &texturePath, const glm::vec3 &cameraPos);
    void draw(Shader &shader, glm::mat4 modelMatrix);
    void updateCamera(const glm::vec3 &camPos);
private:
    Mesh      quad;
    GLuint    texture;
    glm::vec3 camPosition;
};

// ---------------------------------------------------------------------------
// FlagPole — thin vertical pole + flat flag quad
// ---------------------------------------------------------------------------
class FlagPole {
public:
    explicit FlagPole(glm::vec3 flagColor = glm::vec3(1.0f, 0.0f, 0.0f));
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh      pole;
    Mesh      flag;
    glm::vec3 color;
};

// ---------------------------------------------------------------------------
// Bridge — flat box with wood texture
// ---------------------------------------------------------------------------
class Bridge {
public:
    Bridge(float width, float length, float thickness);
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh   mesh;
    GLuint texture;
};

// ---------------------------------------------------------------------------
// LightPole — tall cylinder + small angled cap box
// ---------------------------------------------------------------------------
class LightPole {
public:
    LightPole();
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh pole;
    Mesh cap;
};

// ---------------------------------------------------------------------------
// Windmill — placeholder; Member 2 will implement internals
// ---------------------------------------------------------------------------
class Windmill {
public:
    Windmill();
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh body;
};
