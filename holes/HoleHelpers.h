#pragma once

#include "HoleConfig.h"
#include <cmath>
#include <vector>

static inline std::vector<glm::vec2> rectBounds(float hw, float hl) {
    return { {-hw, -hl}, { hw, -hl}, { hw,  hl}, {-hw,  hl} };
}
static inline std::vector<glm::vec2> rectBoundsFromSize(float length, float width) {
    return rectBounds(width * 0.5f, length * 0.5f);
}
static inline std::vector<glm::vec2> ellipseBounds(float rx, float rz, int segments = 12) {
    std::vector<glm::vec2> points;
    points.reserve(static_cast<size_t>(segments));
    const float tau = 6.28318530718f;
    for (int i = 0; i < segments; ++i) {
        float t = tau * static_cast<float>(i) / static_cast<float>(segments);
        points.push_back({rx * std::cos(t), rz * std::sin(t)});
    }
    return points;
}
static inline std::vector<float> flatElevs(int n, float y = 0.0f) {
    return std::vector<float>(static_cast<size_t>(n), y);
}
static inline void setRectHole(HoleConfig &hole,
                               float length, float width, float heading,
                               std::vector<float> elevations)
{
    hole.boundaryPoints = rectBoundsFromSize(length, width);
    hole.elevations     = std::move(elevations);
    hole.rotation       = heading;
}
static inline std::vector<glm::vec2> lShapeBounds(
    float mhw, float mhl, float ahw, float ahl, float ox)
{
    float x0 = -mhw, x1 = mhw;
    float z0 = -mhl, z1 =  mhl;
    float ax0 = ox - ahw, ax1 = ox + ahw;
    float az1 = z1 + ahl;
    return {
        { x0,  z0}, { x1,  z0}, { x1,  z1},
        {ax1,  z1}, {ax1, az1}, {ax0, az1},
        {ax0,  z1}, { x0,  z1},
    };
}
static inline std::vector<float> lShapeElevs(float bot, float top) {
    return {bot, bot, bot, bot, top, top, top, bot};
}

// Per-hole obstacle entry builder — shared across all hole headers.
// Defined here (once) because all HoleXX.h files are included into the
// same translation unit via CourseData.h.
static inline HoleConfig::ObstacleEntry obs(
    const std::string& type,
    float x, float y, float z,
    float sx, float sy, float sz,
    float rot = 0.0f)
{
    HoleConfig::ObstacleEntry e;
    e.type     = type;
    e.localPos = glm::vec3(x, y, z);
    e.scale    = glm::vec3(sx, sy, sz);
    e.rotation = rot;
    return e;
}
