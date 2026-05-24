#include "ShadowMap.h"
#include "MathHelpers.h"
#include <cmath>
#include <iostream>

ShadowMap::ShadowMap() {
    
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    
    glGenFramebuffers(1, &depthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "[ShadowMap] Framebuffer incomplete\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap() {
    glDeleteFramebuffers(1, &depthFBO);
    glDeleteTextures(1, &depthTexture);
}

void ShadowMap::beginShadowPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glCullFace(GL_FRONT);
}

void ShadowMap::endShadowPass() {
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 ShadowMap::getLightSpaceMatrix(glm::vec3 lightDir, glm::vec3 sceneCenter) const {
    glm::vec3 ld = glm::normalize(lightDir);    
    glm::vec3 worldUp = (std::abs(ld.y) > 0.99f)? glm::vec3(1.0f, 0.0f, 0.0f): glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 lightEye = sceneCenter - ld * 90.0f;
    glm::mat4 lightView = makeLookAt(lightEye, sceneCenter, worldUp);
    glm::mat4 lightProj = makeOrtho(-55.0f, 55.0f, -42.0f, 42.0f, 1.0f, 220.0f);
    return lightProj * lightView;
}

GLuint ShadowMap::getDepthTexture() const {
    return depthTexture;
}
