#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    Camera(glm::vec3 startPosition);

    void processKeyboard(GLFWwindow *window, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float yoffset);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;

    // Spotlight
    bool      spotlightOn;
    glm::vec3 getSpotlightPosition() const;
    glm::vec3 getSpotlightDirection() const;
    float     getCutoffAngle() const;   // inner cutoff, degrees

private:
    void updateVectors();

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    float yaw;          // degrees, -90 = looking down -Z
    float pitch;        // degrees, clamped [-89, 89]

    // Smooth movement
    glm::vec3 velocity;         // current world-space velocity
    glm::vec3 targetVelocity;   // desired velocity this frame
    float     moveSpeed;        // base speed, scroll-adjustable
    float     mouseSensitivity;

    // F key debounce
    bool fKeyWasPressed;
};
