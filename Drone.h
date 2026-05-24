#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

class Shader;

// Quadcopter drone drawn at a fixed offset below the camera position.
// The body stays level regardless of camera pitch.
class Drone {
public:
    Drone();

    // Draw the opaque drone body (body, arms, rotors, camera housing).
    // rotorAngle is accumulated in degrees each frame at 720 deg/s.
    void draw(Shader &shader,
              glm::vec3 camPos,
              glm::vec3 camFront,
              float     rotorAngle);

    // Draw a semi-transparent spotlight cone (call only when spotlight is on).
    // Uses GL_BLEND — caller must have already bound + configured the main shader.
    void drawSpotlightCone(Shader &shader, glm::vec3 camPos);

private:
    Mesh body;       // flat central square
    Mesh camBox;     // small housing below body
    Mesh arm;        // one box bar; reused for both cross-arms
    Mesh rotor;      // flat disk; reused for all 4 rotors
    Mesh cone;       // spotlight cone mesh (apex at origin, opens downward)
};
