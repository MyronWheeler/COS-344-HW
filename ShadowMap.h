#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class ShadowMap {
public:
    static const int SHADOW_WIDTH  = 2048;
    static const int SHADOW_HEIGHT = 2048;

    ShadowMap();
    ~ShadowMap();

    // Bind the depth FBO and resize the viewport to the shadow resolution.
    void beginShadowPass();

    // Unbind the FBO (return to default).
    void endShadowPass();

    // Build an orthographic light-space matrix for a directional light.
    // lightDir  — direction the light travels (pointing toward the scene)
    // sceneCenter — world-space point the ortho frustum is centred on
    glm::mat4 getLightSpaceMatrix(glm::vec3 lightDir, glm::vec3 sceneCenter) const;

    GLuint getDepthTexture() const;

private:
    GLuint depthFBO;
    GLuint depthTexture;
};
