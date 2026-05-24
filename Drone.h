#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

class Shader;

class Drone {
    public:
        Drone();
        void draw(Shader &shader, glm::vec3 camPos, glm::vec3 camFront, float rotorAngle);
        void drawSpotlightCone(Shader &shader, glm::vec3 camPos);

    private:
        Mesh body;
        Mesh camBox;
        Mesh arm;
        Mesh rotor;
        Mesh cone;
};
