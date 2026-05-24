#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "Mesh.h"

class Shader;
class Rock {
public:
    Rock(glm::vec3 scale = glm::vec3(1.0f));
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh mesh;
    GLuint texture;
    glm::vec3 localScale;
};
class Barrel {
public:
    explicit Barrel(bool sideways = false);
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh body;
    Mesh capTop;
    Mesh capBot;
    GLuint texture;
    bool sideways;
};

class LogBarrier {
public:
    LogBarrier();
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh mesh;
    GLuint texture;
};

class Billboard {
public:
    Billboard(const std::string &texturePath, const glm::vec3 &cameraPos);
    void draw(Shader &shader, glm::mat4 modelMatrix);
    void updateCamera(const glm::vec3 &camPos);
private:
    Mesh quad;
    GLuint texture;
    glm::vec3 camPosition;
};


class FlagPole {
public:
    explicit FlagPole(glm::vec3 flagColor = glm::vec3(1.0f, 0.1f, 0.1f));
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh pole;
    Mesh flag;
    glm::vec3 color;
};

class Bridge {
public:
    Bridge(float width, float length, float thickness);
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh mesh;
    GLuint texture;
};

class LightPole {
public:
    LightPole();
    void draw(Shader &shader, glm::mat4 modelMatrix);
private:
    Mesh pole;
    Mesh cap;
};


class Windmill {
public:
    Windmill();
    void draw(Shader &shader, glm::mat4 modelMatrix, float spinAngle);
private:
    Mesh bodyTop;
    Mesh bodyMid;
    Mesh bodyBot;
    Mesh baseLeft;
    Mesh baseRight;
    Mesh baseLintel;
    Mesh roof;
    Mesh axle;
    Mesh hub;
    Mesh blade;
};
