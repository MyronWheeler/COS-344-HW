#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

static const float FOV              = 45.0f;
static const float NEAR_PLANE       = 0.1f;
static const float FAR_PLANE        = 2000.0f;
static const float LERP_FACTOR      = 12.0f;
static const float SPEED_MIN        = 1.0f;
static const float SPEED_MAX        = 80.0f;
static const float SPOTLIGHT_CUTOFF = 15.0f;
Camera::Camera(glm::vec3 startPosition)
    : spotlightOn(false)
    , position(startPosition)
    , front(0.0f, 0.0f, -1.0f)
    , up(0.0f, 1.0f, 0.0f)
    , right(1.0f, 0.0f, 0.0f)
    , yaw(-90.0f)
    , pitch(-20.0f)
    , velocity(0.0f)
    , targetVelocity(0.0f)
    , moveSpeed(5.0f)
    , turnSpeed(60.0f)
    , fKeyWasPressed(false)
{
    updateVectors();
}

void Camera::processKeyboard(GLFWwindow *window, float deltaTime) {
    glm::vec3 wishDir(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) wishDir += front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) wishDir -= front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) wishDir += right;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) wishDir -= right;
    if (glfwGetKey(window, GLFW_KEY_SPACE)      == GLFW_PRESS) wishDir += glm::vec3(0,1,0);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) wishDir -= glm::vec3(0,1,0);

    if (glm::length(wishDir) > 0.0f)
        wishDir = glm::normalize(wishDir);

    targetVelocity = wishDir * moveSpeed;

    float t = 1.0f - std::exp(-LERP_FACTOR * deltaTime);
    velocity = glm::mix(velocity, targetVelocity, t);
    position += velocity * deltaTime;

    
    float td = turnSpeed * deltaTime;
    bool looked = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) { yaw   -= td; looked = true; }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { yaw   += td; looked = true; }
    if (glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) { pitch += td; looked = true; }
    if (glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) { pitch -= td; looked = true; }

    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    if (looked) updateVectors();

    bool fDown = (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);
    if (fDown && !fKeyWasPressed) spotlightOn = !spotlightOn;
    fKeyWasPressed = fDown;
}

void Camera::processMouseScroll(float yoffset) {
    moveSpeed += yoffset * 2.0f;
    if (moveSpeed < SPEED_MIN) moveSpeed = SPEED_MIN;
    if (moveSpeed > SPEED_MAX) moveSpeed = SPEED_MAX;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(FOV), aspectRatio, NEAR_PLANE, FAR_PLANE);
}

glm::vec3 Camera::getPosition()          const { return position; }
glm::vec3 Camera::getFront()             const { return front;    }
glm::vec3 Camera::getSpotlightPosition() const { return position; }
glm::vec3 Camera::getSpotlightDirection()const { return front;    }
float     Camera::getCutoffAngle()       const { return SPOTLIGHT_CUTOFF; }

void Camera::updateVectors() {
    float yawR   = glm::radians(yaw);
    float pitchR = glm::radians(pitch);
    glm::vec3 f;
    f.x = std::cos(pitchR) * std::cos(yawR);
    f.y = std::sin(pitchR);
    f.z = std::cos(pitchR) * std::sin(yawR);
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, glm::vec3(0,1,0)));
    up    = glm::normalize(glm::cross(right, front));
}
