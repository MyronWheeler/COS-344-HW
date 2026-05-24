#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct HoleConfig {
    int holeNumber = 1;
    int par        = 3;

    glm::vec3 position = glm::vec3(0.0f);
    float     rotation = 0.0f;          // Y-axis degrees

    std::vector<glm::vec2> boundaryPoints;  // 2-D outline in local space
    std::vector<float>     elevations;      // Y per boundary point (same size)

    bool hasStream = false;
    bool hasPond   = false;
    bool hasBridge = false;
    bool hasTunnel = false;

    std::vector<glm::vec2> streamPath;  // waypoints in local XZ
    float pondRadius = 2.0f;

    struct ObstacleEntry {
        std::string type;
        glm::vec3   localPos;
        glm::vec3   scale    = glm::vec3(1.0f);
        float       rotation = 0.0f;

        ObstacleEntry() = default;
        ObstacleEntry(const std::string &t, glm::vec3 p, glm::vec3 s, float r)
            : type(t), localPos(p), scale(s), rotation(r) {}
    };

    std::vector<ObstacleEntry> obstacles;
    std::vector<ObstacleEntry> decor;

    bool      hasWindmill     = false;
    glm::vec3 windmillLocalPos = glm::vec3(0.0f);
};
