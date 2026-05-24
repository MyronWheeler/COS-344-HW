#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Shader;

class Skybox {
    public:
        Skybox();
        ~Skybox();

        void setNight(bool night);
        void draw(Shader &shader, glm::mat4 view, glm::mat4 projection);

    private:
        GLuint VAO, VBO, EBO;
        GLuint cubemapDay;
        GLuint cubemapNight;
        bool   isNight;
};