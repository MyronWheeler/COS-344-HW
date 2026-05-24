#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;

class Water {
public:
    // width and depth in world units, worldPos is the centre
    // Added rotY to allow angling the river across the golf course!
    Water(float width, float depth, glm::vec3 worldPos, float rotY = 0.0f);

    void draw(Shader &shader, float time, glm::mat4 view, glm::mat4 projection,
              glm::vec3 lightPos, glm::vec3 viewPos);

private:
    GLuint VAO, VBO, EBO;
    GLuint normalMap;
    unsigned int indexCount;
    glm::mat4    modelMatrix;
};