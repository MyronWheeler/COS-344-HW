#pragma once

#include <glm/glm.hpp>
#include <cmath>

static inline float degToRad(float degrees) {
    return degrees * 0.017453292519943295769f;
}

static inline glm::mat4 makeTranslate(const glm::vec3 &offset) {
    glm::mat4 m(1.0f);
    m[3] = glm::vec4(offset, 1.0f);
    return m;
}

static inline glm::mat4 makeScale(const glm::vec3 &scale) {
    glm::mat4 m(1.0f);
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
    return m;
}

static inline glm::mat4 makeRotate(float angleRad, glm::vec3 axis) {
    axis = glm::normalize(axis);
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    float oc = 1.0f - c;

    glm::mat4 m(1.0f);
    m[0][0] = oc * axis.x * axis.x + c;
    m[0][1] = oc * axis.x * axis.y + axis.z * s;
    m[0][2] = oc * axis.x * axis.z - axis.y * s;

    m[1][0] = oc * axis.x * axis.y - axis.z * s;
    m[1][1] = oc * axis.y * axis.y + c;
    m[1][2] = oc * axis.y * axis.z + axis.x * s;

    m[2][0] = oc * axis.x * axis.z + axis.y * s;
    m[2][1] = oc * axis.y * axis.z - axis.x * s;
    m[2][2] = oc * axis.z * axis.z + c;

    m[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return m;
}

static inline glm::mat4 makePerspective(float fovyRad, float aspect, float zNear, float zFar) {
    float f = 1.0f / std::tan(fovyRad * 0.5f);
    glm::mat4 m(0.0f);
    m[0][0] = f / aspect;
    m[1][1] = f;
    m[2][2] = (zFar + zNear) / (zNear - zFar);
    m[2][3] = -1.0f;
    m[3][2] = (2.0f * zFar * zNear) / (zNear - zFar);
    return m;
}

static inline glm::mat4 makeOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
    glm::mat4 m(1.0f);
    m[0][0] = 2.0f / (right - left);
    m[1][1] = 2.0f / (top - bottom);
    m[2][2] = -2.0f / (zFar - zNear);
    m[3][0] = -(right + left) / (right - left);
    m[3][1] = -(top + bottom) / (top - bottom);
    m[3][2] = -(zFar + zNear) / (zFar - zNear);
    return m;
}

static inline glm::mat4 makeLookAt(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up) {
    glm::vec3 f = glm::normalize(center - eye);
    glm::vec3 s = glm::normalize(glm::cross(f, glm::normalize(up)));
    glm::vec3 u = glm::cross(s, f);

    glm::mat4 m(1.0f);
    m[0][0] = s.x; m[1][0] = s.y; m[2][0] = s.z;
    m[0][1] = u.x; m[1][1] = u.y; m[2][1] = u.z;
    m[0][2] = -f.x; m[1][2] = -f.y; m[2][2] = -f.z;
    m[3][0] = -glm::dot(s, eye);
    m[3][1] = -glm::dot(u, eye);
    m[3][2] =  glm::dot(f, eye);
    return m;
}

static inline const float *mat4Ptr(const glm::mat4 &m) {
    return &m[0][0];
}

static inline const float *vec3Ptr(const glm::vec3 &v) {
    return &v.x;
}

static inline const float *vec4Ptr(const glm::vec4 &v) {
    return &v.x;
}