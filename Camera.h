#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    Camera(glm::vec3 startPosition);

    void processKeyboard(GLFWwindow *window, float deltaTime);
    void processMouseScroll(float yoffset);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;

    
    bool      spotlightOn;
    glm::vec3 getSpotlightPosition() const;
    glm::vec3 getSpotlightDirection() const;
    float     getCutoffAngle() const;   

private:
    void updateVectors();

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    float yaw;          
    float pitch;        

    
    glm::vec3 velocity;         
    glm::vec3 targetVelocity;   
    float     moveSpeed;        
    float     turnSpeed;        

    
    bool fKeyWasPressed;
};
