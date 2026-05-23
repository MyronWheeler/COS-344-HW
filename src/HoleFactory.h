#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "HoleConfig.h"
#include "Terrain.h"
#include "Mesh.h"

class Shader;

// ---------------------------------------------------------------------------
// HoleNode — owns all renderable pieces for one hole
// ---------------------------------------------------------------------------
struct HoleNode {
    glm::mat4 worldTransform;   // translate + Y-rotate from HoleConfig

    std::unique_ptr<Terrain>                 terrain;
    std::vector<std::pair<Mesh, glm::mat4>>  streamSegments;
    std::unique_ptr<Mesh>                    pond;
    glm::mat4                                pondTransform;
    std::unique_ptr<Mesh>                    bridge;
    glm::mat4                                bridgeTransform;

    // Obstacle / decor transform list; caller looks up the right Object class
    struct PlacedObject {
        std::string type;
        glm::mat4   transform;
    };
    std::vector<PlacedObject> obstacles;
    std::vector<PlacedObject> decor;

    bool hasPond   = false;
    bool hasBridge = false;

    void draw(Shader &shader);

    // HoleNode is move-only (Terrain/Mesh are non-copyable)
    HoleNode()                           = default;
    HoleNode(HoleNode&&)                 = default;
    HoleNode& operator=(HoleNode&&)      = default;
    HoleNode(const HoleNode&)            = delete;
    HoleNode& operator=(const HoleNode&) = delete;
};

class HoleFactory {
public:
    static HoleNode build(const HoleConfig &config);
};
