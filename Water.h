#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;

class Water {
    public:
        Water(float width, float depth, glm::vec3 worldPos);

        void draw(Shader &shader, float time, glm::mat4 view, glm::mat4 projection, glm::vec3 lightPos, glm::vec3 viewPos);

    private:
        GLuint VAO, VBO, EBO;
        GLuint normalMap;
        unsigned int indexCount;
        glm::mat4 modelMatrix;
};
