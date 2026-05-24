#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class ShadowMap {
    public:
        static const int SHADOW_WIDTH  = 2048;
        static const int SHADOW_HEIGHT = 2048;

        ShadowMap();
        ~ShadowMap();

        void beginShadowPass();
        void endShadowPass();
        glm::mat4 getLightSpaceMatrix(glm::vec3 lightDir, glm::vec3 sceneCenter) const;
        GLuint getDepthTexture() const;

    private:
        GLuint depthFBO;
        GLuint depthTexture;
};
